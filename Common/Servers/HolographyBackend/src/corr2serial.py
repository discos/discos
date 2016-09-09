#!/usr/bin/env python

# (C) 2009 Giovanni Busonera <giovannibusonera@gmail.com>
# Interface to a FPGA based correlator
# based on PySerial package

import sys
import os
import serial
import optparse
import datetime


from math import sqrt
from math import atan
from math import atan2
from math import log10
####################
# CORRELATOR CLASS #
####################
class Correlator:
    def __init__(self):
        self.results = { 'x_2' : 0, 'y_2' : 0, 'xy' : 0, 'xy90' : 0, 'y90_2' : 0, 'y_y90' : 0,'modNrm':0.,'PwrInX2':0.,'PwrInY2':0.}
        self.coeff   = { 'real' : 0.00, 'imm' : 0.00 }
        self.buffer  = ""
        self.ser     = serial.Serial()
        self.isConnected=False
    
    def connect(self, portname, baudrate):
        
        conn_byte = 'c'
        ack_byte  = 'A'

        self.ser.port = portname
        self.ser.baudrate = baudrate
        self.ser.timeout = 20
        
        if self.ser.isOpen():
            self.ser.close()

        self.ser.open()
        self.ser.flushInput()
    
        self.ser.write(conn_byte)
        
        rx_byte = self.ser.read(1)
        if len(rx_byte) != 1 or rx_byte != ack_byte:
            sys.stderr.write("Connection Failed. rx_byte = %s, len = %d\n" % (rx_byte,len(rx_byte)))
            self.isConnected=False
            return -1
        self.isConnected=True
        
        return 0

    def reset(self):
        reset_byte = 'r'
        
        #No acknoledge is expected
        self.ser.write(reset_byte)

    def disconnect(self):
        self.ser.close()
        self.isConnected=False
        
    def set_samples(self, n_samples):
	send_sample = 't'
        ack_byte  = 'A'
        for i in xrange(4):
            q = n_samples / 256
            m = n_samples % 256
            n_samples = q
            self.ser.write(send_sample)
            self.ser.write(chr(m))
	    
	    rx_byte = self.ser.read(1)
            if len(rx_byte) != 1 or rx_byte != ack_byte:
                sys.stderr.write("Trasmission Failed. rx_byte = %s, len = %d\n" % (rx_byte,len(rx_byte)))
                return -1
            
    def run(self):
        start_byte = 's'
        buffer_size = 48
     	   
        self.ser.write(start_byte)
        self.ser.flushInput()
        self.buffer = self.ser.read(buffer_size)
        
        if len(self.buffer) != buffer_size:
            return -1
        else:
            return 0        

    def fixed2float(self,data, dim_data, q):
    	
    	if data == 2**(dim_data-1):
            return -1.00*2**(dim_data-q-1)
    
    	if data & 2**(dim_data-1) == 2**(dim_data-1):
            return -1.00 * ((2**dim_data)-data) / float(2**q)
    	else:
            return float(data/float(2**q))

    def getCoeff(self):
        sum_x_2   = 0
        sum_y_2   = 0
        sum_xy    = 0
        sum_xy90  = 0
        sum_y90_2 = 0
	sum_y_y90 = 0
        
        for i in xrange(8):
            sum_y_y90 = sum_y_y90 | (ord(self.buffer[i]) << 8*i)
            sum_y90_2 = sum_y90_2 | (ord(self.buffer[i+8]) << 8*i)
            sum_xy90 = sum_xy90 | (ord(self.buffer[i+16]) << 8*i)
            sum_xy = sum_xy | (ord(self.buffer[i+24]) << 8*i)
            sum_y_2 = sum_y_2 | (ord(self.buffer[i+32]) << 8*i)
            sum_x_2 = sum_x_2 | (ord(self.buffer[i+40]) << 8*i)
            
        self.results['x_2'] = self.fixed2float(sum_x_2, 64, 30)
        self.results['y_2'] = self.fixed2float(sum_y_2, 64, 30)
        self.results['xy'] = self.fixed2float(sum_xy, 64, 30)
        self.results['xy90'] = self.fixed2float(sum_xy90, 64, 30)
        self.results['y90_2'] = self.fixed2float(sum_y90_2, 64, 30)
        self.results['y_y90'] = self.fixed2float(sum_y_y90, 64, 30)

        self.results['t'] = datetime.datetime.now()
            
        self.coeff['real'] = self.results['xy'] / sqrt(self.results['y_2'])
 
        self.coeff['realNrm'] = self.results['xy'] / ( sqrt(self.results['y_2'])*sqrt(self.results['x_2']) )

        self.coeff['immNrm'] = self.results['xy90'] / ( sqrt(self.results['y90_2']) *sqrt(self.results['x_2']) )


        self.coeff['imm'] = self.results['xy90'] / sqrt(self.results['y90_2'])
        
        self.coeff['mod'] = sqrt(self.coeff['real']**2 + self.coeff['imm']**2) 

        self.coeff['phase'] = atan2 (self.coeff['imm'],self.coeff['real'])

        self.coeff['phase_deg']= (180 / 3.141592654) * self.coeff['phase']

        self.coeff['modNrm'] = sqrt(self.coeff['realNrm']**2 + self.coeff['immNrm']**2) 

        self.results['PwrInX2'] = 10 * log10 ( (1000/100) * ( sqrt ( self.results['x_2'] * (2**30) / (5000000) ) * (39.0625/1000000))**2)
        self.results['PwrInY2'] = 10 * log10 ( (1000/100) * ( sqrt ( self.results['y_2'] * (2**30) / (5000000) ) * (39.0625/1000000))**2)
	self.results['modNrm'] = sqrt(self.coeff['realNrm']**2 + self.coeff['immNrm']**2) 

    def get_real(self):
        return self.coeff['real']

    def get_real(self):
        return self.coeff['realNrm']

    def get_imm(self):
        return self.coeff['imm']

    def get_imm(self):
        return self.coeff['immNrm']


    def get_mod(self):
        return self.coeff['mod']

    def get_mod(self):
        return self.coeff['modNrm']

    def get_phase(self):
        return self.coeff['phase_deg']

    def get_poweX2(self):
        return self.coeff['PwrInX2']

    def get_poweY2(self):
        return self.coeff['PwrInY2']



    def printResults(self):

#         print "%10.4f\t\t" %self.results['x_2'], "% 10.4f\t\t" %self.results['y_2'], "%    10.4f\t\t" %self.results['xy'], "%    10.4f\t\t" %self.results['xy90'], "%    10.4f\t\t" %self.results['y90_2'], "%    10.4f\t\t" %self.results['y_y90'], "%     s" %self.results['t']
        print "%10.4f\t" %self.results['x_2'], "% 10.4f\t" %self.results['y_2'], "%   10.4f\t" %self.results['xy'], "%   10.4f\t" %self.results['xy90'], "%   10.4f\t" %self.results['y90_2'], "% 3.4f\t" %self.results['y_y90'], "% 10.4f\t" %self.coeff['mod'], "% 4.4f" %self.coeff['phase_deg'], "% 1.4f\t" %self.coeff['modNrm'], "% s" %self.results['t'], "% 3.3f" %self.coeff['PwrInX2'], "% 3.3f" %self.coeff['PwrInY2']
 
         

    #    print "x2    = %10.6f" % self.results['x_2']
    #    print "y2    = %10.6f" % self.results['y_2']
    #    print "xy    = %10.6f" % self.results['xy']
    #    print "xy90  = %10.6f" % self.results['xy90']
    #    print "y90_2 = %10.6f" % self.results['y90_2']
        
    def printCoeff(self, message):

         message = ""

    #    if message == "":
    #       sys.stdout.write("%10.6f\t%10.6f\n" % (self.coeff['real'], self.coeff['imm']))
    #    else:
    #        sys.stdout.write("%s\t%10.6f\t%10.6f\n" % (message, self.coeff['real'], self.coeff['imm']))
		