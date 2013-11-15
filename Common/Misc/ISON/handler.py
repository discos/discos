import argparse
import datetime
import time
import sys
import os
from ison_jobs import *
from targets import next_target, date_parser

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-l', '--lo_time', default=0.5, type=float, 
                        help="Sleeping time (in seconds) between two LO value settings.")
    parser.add_argument('-a', '--acquiring_time', default=3.0, type=float, 
                        help="Time (in seconds) between beckend.start() and backend.stop()")
    parser.add_argument('-n', '--cycles', default=2, type=int, 
                        help="Number of cycles")
    parser.add_argument('filename', nargs='?', help='file containing tle informations, if no file is given uses ISON tle')
    args = parser.parse_args()

    logfile = os.path.join('logs', 'ison--%s.log' % datetime.datetime.now().strftime("%Y_%m_%d-%H_%M_%S"))
    logging.basicConfig(filename=logfile, level=logging.DEBUG)
    # Arguments of Job: Job name, job, updating_time, delay, *args
    job_goOnSource = ForegroundJob('go on source', goOnSource, ())
    job_waitForTracking = ForegroundJob('wait for tracking', waitForTracking, ())
    job_acquireOnSource = ForegroundJob('acquire', acquire, args=('on source', args.acquiring_time))
    job_goOffSource = ForegroundJob('go off source', goOffSource, ())
    job_acquireOffSource = ForegroundJob('acquire', acquire, args=('off source', args.acquiring_time))

    lo_daemon = DaemonJob('update local oscillator', updateLOLoop, args.lo_time)
    lo_daemon()
    print next_target(args.filename, datetime.datetime.now())
    
    def cycle():
        print "-" * 30
        lo_daemon.doNotUpdate()
        job_goOnSource()
        job_waitForTracking()
        lo_daemon.update()
        time.sleep(2)
        job_acquireOnSource()
        lo_daemon.doNotUpdate()
        time.sleep(2)
        job_goOffSource()
        job_waitForTracking()
        lo_daemon.update()
        time.sleep(2)
        job_acquireOffSource()
        lo_daemon.doNotUpdate()
        print "-" * 30

    for i in range(args.cycles):
        try:
            cycle()
        except KeyboardInterrupt:
            print 'Goodby'
        except Exception, e: 
            print 'Unexpected error:', sys.exc_info()[0]
            logging.exception('Got exception on main handler')
        finally:
            lo_daemon.terminate()
            sys.exit(0)

