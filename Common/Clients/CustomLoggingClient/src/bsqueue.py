"""
This module implements priority queues functionalities. 
Item stored within the same queue must be comparable via __le__ __lt__ __ge__ __gt__ methods, i.e. 
bisect module is used to find the proper inertion point as o(log(n)).

classes:
    * SortedQueue: unbounded priority queue
    * BoundedSortedQueue(SortedQueue): bounded priority queue

"""
import bisect

class SortedQueue(object):
    """
    Priority queue implemented on top of a standard python list.    
    The queue mantains informations about its size and can be checked for emptiness.
    Items can be pushed int the queue, and pop method always returns the less prioritized element.

    """
    def __init__(self, data=[]):
        """Constructor
        @param data: initialization data, should be an iterable
     
        """
        self._data = sorted(data)
        self.size = len(self._data)
    
    def empty(self):
        """
        @return: True if the queue is empty, False otherwise
        """
        return self.size == 0

    def clear(self):
        """Empty the queue"""
        self._data = []
        self.size = 0

    def pop(self):
        """
        Extracts the lesser element from the queue and decrements its size
        @return: the extracted item
        """
        if not self.empty():
            self.size -= 1
            return self._data.pop(0)
        else:
            return None

    def push(self, item):
        """
        Insert a new item in the queue in the correct position. Insertion index is searched
        using the bisect module in o(log(size)) 
        @return: the insertion index
        """
        index = bisect.bisect(self._data, item)
        self._data.insert(index, item)
        self.size += 1
        return index

    def __repr__(self):
        res = ""
        for d in self._data:
            res += str(d) + '\n'
        return res

class BoundedSortedQueue(SortedQueue):
    """
    Bounded version of the SortedQueue. 
    This Queue has a max_size attrtibute so that it cannot grow indefinitely.
    When the queue is full a call to push means pop + push.

    """
    def __init__(self, max_size=10, data=[]):
        SortedQueue.__init__(self, data)
        self._max_size = max_size
        if self._max_size > len(self._data):
            self._data = self._data[:self._max_size]

    def full(self):
        """@return: True if the queue is full"""
        return self.size == self._max_size

    def push(self, item):
        """
        @return: (indexm, popped) tuple. Index is the insertion index and popped is True if an element has been popped from the queue (if it was full)

        """
        if self.full():
           self.pop()
           popped = True
        else:
           popped = False
        index = SortedQueue.push(self, item)
        return (index, popped)
