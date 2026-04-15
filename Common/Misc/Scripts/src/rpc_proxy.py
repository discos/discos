#!/usr/bin/env python
import re
import uuid
import time
import json
from pathlib import Path
import zmq
from zmq.utils import z85
from zmq.auth import load_certificate
from ClientErrorsImpl import BusyErrorExImpl, DISCOSOfflineErrorExImpl
from IRAPy import ComponentProxy
from ZMQLibrary import ZMQTimeStamp


BUSY = BusyErrorExImpl()


class SchedulerClient:

    ERROR_RE = re.compile(
        r"""
        ^\(
            type:(?P<type>\d+)
            \s+
            code:(?P<code>\d+)
        \):
        \s*
        (?P<answer>.+)
        """,
        re.VERBOSE
    )

    def __init__(self):
        self._scheduler = ComponentProxy(
            component_idl="IDL:alma/Management/Scheduler:1.0"
        )

    def command(self, cmd_dict):
        retval = {"executed": False}
        cmd = self.build_command(cmd_dict)
        try:
            executed, answer = self._scheduler.command(cmd)
            retval["executed"] = executed
            if executed:
                command, answer = answer.split('\\', 1)
                retval["command"] = command
                answer = answer.rstrip("\\")
                if answer:
                    values = answer.split(";")
                    for index, value in enumerate(values):
                        if '.' in value:
                            try:
                                values[index] = float(value)
                            except ValueError:
                                pass
                        else:
                            try:
                                values[index] = int(value)
                            except ValueError:
                                pass
                    if len(values) == 1:
                        retval["answer"] = values[0]
                    elif len(values) > 1:
                        retval["answer"] = values
            else:
                command, answer = answer.split('?')
                retval["command"] = command
                retval.update(self.parse_error(answer))
        except DISCOSOfflineErrorExImpl as impl:
            retval.update({
                "error_trace": [{
                    "message": impl.getDescription(),
                    "category": impl.getErrorType(),
                    "code": impl.getErrorCode()
                }]
            })
        retval["timestamp"] = ZMQTimeStamp.now()
        return retval

    @staticmethod
    def parse_error(error_string):
        error_list = []

        if not error_string:
            return {"error_trace": []}

        lines = error_string.strip().split('\n')

        for line in lines:
            line = line.strip()
            if not line:
                continue

            m = SchedulerClient.ERROR_RE.match(line)
            if m:
                error_list.append({
                    "message": m.group("answer").strip(),
                    "category": int(m.group("type")),
                    "code": int(m.group("code"))
                })
            else:
                continue

        return {
            "error_trace": error_list
        }

    @staticmethod
    def build_command(cmd_dict):
        cmd = cmd_dict["command"]
        args = cmd_dict.get("args")
        if args is not None:
            cmd += "="
            for index, arg in enumerate(args):
                if isinstance(arg, list):
                    args[index] = ";".join([str(x) for x in arg])
            cmd += ",".join([str(x) for x in args])
        return cmd


def load_authorized_keys(authorized_directory: Path) -> set[bytes]:
    authorized = set()
    for keyfile in authorized_directory.glob("*.key"):
        public_key, _ = load_certificate(str(keyfile))
        authorized.add(public_key.decode("ascii"))
    return authorized


if __name__ == "__main__":
    curve_directory = Path.home() / ".config" / "discos" / "rpc" / "server"
    authorized_directory = curve_directory / "authorized"
    server_pair = curve_directory / "identity.key_secret"
    server_public, server_secret = load_certificate(server_pair)
    admin_keys = load_authorized_keys(authorized_directory / "admin")
    user_keys = load_authorized_keys(authorized_directory / "user")

    context = zmq.Context()

    zap = context.socket(zmq.REP)

    socket = context.socket(zmq.ROUTER)
    socket.curve_secretkey = server_secret
    socket.curve_publickey = server_public
    socket.curve_server = True
    socket.setsockopt(zmq.ROUTER_NOTIFY, zmq.RouterNotify.CONNECT | zmq.RouterNotify.DISCONNECT)
    socket.setsockopt(zmq.HEARTBEAT_IVL, 1000)
    socket.setsockopt(zmq.HEARTBEAT_TIMEOUT, 1000)
    socket.setsockopt(zmq.HEARTBEAT_TTL, 10000)

    poller = zmq.Poller()
    poller.register(socket, zmq.POLLIN)
    poller.register(zap, zmq.POLLIN)

    zap.bind("inproc://zeromq.zap.01")
    socket.bind("tcp://*:16010")

    print("RPC server listening")

    clients = {}
    orphan_routing_ids = set()
    orphan_user_ids = set()

    current_admin = None
    current_user = None

    pending_client = None
    pending_since = None

    scheduler = SchedulerClient()

    try:
        while True:
            events = dict(poller.poll())

            if socket in events:
                req = socket.recv_multipart(copy=False)
                routing_id, sep, payload = (req + [None])[:3]

                if payload is None:
                    # Notification from the router socket
                    routing_id = routing_id.bytes
                    if routing_id in clients:
                        # Disconnect event
                        clients.pop(routing_id)
                        if current_user == routing_id:
                            current_user = None
                        if current_admin == routing_id:
                            current_admin = None
                    else:
                        # Connect event
                        if pending_client:
                            clients[routing_id] = pending_client
                            if pending_client in admin_keys:
                                if current_admin is None:
                                    current_admin = routing_id
                            pending_client = None
                        else:
                            # Somehow we received a connection event without a
                            # valid authorization, maybe a timeout event?
                            # We put it in orphans
                            orphan_routing_ids.add(routing_id)
                else:
                    # Actual message from a client
                    routing_id = routing_id.bytes
                    client_key = payload["User-Id"]

                    execute = False

                    if routing_id not in clients:
                        # Orphan, should never happen
                        pass
                    elif client_key in admin_keys:
                        if clients.get(routing_id) == clients.get(current_admin):
                            execute = True
                    elif client_key in user_keys:
                        if current_admin is not None:
                            # This is a regular user but an admin is
                            # controlling
                            execute = False
                        elif current_user is None:
                            # No regular user has control, the current client
                            # claims it
                            current_user = routing_id
                            execute = True
                        elif current_user == routing_id:
                            # The current client is the control client
                            execute = True

                    payload = json.loads(payload.bytes)

                    if execute:
                        answer = scheduler.command(payload)
                    else:
                        answer = {
                            "executed": False,
                            "command": payload.get("command"),
                            "error_trace": [{
                                "category": BUSY.getErrorType(),
                                "code": BUSY.getErrorCode(),
                                "message": BUSY.getDescription()
                            }]
                        }
                        if answer["command"] is None:
                            del answer["command"]

                    answer["timestamp"] = ZMQTimeStamp.now()

                    socket.send_multipart([
                        routing_id,
                        b"",
                        json.dumps(
                            answer,
                            separators=(",", ":")
                        ).encode()
                    ])

            if pending_client and time.monotonic() > pending_since + 1:
                orphan_user_ids.add(pending_client)
                pending_client = pending_since = None

            if zap in events and not pending_client:
                req = zap.recv_multipart()
                version, sequence, _, _, _, mechanism, *rest = req
                authorized = False
                if mechanism == b"CURVE":
                    client_key = z85.encode(*rest)
                    client_key_str = client_key.decode("ascii")
                    if client_key_str in admin_keys or \
                            client_key_str in user_keys:
                        authorized = True
                if authorized:
                    answer_code = b"200"
                    answer = b"OK"
                    answer_key = client_key
                    pending_client = client_key_str
                    pending_since = time.monotonic()
                else:
                    answer_code = b"400"
                    answer = b"NO ACCESS"
                    answer_key = b""
                zap.send_multipart(
                    [version, sequence, answer_code, answer, answer_key, b""]
                )
    except KeyboardInterrupt:
        print()
    print("RPC server shutdown")
