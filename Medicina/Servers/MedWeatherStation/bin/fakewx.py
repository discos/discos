import SocketServer
import datetime

class MyTCPHandler(SocketServer.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        # self.request is the TCP socket connected to the client
      msg=""
      done=0
      crec=0
      while not done:	
            ch = self.request.recv(1)
	    if ch=='':
			self.data=""
			break
#	    if ch=='\n':
#			done=1
	    msg=msg+ch
      	    self.data=msg
            print 'received:'+ self.data
            print len(self.data)
            if self.data=='poppiexit\n':
		    self.request.close()
	    	    reading=0
	    	    done=1
		    break 
    	          
            if self.data=='spettro\n':
		  now=datetime.datetime.utcnow()
		  tt=now.timetuple()
#	          self.request.send('0132  dr21 %s +42:19:44 2000.0 300.00 5  21964.000   0.0 kkc 80.047 60.416 17:23:13 +44:37:55 0 0 31.5 1018.0 90.0 \n')
            	  print "received spettro, sent back fake string"     
		  	  
            	  self.request.send('0132  dr21 %s +42:19:44 2000.0 300.00 5  21964.000   0.0 kkc 80.047 60.416 17:23:13 +44:37:55 0 0 %f %f %f 22\n' % (datetime.datetime.utcnow().strftime("%H:%M:%S"),tt[3]+.04,tt[4]+.99,tt[5]+.43))
            	  self.data=""
	          msg=""
	    
if __name__ == "__main__":
    HOST, PORT = "0.0.0.0", 5002

    # Create the server, binding to localhost on port 9999
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()

