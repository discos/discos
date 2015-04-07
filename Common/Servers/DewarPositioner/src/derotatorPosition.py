#!/usr/bin/env python

from Acspy.Clients.SimpleClient import PySimpleClient
import time

client = PySimpleClient()

dp = client.getComponent('RECEIVERS/DewarPositioner')

counter = 0

while True:
    position = dp.getPosition()
    print "%d -> %.2f" %(counter, position)
    counter += 1
    time.sleep(3)
