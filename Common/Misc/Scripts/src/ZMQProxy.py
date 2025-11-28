#!/usr/bin/env python
import re
import json
import socket
from pathlib import Path
from argparse import ArgumentParser
from collections import defaultdict
from collections.abc import Mapping
import zmq
import zmq.auth
from zmq.auth import load_certificate
from zmq.auth.thread import ThreadAuthenticator

RX_BOOT = re.compile(r'^[0-9A-Za-z]{4}_[^\s]+$')

def deep_merge(dst: dict, src: Mapping) -> dict:
    for k, v in src.items():
        if isinstance(v, Mapping) and isinstance(dst.get(k), Mapping):
            deep_merge(dst[k], v)
        else:
            dst[k] = v

parser = ArgumentParser()
parser.add_argument(
    '-f', '--front-end-port',
    type=int,
    required=False,
    default=16001,
    help='port on which the DISCOS components will send their messages'
)
parser.add_argument(
    '-b', '--back-end-port',
    type=int,
    required=False,
    default=16000,
    help='port on which the clients will connect to receive the messages'
)
args = parser.parse_args()

if args.front_end_port == args.back_end_port:
    parser.error('FRONT_END_PORT and BACK_END_PORT cannot be the same port!')

curve_directory = Path.home() / ".curve" / "discos" / "telemetry" / "server"
authorized_directory = curve_directory / "authorized"
server_pair = curve_directory / "identity.key_secret"
server_public, server_secret = load_certificate(server_pair)

# Data flows from frontend (DISCOS components endpoint) to backend (clients).
# We use a TCP/IP socket listening to only the localhost interface.

# Create the zmq context
context = zmq.Context()

# Create the frontend socket, subscriber
frontend = context.socket(zmq.SUB)
frontend.setsockopt(zmq.RCVHWM, 2000)
frontend.setsockopt(zmq.SUBSCRIBE, b'')
frontend.setsockopt(zmq.LINGER, 0)
frontend.curve_secretkey = server_secret
frontend.curve_publickey = server_public
frontend.curve_server = True

# Create the backend socket, publisher
backend = context.socket(zmq.XPUB)
backend.setsockopt(zmq.XPUB_VERBOSE, 1)
backend.setsockopt(zmq.IMMEDIATE, 1)
backend.setsockopt(zmq.LINGER, 0)
backend.setsockopt(zmq.TCP_KEEPALIVE, 1)
backend.setsockopt(zmq.TCP_KEEPALIVE_IDLE, 60)
backend.setsockopt(zmq.TCP_KEEPALIVE_INTVL, 15)
backend.setsockopt(zmq.TCP_KEEPALIVE_CNT, 4)
backend.setsockopt(zmq.HEARTBEAT_IVL, 5000)
backend.setsockopt(zmq.HEARTBEAT_TIMEOUT, 15000)
backend.setsockopt(zmq.HEARTBEAT_TTL, 10000)
backend.setsockopt(zmq.SNDHWM, 30)

auth = ThreadAuthenticator(context)
auth.start()
auth.configure_curve(domain='*', location=authorized_directory)

# Create the cache dictionary
cache = defaultdict(dict)

# To handle a cache, we need to perform some polling to the zmq system library
poller = zmq.Poller()
poller.register(frontend, zmq.POLLIN)
poller.register(backend, zmq.POLLIN)

# Finally bind the sockets
backend.bind(f'tcp://*:{args.back_end_port}')
frontend.bind(f'tcp://*:{args.front_end_port}')

print('ZMQ proxy started')
print(
    f'Receiving messages from DISCOS on all network interfaces ,'
    f'port {args.front_end_port}'
)
print(
    f'Relaying messages to clients on all network interfaces, '
    f'port {args.back_end_port}'
)

try:
    while True:
        # The poller object will block until a new event comes
        events = dict(poller.poll())

        # The event involves the frontend socket
        # new message from a DISCOS # component
        if frontend in events:
            try:
                # Receive the message without copying it
                message = frontend.recv_multipart(copy=False)

                # Relay the message via the backend socket to the clients
                backend.send_multipart(message, copy=False)

                # Now that the message was relayed, we can update the cache
                # We copy the inner message buffer into topic and payload
                topic_f, payload_f = message
                topic = memoryview(topic_f).tobytes().decode("ascii", "ignore")
                payload = memoryview(payload_f).tobytes()

                # Update the cached dictionary
                if topic not in cache or not isinstance(cache[topic], dict):
                    cache[topic] = {}
                deep_merge(cache[topic], json.loads(payload))
            except ValueError:
                # Not enough values to unpack, malformed message
                pass

        # The event involves the backend socket
        # new subscription or unsubscription from a client
        if backend in events:
            # Retrieve the event object
            event = backend.recv()

            # If the event object starts with 1 it is a new subscription
            if event[0] == 1:
                # Retrieve the topic to which the client has subscribed
                topic = event[1:].decode()

                # If the topic contains and underscore, it means a client is
                # asking for the cached dictionary of the given topic
                # Before the underscore there should be a random integer
                # between 0 and 100. By reusing this random integer as a topic
                # we avoid sending the cached element to the main topic, which
                # will trigger an update to other clients even if it not
                # needed.
                if RX_BOOT.match(topic):
                    # Retrieve the dictionary key by splitting the received
                    # topic subscription.
                    key = topic.split('_', 1)[1]

                    # If we already cached a value, send it to the client via
                    # the subscribed topic (ex.: '56_mount', we retrieve
                    # 'mount' from the cache)
                    payload = json.dumps(cache.get(key, {}), separators=(",", ":"))
                    backend.send_multipart([
                        topic.encode("ascii"),
                        payload.encode("utf-8")
                    ])
except KeyboardInterrupt:
    # Service stopped or CTRL-C pressed
    pass

print('\nZMQ proxy stopped')
