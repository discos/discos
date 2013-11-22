from __future__ import division

__credits__ = """Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""

import datetime
import ephem
import logging

__credits__ = """Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""

class Observer(ephem.Observer):
    def __init__(self, name, latitude, longitude, elevation):
        super(Observer, self).__init__()
        self.name = name
        self.lat = latitude
        self.lon = longitude
        self.elev = elevation

class Target(object):
    def __init__(self, op, observer):
        """Target information: orbital parameter and observer."""
        try:
            self.source = ephem.readdb(op)
        except Exception, e:
            print "Unexpected exception: %s" %e
            logging.exception('Got exception on pyephem')
            sys.exit()
        self.observer = observer
        self.name = self.source.name
        self.nickname = Target.getNickName(op)
        self.from_ = observer.name

    def getRaDec(self, at_time=None):
        """Get the (ra, dec) position of the source from the observer, at `at_time` UTC time."""
        self._compute(at_time)
        return self.source.ra, self.source.dec

    def getRaDecDiff(self, time1, time2):
        return map(lambda x, y: x - y, self.getRaDec(time2), self.getRaDec(time1))

    def nextRising(self, at_time=None):
        self._compute(at_time)
        return self.observer.next_rising(self.source).datetime()

    def nextSetting(self, at_time=None):
        self._compute(at_time)
        return self.observer.next_setting(self.source).datetime()

    def isVisible(self, at_time=None):
        at_time = at_time if at_time else datetime.datetime.utcnow()
        return self.nextSetting(at_time) < self.nextRising(at_time)

    def _compute(self, at_time):
        self.observer.date = at_time if at_time else datetime.datetime.utcnow()
        self.source.compute(self.observer)

    @staticmethod
    def getNickName(op):
        return op[op.find('(')+1:op.find(')')].lower()


if __name__ == '__main__':
    import doctest
    doctest.testmod()

