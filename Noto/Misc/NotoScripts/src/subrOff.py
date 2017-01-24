#!/usr/bin/env python

# This is a python test program that rchanges the current offsets of Noto SCU
#who                                   when           what
#Andrea Orlati(a.orlati@ira.inaf.it)   24/01/2017     Creation

import getopt, sys

import string
import math
import time
import socket
import struct
from math import *
import os
        
def usage():
	print "subrOff [-h|--help] [-x val] [-y val] [-z val]"
	print ""
	print "[-h|--help]      displays this help"
	print "[-x val]         offset for x axis"
	print "[-y val]         offset for y axis"
	print "[-z val]         offset for z axis"

def main():
    
	try:
		opts, args = getopt.getopt(sys.argv[1:],"hx:y:z:",["help"])
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(1)
        
	pipeName="/tmp/subrPipe"

	X=0.0
	Y=0.0
	Z=0.0

	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
			sys.exit()
		elif o in ("-x"):
			X = float(a)
		elif o in ("-y"):
			Y = float(a)
		elif o in ("-z"):
			Z = float(a)

	print "The offsets are X: %lf, Y: %lf, Z, %lf" % (X,Y,Z)

	print "Preparing pipe....."
	if not os.path.exists(pipeName):
		print "Creating pipe"
		os.mkfifo(pipeName,0777)
		print "Created"
	else:
		print "pipe already exists"

	print "Opening pipe......"
	pipeOut=open(pipeName,'w', 0)
	print "Pipe openened for writing......"
	try:
		pipeString="%lf,%lf,%lf"%(X,Y,Z)
		pipeOut.write(pipeString)
		print "command sent!"
		pipeOut.close()
	except Exception, ex:
		print ex

if __name__=="__main__":
   main()
    



