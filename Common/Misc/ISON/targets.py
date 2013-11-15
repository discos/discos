#!/usr/bin/env python
#coding=utf-8
# Author: Marco Bartolini
 
from __future__ import with_statement 
import logging
import sys
import datetime
 
import ephem

def date_parser(arg):
    return datetime.datetime.strptime(arg, "%d/%m/%Y_%H:%M:%S")
 
SRT = ephem.Observer()
SRT.lat = "39:29:34"
SRT.lon = "09:14:42"
SRT.elevation = 700 
 
def next_target(filename, delay=0):
    if filename:
        with open(filename, "rt") as tle_file:
            lines = tle_file.readlines()
        index = 0
        #search for the first not empty and uncommented line in the file
        while not lines[index] or lines[index].startsWith('#'):
            index += 1
        tle = lines[index]
    else: #default is ISON tle ... 
        tle = "C/2012 S1 (ISON),h,11/28.7757/2013,62.3948,295.6536,345.5636,1.000002,0.012446,2000,10.0,3.2"
    logging.info("using tle: %s" % (tle,))
    try:
        source = ephem.readdb(tle)
    except Exception, e:
        logging.exception('Got exception on pyephem')
        sys.exit()
    source_name = source.name.strip().split()[0]
 
    # SRT.date = datetime.datetime.now() + datetime.timedelta(seconds=delay)
    SRT.date = datetime.datetime.now()
    source.compute(SRT)
    # rising_time = SRT.next_rising(source).datetime() # CHECK: fix the time
    # if rising_time > SRT.date:
    #     logging.warning("rising time is: %s" % rising_time)
    #     # start_date = source.rise_time.datetime()
    #     SRT.date = rising_time # CHECK: time computed before the if
    #     source.compute(SRT)
    return source.ra, source.dec
 
