#!/usr/bin/env python
"""
Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>
"""
if __name__ == '__main__':
    import logging
    import argparse
    import datetime
    import sys
    from handler import Handler
    from models import Target
    from os.path import join, exists
    from os import mkdir

    # Parse the command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--simulate', action='store_true', help="Simulate the observation.")
    args = parser.parse_args()

    # Create a program configuration
    import config
    try:
        conf = config.Configuration(
            cycles=10, # Number of on/off cycles, without tacking in account calibrations
            acquisition_time=70, # Time between backend.acquire() and backend.stop()
            positioning_time=5.0, # The time the program updates the antenna position
            observer_name='SRT', # The observer name (SRT, Medicina, ...)
            op="C/2012 S1 (ISON),h,11/28.7757/2013,62.3948,295.6536,345.5636,1.000002,\
                    0.012446,2000,10.0,3.2", # Orbital parameters
            offset=1.0, # The offset in degrees (0, 360)
            reference=0.5, # LO(time) -> starting_acquisition_time + acquisition_time*reference
            simulate=args.simulate, # If True, the program simulates the observation
            datestr_format='%d/%b/%Y:%H:%M:%S', # [day/month/year:hour:minute:second]
            stats=True, # If True, the program saves some useful information
            horizons_file_name='26-27_horizons.txt', # Horizons file name
            lab_freq=23694.495, # Lab frequency (MHz)
            lower_freq=155.0, # Backend lower frequency (MHz)
            upper_freq=170.6, # Backend upper frequency (MHz)
            calibrations=10, # Percentage of calibrations (10 means 10% of cycles)
            ) 
    except (TypeError, AttributeError), e:
        print('CONFIGURATION ERROR: %s' %e)
        sys.exit(1)
    except Exception, e:
        import traceback
        print('UNEXPECTED ERROR: %s' %e)
        traceback.print_tb(sys.exc_info()[2])
        sys.exit(1)

    # Logging stuffs
    logdir = 'logs'
    if not exists(logdir):
        mkdir(logdir)

    utc = datetime.datetime.utcnow()
    file_name = '%s--%s.log' %(Target.getNickName(conf.op),utc.strftime("%Y_%m_%d-%H_%M_%S"))
    logfile = join(logdir, file_name)
    logging.basicConfig(filename=logfile, level=logging.DEBUG)

    # Instantiate the handler and start the program
    handler = Handler(conf)
    title = '\n' + handler.getObservationTitle()
    logging.info(title)
    print(title)
    try:
        handler.run()
    finally:
        if not conf.simulate:
            import shutil
            shutil.copyfile(logfile, join(handler.obs_path, file_name))
    
