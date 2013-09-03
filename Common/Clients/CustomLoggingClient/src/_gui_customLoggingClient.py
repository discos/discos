#!/usr/bin/env python
"""
CustomLoggingClient implements a Tkinter interface that monitors in realtime the ACS custom
logging events generated runtime.
@TODO: implement window scrolling and window resize

"""
import acscommon
import Management
from Acspy.Nc.Consumer import Consumer
import signal
import logging
import bisect
import functools
import Tkinter as tk
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
    try:
        root = tk.Tk()
        root.wm_title("ACS custom logging client")
        root.resizable(tk.YES, tk.YES)
        app = App(root)
        consumer = Consumer(Management.CUSTOM_LOGGING_CHANNEL_NAME)
        consumer.addSubscription(Management.CustomLoggingData, app.handler)
        consumer.consumerReady()
    except Exception, ex:
        print "exception caught: ", ex #TODO: throw excep
        logger.logError(ex.message)
    #try:

    def handle_signal(num, trace):
        trace.f_locals['app'].clear()
        trace.f_locals['consumer'].disconnect()
        print "Exiting"
        sys.exit(0)

    signal.signal(signal.SIGINT, handle_signal)     
    signal.signal(signal.SIGUSR1, handle_signal)     
    root.mainloop()    
    #except KeyboardInterrupt, ki:
    #    pass
    #finally:
    #    consumer.disconnect()
    #    print "Exiting"
