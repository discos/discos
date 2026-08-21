#!/usr/bin/env python
from __future__ import print_function
"""
CustomLoggingClient implements a Tkinter interface that monitors in realtime the ACS custom
logging events generated runtime.

"""
import acscommon
import Management
from Acspy.Nc.Consumer import Consumer
import signal
import sys
import logging
import bisect
import functools
import argparse
try:
    import Tkinter as tk
except:
    import tkinter as tk
from IRAPy.bsqueue import BoundedSortedQueue

#DEFINE LOGGING RECORDS TOTAL ORDERING RULES
Management.CustomLoggingData.__eq__ = lambda self, other: self.timeStamp == other.timeStamp
Management.CustomLoggingData.__ne__ = lambda self, other: self.timeStamp != other.timeStamp
Management.CustomLoggingData.__lt__ = lambda self, other: self.timeStamp <  other.timeStamp
Management.CustomLoggingData.__le__ = lambda self, other: self.timeStamp <= other.timeStamp
Management.CustomLoggingData.__gt__ = lambda self, other: self.timeStamp >  other.timeStamp
Management.CustomLoggingData.__ge__ = lambda self, other: self.timeStamp >= other.timeStamp
#DEFINE LOGGING RECORDS STRING REPRESENTATION
Management.CustomLoggingData.__repr__ = lambda self: self.msg

# Maximum size for log messages display
"""
The number of logging events kept in memory
"""
MAX_QUEUE_SIZE = 200

"""
The number of logging events displayed in the window
"""
MAX_WINDOW_SIZE = 30


class App:
    """Tkinter GUI application
    """
    def __init__(self, master):
        """Constructor."""
        self.record_list = BoundedSortedQueue(max_size=MAX_QUEUE_SIZE)
        frame = master
        self.yscrollbar = tk.Scrollbar(frame, orient=tk.VERTICAL)
        self.yscrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.list_box = tk.Listbox(frame, 
                                   width=160, 
                                   height=MAX_WINDOW_SIZE, 
                                   yscrollcommand=self.yscrollbar.set)
        self.clear_button = tk.Button(frame, text="clear", command=self.clear)
        self.clear_button.pack(side=tk.BOTTOM, anchor=tk.SW)
        self.list_box.pack(expand=True, side=tk.LEFT, fill=tk.BOTH)
        self.yscrollbar.config(command=self.list_box.yview)
  
    def handler(self, event):
        """
        Handle a log_event adding it to a BoundedSortedQueue in memory and to the 
        corresponding GUI representation (in Tkinter this sould be a Listbox).
        @param event: the logging event.
        """
        index, popped = self.record_list.push(event)
        if popped:
            self.list_box.delete(0)
        self.list_box.insert(index, event.msg)
        self.list_box.yview(tk.END) # Listbox autoscroll

    def clear(self):
        """Empty the event list."""
        self.record_list.clear()
        self.list_box.delete(0, self.list_box.size())

if __name__ == '__main__':
    from IRAPy import logger

    # Parse command line options. Add --geometry to allow positioning the window
    parser = argparse.ArgumentParser(description='Logging window GUI')
    parser.add_argument('--geometry', help="Window position as 'X,Y' (pixels from top-left)")
    args = parser.parse_args()

    # Default width x height used previously in the code
    DEFAULT_GEOMETRY = '934x256'

    # Build the geometry string for root.geometry(). If --geometry is provided it is
    # expected as 'X,Y' (e.g. 200,100). This will produce a geometry string like
    # '934x256+200+100' which Tkinter accepts.
    geom = DEFAULT_GEOMETRY
    if args.geometry:
        try:
            parts = args.geometry.replace(',', ' ').split()
            if len(parts) == 2:
                x = int(parts[0])
                y = int(parts[1])
                geom = '{}+{}+{}'.format(DEFAULT_GEOMETRY, x, y)
            else:
                # invalid format; fall back to default but print a warning
                print("Warning: --geometry should be in the form 'X,Y' (e.g. --geometry 200,100). Using default geometry.")
        except Exception:
            print("Warning: invalid --geometry value '{}'. Using default geometry.".format(args.geometry))

    try:
        root = tk.Tk()
        root.wm_title("Logging window")
        root.resizable(tk.YES, tk.YES)
        root.geometry(geom)
        app = App(root)
        consumer = Consumer(Management.CUSTOM_LOGGING_CHANNEL_NAME)
        consumer.addSubscription(Management.CustomLoggingData, app.handler)
        consumer.consumerReady()
    except Exception as ex:
        print("exception caught: ", ex) #TODO: throw excep
        try:
            logger.logError(ex.message)
        except Exception:
            # logger may not be available or have .logError/message
            pass
    #try:

    def handle_signal(num, trace):
        #trace.f_locals['app'].clear()
        #trace.f_locals['consumer'].disconnect()
        print("Exiting")
        sys.exit(0)

    signal.signal(signal.SIGINT, handle_signal)     
    signal.signal(signal.SIGUSR1, handle_signal)     
    try:
        root.mainloop()
    except NameError:
        # root was not created due to earlier exception
        pass
    #except KeyboardInterrupt as ki:
    #    pass
    #finally:
    #    consumer.disconnect()
    #    print("Exiting")