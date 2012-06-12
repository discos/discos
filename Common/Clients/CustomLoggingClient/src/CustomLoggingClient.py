#!/usr/bin/env python
import acscommon
import Management
from Acspy.Nc.Consumer import Consumer
import logging
import bisect
import Tkinter as tk
from bsqueue import BoundedSortedQueue

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
MAX_SIZE = 30

class App:
    def __init__(self, master):
        self.record_list = BoundedSortedQueue(max_size=MAX_SIZE)
        frame = tk.Frame(master) #frame will be accessible via master
        frame.pack()
        self.list_box = tk.Listbox(frame, width=80, height=MAX_SIZE)
        self.list_box.pack(expand=True, fill=tk.BOTH)
        self.clear_button = tk.Button(frame, text="clear", command=self.clear)
        self.clear_button.pack(side=tk.LEFT)
   
    def handler(self, event):
        index, popped = self.record_list.push(event)
        if popped:
            self.list_box.delete(0)
        self.list_box.insert(index, event.msg)
        logging.info(str(event))
   
    def clear(self):
        self.record_list.clear()
        self.list_box.delete(0, self.list_box.size())

if __name__ == '__main__':
    try:
        root = tk.Tk()
        root.wm_title("ACS custom logging client")
        app = App(root)
        consumer = Consumer(Management.CUSTOM_LOGGING_CHANNEL_NAME)
        consumer.addSubscription(Management.CustomLoggingData, app.handler)
        consumer.consumerReady()
    except Exception, ex:
        print "exception caught: ", ex #TODO: throw excep
    try:
        root.mainloop()    
    except KeyboardInterrupt, ki:
        pass
    finally:
        consumer.disconnect()
        print "Exiting"
