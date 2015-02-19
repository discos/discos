import time
import shelve
from parameters import closers, app_nr, axes

# REMOVE -------------------
# Drive cabinet states
dc_startup_state = 1
dc_park_state = 3
dc_ok = 0

# Application states
app_remote_auto = 4
# END REMOVE ---------------


class Servo(object):

    def __init__(self, address):
        self.id = address
        self.name = app_nr[self.id] # GRF, PFP, SRP, M3R
        self.axes = axes[self.name] # Number of axes
        self.history = History(self.name, self.axes)

    def set_app_state(self, value):
        self.app_state = int(value)

    def set_cab_state(self, value):
        self.cab_state = int(value)

    def set_app_status(self, value):
        self.app_status = int(value)

    def getpos(self, cmd_num):
        data = self.history.get();
        answer = '?getpos' + ':%d=%d> %s' % (cmd_num, self.id, Servo.time())
        # Read the positions stored in a shelve db by a setpos command
        for position in data[1:]:
            answer += ',%s' %position
        else:
            answer += closers[0]

        return answer

    def getappstatus(self, cmd_num):
        value = '0000030D'
        answer = '?getappstatus' + ':%d=%d> %s' %(cmd_num, self.id, value)
        return answer + closers[0]

    def getstatus(self, cmd_num):
        # Ready
        app_state = app_remote_auto
        app_status = "FFFF"
        cab_state = dc_ok

        answer = '?getstatus' + ':%d=%d> ' % (cmd_num, self.id)
        answer += '%d,%d,%s,%d' %(Servo.time(), app_state, app_status, cab_state)

        # Read the positions stored in a shelve db by a setpos command
        data = self.history.get()
        for position in data[1:]:
            answer += ',%s' %position
        else:
            answer += closers[0]
        return answer


    def setpos(self, cmd_num, *params):
        positions = params[-self.axes:], params[0]
        self.history.insert(positions)
        answer = '@setpos' + ':%d=%d' %(cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        return answer


    def setup(self, cmd_num, *params):
        answer = '@setup' + ':%d=%d' % (cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        return answer


    def stow(self, cmd_num, *params):
        answer = '@stow' + ':%d=%d' % (cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        return answer

    def clean(self, cmd_num, *params):
        answer = '@clean' + ':%d=%d' % (cmd_num, self.id)
        for param in params:
            answer += ",%s" %param
        else:
            answer += closers[0]
        return answer

    def getspar(self, cmd_num, *params):
        answer = '?getspar' + ':%d=%d' % (cmd_num, self.id)

        for param in params[1:]:
            answer += ',%s' %param

        answer += '> %d' % (sum([ int(param) for param in params]))
        answer += closers[0]
        return answer

    @staticmethod
    def time():
        """Return the actual time in OMG format"""
        acstime_ACE_BEGIN = 122192928000000000L
        return int(acstime_ACE_BEGIN + time.time() * 10000000L)


class History(object):
    def __init__(self, servo_name, servo_axes):
        self.servo_name = servo_name
        self.servo_axes = servo_axes
        self.path = '/tmp/%s_positions.db' %servo_name
        db = shelve.open(self.path, writeback=True)
        db[servo_name] = [[Servo.time()] + [0]*servo_axes]
        db.close()

    def insert(self, positions, timestamp=None):
        target_time = timestamp if timestamp else Servo.time()
        db = shelve.open(self.path, writeback=True)
        list_of_lists = db[self.servo_name]
        index = None
        for idx, lst in enumerate(list_of_lists):
            if lst[0] <= target_time:
                if idx  + 1 == len(list_of_lists):
                    index = idx + 1
                    break
                else:
                    if list_of_lists[idx + 1][0] > target_time:
                        index = idx + 1
                        break
                    else:
                        continue
            else:
                index = 0
                break
        list_of_lists.insert(index, [target_time] + list(positions))
        db.close()


    def get(self, time_ref=Servo.time):
        """Return the actual position as [timestamp, axis A, ..., axis N]"""
        timestamp = time_ref() if callable(time_ref) else time_ref
        db = shelve.open(self.path, writeback=True)
        list_of_lists = db[self.servo_name]
        index = None
        for idx, lst in enumerate(list_of_lists):
            if lst[0] <= timestamp:
                if idx + 1 == len(list_of_lists):
                    index = idx
                    break
                else:
                    if list_of_lists[idx + 1][0] > timestamp:
                        index = idx
                        break
                    else:
                        continue
            else:
                index = 0
                break
        value = list_of_lists[index]
        db[self.servo_name] = list_of_lists
        if callable(time_ref):
            db[self.servo_name] = list_of_lists[index::]
        db.update()
        db.close()
        return value

