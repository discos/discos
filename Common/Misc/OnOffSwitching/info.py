"""This module define the Observer and Target classes, and stores the observers information. 

An example of Observer and Target instantiations::

    >>> observer = Observer(**observer_info['SRT'])
    >>> observer.name
    'Sardinia Radio Telescope'
    >>> observer.latitude, observer.longitude, observer.elevation
    ('39:29:34', '09:14:42', 700.0)
    >>> op = "C/2012 S1 (ISON),h,11/28.7757/2013,62.3948,295.6536,345.5636,\
    ... 1.000002,0.012446,2000,10.0,3.2" # Orbital parameters
    >>> target = Target(op, observer)
    >>> target.name
    'ISON'
    >>> target.from_
    'Sardinia Radio Telescope'
    >>> import datetime
    >>> target.getRaDec(datetime.datetime.utcnow()) # doctest: +SKIP
    (13:11:31.70, -9:11:18.6)

Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
The author wants to say thanks to: S.Poppi, M.Bartolini, A.Orlati, C.Migoni, and A.Melis
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""

import ephem
import logging

class Observer(ephem.Observer):
    def __init__(self, name, latitude, longitude, elevation):
        super(Observer, self).__init__()
        self.name = name
        self.latitude = latitude
        self.longitude = longitude
        self.elevation = elevation
        # TODO: horizont?

class Target:
    def __init__(self, op, observer):
        """Target information: orbital parameter and observer."""
        self.op = op
        self.observer = observer
        self.name = Target.nameFromOP(op)
        self.from_ = observer.name

    def getRaDec(self, at_time):
        """Get the (ra, dec) position of the source from the observer, at `at_time` UTC time."""
        try:
            source = ephem.readdb(self.op)
        except Exception, e:
            print "Unexpected exception: %s" %e
            logging.exception('Got exception on pyephem')
            sys.exit()
        self.observer.date = at_time
        source.compute(self.observer)
        # TODO: logging.warning if not isVisible()
        return source.ra, source.dec

    def isVisible(self):
        pass # TODO

    @staticmethod
    def nameFromOP(op):
        try:
            return op[op.find('(')+1:op.find(')')]
        except:
            return "unknown_source"

# Observer information
observer_info = {

        'SRT': {
            'name': 'Sardinia Radio Telescope', 
            'latitude': "39:29:34", 
            'longitude': "09:14:42", 
            'elevation': 700
        },

        'Medicina': {
            'name': 'Medicina Radio Telescope',
            'latitude': "44:31:14", 
            'longitude': "11:38:43", 
            'elevation': 41
        }
}


if __name__ == '__main__':
    import doctest
    doctest.testmod()


