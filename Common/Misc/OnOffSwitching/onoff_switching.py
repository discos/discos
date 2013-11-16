#!/usr/bin/env python
"""Use this module in order to run the handler.

In order to undestand properly this instruction, you should first have a look
at the `handler.py` documentation. 
If you want to execute the on/off cycle 4 times, with a beckend acquisition time of
30 seconds, with a reference of 0.5 (central acquisition time):

    $ python onoff_switching.py -c 4 -t 30 -r 0.5

A KeyboardInterrupt stops the execution.
If you want to execute the on/off cycle 100 times, with a beckend acquisition time of
10 seconds, with a reference of 0 (beginning of the acquisition time) and with a 
concurrent process that computes and updates the LO value, every 2 seconds:

    $ python onoff_switching.py -c 100 -t 10 -r 0 -d 2

For more information, look at the help:

    $ python onoff_switching.py --help # show the help

Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
The author wants to say thanks to: S.Poppi, M.Bartolini, A.Orlati, C.Migoni, and A.Melis
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""

if __name__ == '__main__':
    import argparse
    import datetime
    import logging
    from info import Observer, Target, observer_info
    from handler import Handler
    from os.path import join, exists
    from os import mkdir

    # Parse the command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--cycles', default=2, type=int, help="Number of on/off cycles")
    parser.add_argument('-t', '--acquisition_time', default=2.0, type=float, 
            help="Time (in seconds) between beckend start and stop commands.")
    parser.add_argument('-r', '--reference', default=0.5, type=float, choices=(0, 0.5, 1),
            help="Position time and LO time -> starting_acquisition_time + acquisition_time*reference")
    parser.add_argument('--time_to_track', default=0, type=float, 
            help="Time (in seconds) we suppose the antenna needs to reach the target (on or off) position.")
    parser.add_argument('-d', '--daemon_cycle_time', default=0, type=float, 
            help="If 0 no LO daemon is used, elsewhere it is the daemon cycle time.")
    parser.add_argument('-o', '--observer', default='SRT', type=str, help="The observer.")
    parser.add_argument('-s', '--source', 
            default="C/2012 S1 (ISON),h,11/28.7757/2013,62.3948,295.6536,345.5636,\
                    1.000002,0.012446,2000,10.0,3.2",
            type=str, 
            help="Source orbital parameters.")
    args = parser.parse_args()

    obs = Observer(**observer_info[args.observer])
    target = Target(op=args.source, observer=obs)

    # Logging stufs
    logdir = 'logs'
    if not exists(logdir):
        mkdir(logdir)
    file_name = '%s--%s.log' %(target.name.lower(), datetime.datetime.utcnow().strftime("%Y_%m_%d-%H_%M_%S"))
    logfile = join(logdir, file_name)
    logging.basicConfig(filename=logfile, level=logging.DEBUG)

    handler = Handler(target, args.acquisition_time, args.time_to_track, args.reference)

    title = '\n' + handler.getObservationTitle()
    logging.info(title)
    print title
    handler.run(args.cycles, args.daemon_cycle_time)
    
