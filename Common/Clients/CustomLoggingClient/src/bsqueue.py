import bisect

#Priority queue to mantain log events ordering
class SortedQueue(object):
    def __init__(self, data=[]):
        self._data = sorted(data)
        self.size = len(self._data)
    
    def empty(self):
        return self.size == 0

    def clear(self):
        self._data = []
        self.size = 0

    def pop(self):
        if not self.empty():
            self.size -= 1
            return self._data.pop(0)
        else:
            return None

    def push(self, item):
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
    def __init__(self, max_size=10, data=[]):
        SortedQueue.__init__(self, data)
        self._max_size = max_size
        if self._max_size > len(self._data):
            self._data = self._data[:self._max_size]

    def full(self):
        return self.size == self._max_size

    def push(self, item):
        if self.full():
           self.pop()
           popped = True
        else:
           popped = False
        index = SortedQueue.push(self, item)
        return (index, popped)
