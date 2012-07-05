#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

"""
This module defines a Minor Servo Control Unit (MSCU) simulator. 
It is useful mainly for testing a MinorServo ACS component or a 
MSCU GUI client. The real MSCU server was developed by Franco Fiocchi
in Medicina (BO) and provides an high level API to communicate with 
minor servos hardware of SRT.
This module defines the following classes:

- `MSCU`, a Minor Servo Control Unit server

Functions:

- `error_message(command)`: return a generic error message for the command
- `reap()`: collect any child processes that may be outstanding

Global variables used to make and to parse messages (defined in parameters.py):

- `headers`: allowed headers of messages
- `closers`: allowed closers of messages
- `commands`: allowed MSCU high level commands
- `app_nr`: dictionary that hold the address of every minor servo as
   a key and a *human name* as a value
- `response_types`: type of response to send to a client, used for testing
   wrong or unexpected responses


How To Use This Module
======================
(See the individual classes, methods, and attributes for details.)

1. Import it: ``import mscu`` or ``from mscu import ...``.

2. Create a MSCU object::

       mscu = MSCU(host, port, response_type)

3. Run the server::

       mscu.run()

An alternative way is to execute directly this module from a shell::

       ./mscu.py
"""


import socket, traceback, os, sys
import answer
import posutils
from parameters import headers, closers, commands, time_stamp, app_nr, number_of_axis, \
        response_types, db_name, filtered


class MSCU:
    """
    A Minor Servo Control Unit (MSCU) simulator.
    
    The MSCU is a server that provides high level APIs to communicate
    whit minor servos of SRT.

    The MSCU is started with the `run()` method, which expects connections
    from clients and make and send responses.
    """

    def __init__(self, host='', port=10000, response_type='expected_ack'):
        """
        Initialize a `MSCU` object.

        Parameters:

        - `host`: a string, the IP address allowed to make request. All IPs to default.
        - `port`: a positive integer, the port the MSCU lists for requests.
        - `response_type`: a string; only values in `response_types` are allowed.
        """
        self.host = host
        self.port = port
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.response_type = response_type
        db = posutils.PositionDB()
        db.initialize()

    def __setattr__(self, name, value):
        """Only certain values are allowed for ``response_type`` attribute."""
        if name == "response_type":
            if value not in response_types:
                raise AttributeError("%s is not a valid response_type. Allowed values: %s" \
                        % (value, response_types))
            else:
                # Use the attribute dictionary to avoid loops
                self.__dict__[name] = value 
        else:
            # Use the attribute dictionary to avoid loops
            self.__dict__[name] = value

    def run(self):
        """Run the MSCU server."""
        self.s.bind((self.host, self.port))
        self.s.listen(1)
        # Display a welcome message
        print "*"*43
        print "MSCU_simulator - Waiting for connections..."
        print "*"*43
        print "Response type: %s\n" %self.response_type

        # Number of connections counter
        counter = 0
        while True:
            try:
                connection, clientaddr = self.s.accept()
                counter += 1 
                print "\n%d. Got connection from %s" %(counter, connection.getpeername())
            except KeyboardInterrupt:
                raise
            except:
                traceback.print_exc()
                continue

            # Clean up old children
            reap()

            # Fork a process for this connection
            pid = os.fork()

            if pid:
                # This is the parent process. Close the child's socket and return to the top of the loop.
                connection.close()
                continue
            else:
                # From here on, this is the child
                # Close the parent's socket
                self.s.close()

                # Process the data
                try:
                    while True:
                        data = connection.recv(2048)

                        try:
                            # Retrieve the message header
                            header, body = data[0], data[1:].rstrip()
                            whole_cmd, params_str = body.split('=')
                            # Retrieve the command and the command number
                            cmd, cmd_num = whole_cmd.split(':')
                            cmd_num = int(cmd_num)
                            # Retrieve the command parameters
                            all_params = [eval(param.strip()) for param in params_str.split(',')]
                            app_num, params = all_params[0], all_params[1:]
                        except:
                            cause = "Invalid syntax!\n"
                            print cause
                            # Send a general error message
                            connection.send(cause + error_message(data))
                            # An invalid command pass silently
                            continue

                        # Print a general error message if we received an invalid command
                        if header not in headers or cmd not in commands:
                            cause = "Unexpected command or header.\n"
                            print cause
                            # Send a general error message
                            connection.send(cause + error_message(data))
                            # An invalid command pass silently
                            continue

                        if not cmd in filtered:
                            print "\nReceived message from %s: %r" % (connection.getpeername(), data)
                        
                        try:
                            servo_type = app_nr[app_num]
                        except KeyError:
                            cause = "Unexpected application number. Allowed values: %s\n" % app_nr.keys()
                            print cause
                            # Send a general error message
                            connection.send(cause + error_message(data))
                            # An invalid command pass silently
                            continue
                            
                        # Call the appropriate function whose name is the string cmd
                        for ans in getattr(answer, cmd)(cmd_num, app_num, self.response_type, *params):
                            if not cmd in filtered:
                                print "Answer from %s: %r" % (servo_type, ans)
                            connection.send('%s' %ans)
                except (KeyboardInterrupt, SystemExit):
                    raise
                except:
                    pass

                # Close the connection
                try:
                     connection.close()
                except KeyboardInterrupt:
                    raise
                except:
                    traceback.print_exc()

                # Done handling the connection. Child process *must* terminate
                # and not go back to the top of the loop
                sys.exit(0)

def error_message(command):
    """Return a generic error message."""
    return "MSCU discards %r message and still waiting next command" %command

def reap():
    """Collect any child processes that may be outstanding."""
    while True:
        try:
            result = os.waitpid(-1, os.WNOHANG)
            if not result[0]: 
                break
        except:
            break
        print "Reaped process %d" % result[0]


if __name__ == "__main__":
    # mscu = MSCU(response_type="mixed")
    mscu = MSCU(response_type="expected_ack")
    mscu.run()


