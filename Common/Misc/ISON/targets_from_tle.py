#!/usr/bin/env python
#coding=utf-8
# Author: Marco Bartolini
 
from __future__ import with_statement 
import datetime
import logging
import sys
 
import ephem

try:
    import astropy
    assert(astropy.version.major >= 0)
    assert(astropy.version.minor >= 3)
    from astropy.coordinates import Angle
    from astropy import units as u

    def fmt_dec(angle):
        a = Angle(angle, u.deg)
        return a.to_string(unit=u.deg, decimal=True, precision=5) + 'd'

    def fmt_deg(angle):
        a = Angle(angle, u.deg)
        return a.to_string(unit=u.deg, sep=':', precision=5)

    def fmt_hms(angle):
        a = Angle(angle, u.deg)
        return a.to_string(unit=u.hourangle, sep=':', precision=5) + 's'
    
    USING_ASTROPY = True
except:
    USING_ASTORPY = False

def date_parser(arg):
    return datetime.datetime.strptime(arg, "%d/%m/%Y_%H:%M:%S")
 
SRT = ephem.Observer()
SRT.lat = "39:29:34"
SRT.lon = "09:14:42"
SRT.elevation = 700 
 
def cmd_line():
    import argparse
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('--start', dest='start_date', type=date_parser,
                            default=datetime.datetime.now(),
                            help="start date: dd/mm/YYYY_hh:mm:ss")
    arg_parser.add_argument('--stop', dest='stop_date', type=date_parser,
                            help="stop date: dd/mm/YYYY_hh:mm:ss")
    arg_parser.add_argument('--delta', dest='delta', 
                            type=lambda(x):datetime.timedelta(seconds=float(x)),
                            help="time delta between each coordinate in seconds")
    arg_parser.add_argument('-v', dest='verbose', action='store_true',
                            default=False, help='set verbose mode')
    arg_parser.add_argument('--scan_mode', dest='scanmode', default='TLESCAN',
                            help='the scanmode name in the schedule')
    arg_parser.add_argument('filename', nargs='?', 
                            help='file containing tle informations, if no file \
                            is given uses ISON tle')
    options = arg_parser.parse_args()
    if options.verbose:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)

    if not USING_ASTROPY:
        logging.warning("not using astropy for coordinates formatting")

    if options.filename:
        with open(options.filename, "rt") as tle_file:
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
        #pyephem exit with an error
        logging.error(str(e))
        sys.exit()
    source_name = source.name.strip().split()[0]
 
    start_date = options.start_date
    stop_date = options.stop_date
 
    #check if the source is visible in given time interval
    #adjust start and stop times accordingly
    SRT.date = options.start_date
    source.compute(SRT)
    rising_time = SRT.next_rising(source).datetime() # CHECK: fix the time
    if rising_time > options.start_date:
        logging.warning("rising time is: %s" % rising_time)
        # start_date = source.rise_time.datetime()
        SRT.date = rising_time # CHECK: time computed before the if
        source.compute(SRT)
    next_setting = SRT.next_setting(source)
    if next_setting.datetime() < options.stop_date:
        logging.warning("set time is: %s" % (next_setting,))
        # stop_date = source.set_time.datetime()
        stop_date = SRT.next_setting(source).datetime() # CHECK: set_time deprecated
 
    output_file = open("targets.txt", "wt")
    output_file.write("#SNAME\tSCANTYPE\tFRAME\tLONGITUDE\tLATITUDE\tDATETIME\tSIDEREALTIME\n")
    #get coordinates at specified intervals until stop
    while SRT.date.datetime() < stop_date:
        source.compute(SRT)
        if not USING_ASTROPY:
            target_string = "%s\t%s\tEQ\t%s\t%s\t%s\t%s\n" % (source_name,
                                                            options.scanmode,
                                                            str(float(source.ra)) + 'd',
                                                            str(float(source.dec)) + 'd',
                                                            str(SRT.date),
                                                            str(SRT.sidereal_time()))
        else:
            target_string = "%s\t%s\tEQ\t%s\t%s\t%s\t%s\n" % (source_name,
                                                            options.scanmode,
                                                            fmt_dec(source.ra),
                                                            fmt_dec(source.dec),
                                                            str(SRT.date),
                                                            str(SRT.sidereal_time()))
        output_file.write(target_string)
        logging.debug(target_string)
        SRT.date = SRT.date.datetime() + options.delta
        
    output_file.close()
 
if __name__ == '__main__':
    cmd_line()
