from __future__ import division, with_statement
import time
import shelve
import operator
from multiprocessing import Process, Value, Lock

from parameters import closers, app_nr, axes, stow_position


class DriveCabinet(object):
    cab_state = {
        'ready': 0,          # The servo ready to move
        'starting': 1,       # The servo is performing a setup
        'block_removed': 2,  # A setup is required
        'stow': 3,           # Parked
        'power_failure': 4,  # Power supply failure
        'disable': 5,       # Disabled from other drive cabinet
        'reset_required': 6, # A clearemergency is required 
        'blocked': 7,        # Blocking condition
    }

    app_state = {}
    app_status = {}

    def __init__(self):
        self.cab_state = Value('i', DriveCabinet.cab_state['stow'])

    def set_state(self, state):
        self.cab_state.value = DriveCabinet.cab_state[state]
    

class Servo(object):

    def __init__(self, address):
        self.id = address
        self.name = app_nr[self.id] # GRF, PFP, SRP, M3R
        self.axes = axes[self.name] # Number of axes
        self.stow_position = stow_position[self.name] 
        self.history = History(self.axes)
        self.dc = DriveCabinet()
        self.stow(0)

    def getpos(self, cmd_num):
        data = self.history.get();
        answer = '?getpos' + ':%d=%d> %s' % (cmd_num, self.id, Servo.ctime())
        # Read the position stored in a shelve db by a setpos command
        for position in data[1:]:
            answer += ',%s' %position
        else:
            answer += closers[0]
        return [answer]

    def getappstatus(self, cmd_num):
        value = '0000030D'
        answer = '?getappstatus' + ':%d=%d> %s' %(cmd_num, self.id, value)
        return [answer + closers[0]]

    def getstatus(self, cmd_num):
        app_state = 4 # remote auto
        app_status = "FFFF" # Everything OK
        cab_state = self.dc.cab_state.value

        answer = '?getstatus' + ':%d=%d> ' % (cmd_num, self.id)
        answer += '%d,%d,%s,%d' %(Servo.ctime(), app_state, app_status, cab_state)

        # Read the position stored in a shelve db by a setpos command
        data = self.history.get()
        for position in data[1:]:
            answer += ',%s' %position
        else:
            answer += closers[0]
        return [answer]

    def setpos(self, cmd_num, *params):
        timestamp, position = params[0], list(params[-self.axes:])
        self.history.insert(position, timestamp)
        answer = '@setpos' + ':%d=%d' %(cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        return [answer.replace('@', '?'), answer]

    @staticmethod
    def _setup_process(cab_state):
        cab_state.value = DriveCabinet.cab_state['starting']
        time.sleep(3) # The setup takes 3 seconds
        cab_state.value = DriveCabinet.cab_state['ready']

    def setup(self, cmd_num, *params):
        answer = '@setup' + ':%d=%d' % (cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        p = Process(target=Servo._setup_process, args=(self.dc.cab_state,))
        p.start()
        return [answer.replace('@', '?'), answer]

    def stow(self, cmd_num, *params):
        answer = '@stow' + ':%d=%d' % (cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        self.dc.cab_state.value = DriveCabinet.cab_state['stow']
        self.history.insert(self.stow_position)
        return [answer.replace('@', '?'), answer]

    def disable(self, cmd_num, *params):
        answer = '@disable' + ':%d=%d' % (cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        self.dc.cab_state.value = DriveCabinet.cab_state['stow']
        return [answer.replace('@', '?'), answer]


    def clean(self, cmd_num, *params):
        answer = '@clean' + ':%d=%d' % (cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        self.history.clean()
        return [answer.replace('@', '?'), answer]

    def getspar(self, cmd_num, *params):
        answer = '?getspar' + ':%d=%d> ' % (cmd_num, self.id)

        index_sub = list(params[-2:])
        if [1250, 0] == index_sub: # Acceleration
            answer += '3'
        elif [1240, 0] == index_sub: # Max speed
            answer += '10'
        else:
            print 'index_sub, type: ', index_sub, type(index_sub)
            answer += '0'
        
        answer += closers[0]
        return [answer]

    def setsdatbitb16(self, cmd_num, *params):
        answer = '@setsdatbitb16' + ':%d=%d' % (cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        return [answer]

    @staticmethod
    def ctime():
        """Return the current time in OMG format"""
        acstime_ACE_BEGIN = 122192928000000000L
        return int(acstime_ACE_BEGIN + time.time() * 10000000L)


class History(object):
    lock = Lock()

    def __init__(self, axes):
        self.history = []
        self.insert(axes*[0])

    def insert(self, position, timestamp=None):
        target_time = timestamp if timestamp else Servo.ctime()
        data = [target_time] + list(position)
        with History.lock:
            self.history.append(data)
            self.history.sort(key=operator.itemgetter(0))
            self.history = self.history[-2**15:] # Last 2**15 positions

    def clean(self, since=0):
        target_time = since if since else Servo.ctime()
        with History.lock:
            idx = len(self.history)
            self.history.sort(key=operator.itemgetter(0))
            for idx, item in enumerate(self.history):
                timestamp = item[0]
                if timestamp > target_time:
                    break
            self.history = self.history[:idx] 

    def get(self, target_time=None):
        """Return the position @target_time as [timestamp, axisA, ..., axisN]"""
        if target_time is None:
            target_time = Servo.ctime()
        size = len(self.history)
        idx = -1
        with History.lock:
            while idx >= -size:
                data = self.history[idx]
                timestamp, position = data[0], data[1:]
                if timestamp <= target_time:
                    return data
                elif idx - 1 > -size:
                    prev_timestamp = self.history[idx - 1]
                    if prev_timestamp <= target_time:
                        return self.history[idx - 1] # It's better to interpolate
                    else:
                        idx -= 1
                        continue
                else:
                    return self.history[-size]
                    
