#!/usr/bin/env python
# Author: Andrea Orlati <a.orlati@ira.inaf.it>

# **************************************************************************************************** 
# DISCOS Project                                                                                       
#                                                                                                      
# This code is under GNU General Public License (GPL).             
# Who                                when            What                          
# Andrea Orlati(aorlati@ira.inaf.it) 13/10/2016     Creation                                       

"""
This module provides a dummy server  a dummy server. 

How To Use This Module
======================
(See the individual classes, methods, and attributes for details)

1. Import it: ``import TCPServer``

2. Create a BoardServer object::

       rs = TCPServer(cmd, answ, defAnsw)

3. Run the server::

       rs.run()

An alternative way is to execute directly this module from a shell::

       ./TCPserver.py

To stop the server::

       rs.stop()
"""

import socket, traceback, os, sys
import binhex
from multiprocessing import Value

stop_server = Value('i', False)

class TCPServer:
   
	def __init__(self, cmd, answ, defAnsw, term, port):
		"""
		Initialize

   	Parameters:
		
		- `cmd`: a strings or commands that the server recognizes
		- 'answ': server answers to the defined commands
		- 'defAnsw': a string sent as answer to the clinet in case the command is not recognized
		- `port`: the port the server is listening for connections.
		"""

		self.host = socket.gethostbyname(socket.gethostname())
		self.command=cmd
		self.answer=answ
		self.defaultAnswer=defAnsw
		self.simPort=port
		self.terminator=term
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

	def run(self):
		self.s.bind((self.host, self.simPort))
		self.s.listen(1)
		
		print "*"*40
		print "TCPServer - Waiting for connections from %s port %d..." % (self.host,self.simPort)
		print "Valid commands are: %s" % (self.command)
		print "*"*40
		error=False
		try:
			connection, clientaddr = self.s.accept()
			print "Got connection from %s port %d" % (connection.getpeername())
		except KeyboardInterrupt:
			raise			
		except:
			error=True
		finally:
			self.s.close()
	
		if error:
			sys.exit(0)

		try:
			while True:
				if stop_server.value:
					break
				data = connection.recv(1024)
				if '#stop' in data:
					stop_server.value = True
					connection.close()
					break
				answer=self.defaultAnswer
				if(data):
					#print "Received: %s" % data
					if self.terminator!=32:
						termPos=data.find(chr(self.terminator))
						if termPos>0:
							strcmp=data[:termPos]
							if strcmp in self.command:
								index=self.command.index(strcmp)
								answer=self.answer[index]
						else:
							pass
					#print "Sending message: %s" % (answer)
					connection.send(answer)			
		except KeyboardInterrupt:
			raise
		except:
			pass
		finally:
			connection.close()

		print "Exiting...."
		sys.exit(0)

	@staticmethod
	def stop():
		print "shutdown by external command..."
		stop_server.value = True

if __name__ == "__main__":
	bs = TCPServer()
	bs.run()
