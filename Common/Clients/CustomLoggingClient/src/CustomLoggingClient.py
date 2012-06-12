#!/usr/bin/env python
import acscommon
import Management
from Acspy.Nc.Consumer import Consumer
import logging
import bisect
import Tkinter as tk

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

#Priority queue to mantain log events ordering
class SortedQueue(object):
    def __init__(self, max_size=MAX_SIZE, data=[]):
        self._data = sorted(data)
        self._max_size = max_size
        if self._max_size > len(self._data):
            self._data = self._data[:self._max_size]
        self.size = len(self._data)
    
    def full(self):
        return self.size == self._max_size

    def empty(self):
        return self.size == 0

    def clear(self):
        self._data = []
        self.size = 0

    def push(self, item):
        if self.full():
            self._data.pop(0)
            popped = True
        else:
            popped = False
            self.size += 1
        index = bisect.bisect(self._data, item)
        self._data.insert(index, item)
        return (index, popped)

    def __repr__(self):
        res = ""
        for d in self._data:
            res += str(d) + '\n'
        return res
    
record_list = SortedQueue()

def handler(event):
    record_list.push(event)
    print record_list

class App:
    def __init__(self, master):
        self.record_list = SortedQueue()
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

try:
    root = tk.Tk()
    root.wm_title("ACS custom logging client")
    app = App(root)
    consumer = Consumer(Management.CUSTOM_LOGGING_CHANNEL_NAME)
    consumer.addSubscription(Management.CustomLoggingData, app.handler)
    consumer.consumerReady()
    root.mainloop()    
except Exception, ex:
    print "exception caught: ", ex 
finally:
    consumer.disconnect()
    print "Exiting"
