#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>

import socket
import traceback
import os
import sys

import commands
import posutils

from parameters import headers, closers, commands, app_nr, filtered


class MSCU(object):

    def __init__(self, host='', port=10000):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.bind((host, port))
        self.set_response_policy()

        db = posutils.PositionDB()
        db.initialize()

    def set_response_policy(self, policy='expected'):
        if policy in ('expected', 'mixed'):
            self.response_policy = policy
        else:
            raise ValueError('response policy %s not allowed' %policy)

    def run(self):
        self.socket.listen(1)
        self._welcome()
        # Number of connections counter
        counter = 0
        while True:
            try:
                connection, clientaddr = self.socket.accept()
                counter += 1 
                print "\n%d. Got connection from %s" %(counter, connection.getpeername())
            except KeyboardInterrupt:
                raise
            except:
                traceback.print_exc()
                continue

            # Clean up old children
            MSCU.reap()

            # Fork a process for this connection
            pid = os.fork()

            if pid:
                # This is the parent process. Close the child's socket and 
                # return to the top of the loop.
                connection.close()
                continue
            else:
                # From here on, this is the child. 
                self.socket.close() # Close the parent's socket
                try:
                    buff = ''
                    while True:
                        byte = connection.recv(1)
                        buff = byte if byte in headers else buff + byte
                        if buff.startswith(headers):
                            if buff.endswith(closers):
                                data = buff
                                buff = '' 
                                # Go to the try block
                            else:
                                continue 
                        else:
                            buff = ''
                            continue
                        
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
                            connection.send(cause + MSCU.error_message(data))
                            # An invalid command pass silently
                            continue

                        # Print a general error message if we received an invalid command
                        if header not in headers or cmd not in commands:
                            cause = "Unexpected command or header.\n"
                            print cause
                            # Send a general error message
                            connection.send(cause + MSCU.error_message(data))
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
                            connection.send(cause + MSCU.error_message(data))
                            # An invalid command pass silently
                            continue
                            
                        # Call the appropriate command whose name is the string cmd
                        for ans in getattr(commands, cmd)(cmd_num, app_num, self.response_policy, *params):
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

    def _welcome(self):
        print "*"*43
        print "MSCU_simulator - Waiting for connections..."
        print "*"*43
        print "Response type: %s\n" %self.response_policy

    @staticmethod
    def error_message(msg):
        return "message %s discarded" %msg

    @staticmethod
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
    mscu = MSCU()
    try:
        mscu.run()
    finally:
        db = posutils.PositionDB()
        db.clear()


