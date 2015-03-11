#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>

import socket
import time
import traceback
import os
import sys

import servo

from multiprocessing import Value

from parameters import headers, closers, filtered, app_nr

stop_server = Value('i', False)

class MSCU(object):

    def __init__(self, host='', port=10000):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.bind((host, port))
        self.servos = {}
        for address in app_nr:
            self.servos[address] = servo.Servo(address)

    def run(self):
        self.socket.listen(1)
        self._welcome()
        # Number of connections counter
        counter = 0
        while True:
            if stop_server.value:
                sys.exit(0)
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
                            if data.startswith('#stop'):
                                stop_server.value = True
                                connection.close()
                                break
                            # Retrieve the message header
                            header, body = data[0], data[1:].rstrip()
                            whole_cmd, params_str = body.split('=')
                            # Retrieve the command and the command number
                            cmd, cmd_num = whole_cmd.split(':')
                            cmd_num = int(cmd_num)
                            # Retrieve the command parameters
                            all_params = [eval(param.strip()) for param in params_str.split(',')]
                            address, params = all_params[0], all_params[1:]
                        except Exception:
                            print MSCU.error_message(data)
                            # Send a general error message
                            connection.send(MSCU.error_message(data))
                            # An invalid command pass silently
                            continue

                        servo = self.servos[address]
                        # Print a general error message if we received an invalid command
                        if header not in headers or not hasattr(servo, cmd):
                            print MSCU.error_message(data)
                            # Send a general error message
                            connection.send(MSCU.error_message(data))
                            # An invalid command pass silently
                            continue

                        if not cmd in filtered:
                            print "\nReceived message from %s: %r" % (connection.getpeername(), data)
                        
                        # Call the appropriate command whose name is the string cmd
                        answers = getattr(servo, cmd)(cmd_num, *params)

                        if not cmd in filtered:
                            for answer in answers:
                                print "Answer from %s: %r" % (servo.name, answer)

                        for answer in answers:
                            connection.send('%s' %answer)
                            time.sleep(0.05)
                except (KeyboardInterrupt, SystemExit):
                    raise
                except:
                    raise

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

    @staticmethod
    def error_message(msg):
        return "Invalid message: %s" %msg

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

    @staticmethod
    def stop(server=('127.0.0.1', 10000)):
        sockobj = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sockobj.settimeout(2)
        sockobj.connect(server) 
        sockobj.sendall('#stop\r\n')


if __name__ == "__main__":
    mscu = MSCU()
    mscu.run()


