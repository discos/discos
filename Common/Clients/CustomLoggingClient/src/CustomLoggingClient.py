#!/usr/bin/env python
"""
CustomLoggingClient implements a Tkinter interface that monitors in realtime the ACS custom
logging events generated runtime.
@TODO: implement window scrolling and window resize

"""
import acscommon
import Management
from Acspy.Nc.Consumer import Consumer
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
The number of logging events visualized in the window and kept in memory
"""
MAX_SIZE = 30

"""
Handle a log_event adding it to a BoundedSortedQueue in memory and to the 
corresponding GUI representation (in Tkinter this sould be a Listbox).
@param log_event: the logging event.
@param memory_queue: a bounded sorted queue
@type memory_queue: IRAPy.bsqueue.BoundedSortedQueue
@param gui_queue: a Tkinter widget.
@type gui_queue: Tkinter.Listbox
"""
def handle_log_event(log_event, memory_queue, gui_queue):
    index, popped = memory_queue.push(log_event)
    if popped:
        gui_queue.delete(0)
    gui_queue.insert(index, log_event.msg)

class App:
    """Tkinter GUI application
    """
    def __init__(self, master):
        """Constructor."""
        self.record_list = BoundedSortedQueue(max_size=MAX_SIZE)
        frame = tk.Frame(master) #frame will be accessible via master
        frame.pack()
        self.list_box = tk.Listbox(frame, width=80, height=MAX_SIZE)
        self.list_box.pack(expand=True, fill=tk.BOTH)
        self.clear_button = tk.Button(frame, text="clear", command=self.clear)
        self.clear_button.pack(side=tk.LEFT)
        #define the log event handler
        self.handler = functools.partial(handle_log_event, memory_queue=self.record_list, gui_queue=self.list_box)
   
    def clear(self):
        """Empty the event list."""
        self.record_list.clear()
        self.list_box.delete(0, self.list_box.size())

if __name__ == '__main__':
    from IRAPy import logger
    try:
        root = tk.Tk()
        root.wm_title("ACS custom logging client")
        app = App(root)
        consumer = Consumer(Management.CUSTOM_LOGGING_CHANNEL_NAME)
        consumer.addSubscription(Management.CustomLoggingData, app.handler)
        consumer.consumerReady()
    except Exception, ex:
        print "exception caught: ", ex #TODO: throw excep
        logger.logError(ex.message)
    try:
        root.mainloop()    
    except KeyboardInterrupt, ki:
        pass
    finally:
        consumer.disconnect()
        print "Exiting"
