import time
import shelve
from parameters import closers, time_stamp, number_of_axis, app_nr, db_name, response_types


def now():
    """Return the actual time in OMG format"""
    acstime_ACE_BEGIN = 122192928000000000L
    return int(acstime_ACE_BEGIN + time.time() * 10000000L)

class PositionDB:
    def __init__(self):
        pass

    def initialize(self):
        db = shelve.open(db_name, writeback = True)
        for servo_type in app_nr.values():
            db[servo_type] = [[now()] + [0] * number_of_axis[servo_type]]
        db.close()

    def insert(self, servo_type, positions, timestamp=None):
        if not timestamp:
            timestamp = now()
        db = shelve.open(db_name, writeback = True)
        list_of_lists = db[servo_type]
        index = None
        for idx, lst in enumerate(list_of_lists):
            if lst[0] <= timestamp:
                if idx  + 1 == len(list_of_lists):
                    index = idx + 1
                    break
                else:
                    if list_of_lists[idx + 1][0] > timestamp:
                        index = idx + 1
                        break
                    else:
                        continue
            else:
                index = 0
                break
        list_of_lists.insert(index, [timestamp] + list(positions))
        db.close()

    def print_positions(self, servo_type=None):
        db = shelve.open(db_name, writeback = True)
        if servo_type:
            list_of_lists = db[servo_type]
            for lst in list_of_lists:
                print lst
        else:
            for servo_type in app_nr.values():
                list_of_lists = db[servo_type]
                print ("%s" %servo_type).upper()
                for lst in list_of_lists:
                    print lst
                print
        db.close()

    def get(self, servo_type, time_ref=now):
        """Return the actual position as [timestamp, axis A, ..., axis N]"""
        # timestamp = now() if not time_ref else time_ref
        timestamp = time_ref
        if hasattr(time_ref, "__call__"):
            timestamp = time_ref()
        db = shelve.open(db_name, writeback = True)
        list_of_lists = db[servo_type]
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
        db[servo_type] = list_of_lists
        if hasattr(time_ref, "__call__"):
            db[servo_type] = list_of_lists[index::]
        db.update()
        db.close()
        return value

