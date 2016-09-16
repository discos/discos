"""Docstring
* Write the limits and the documentation.
* ...
"""
#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>

import datetime
import argparse
import time
import sys
from Acspy.Clients.SimpleClient import PySimpleClient
from argparse import ArgumentParser


def wait_until_reached(property, target_pos, increment):
    """Wait in order for the PFP to reach the position."""
    t0 = datetime.datetime.now()
    TIMEOUT = increment * 5  # Seconds
    while (datetime.datetime.now() - t0).seconds < TIMEOUT:
        current_pos, comp = property.get_sync()
        reached = True
        for (cvalue, tvalue) in zip(current_pos, target_pos):
            if abs(cvalue - tvalue) > 0.05:
                reached = False
                break
        if reached:
            break
        else:
            time.sleep(0.1)
    else:
        print('ERROR: cannot reach the position: timeout expired.')
        sys.exit(1)


def fmt_position(position):
    """Return a formatted position.

    For instance:

        >>> position = [1.2333, 0.11111, 2.3313]
        >>> fmt_position(position)
        '1.22, 0.11, 2.33'
    """
    return ', '.join(['%.2f' % p for p in position])


def check_radius(value):
    radius = int(value)
    if radius <= 0:
         raise argparse.ArgumentTypeError(
             "%s not allowed: the radius must be a positive integer" % value)
    return radius

axis = {
    'x': {
        'min_value': -1480,
        'max_value': 1480,
        'address': 1,
    },
    'y': {
        'min_value': -26,
        'max_value': 2740,
        'address': 0,
    },
    'z': {
        'min_value': -199,
        'max_value': 40,
        'address': 2,
    }
}

parser = argparse.ArgumentParser()
parser.add_argument(
    '-a', '--axis',
    type=str,
    choices=axis,
    required=True,
    help='The axis')
parser.add_argument(
    '-r', '--radius',
    type=check_radius,
    required=True,
    help='The radius...')
args = parser.parse_args()

min_value = axis[args.axis]['min_value']
max_value = axis[args.axis]['max_value']
address = axis[args.axis]['address']

client = PySimpleClient()
pfp = client.getComponent("MINORSERVO/PFP")
total_power = client.getComponent('BACKENDS/TotalPower')
property = pfp._get_actPos()
current_pos, comp = property.get_sync()
current_value = current_pos[address]

lvalue = current_value - args.radius  # Left value
rvalue = current_value + args.radius  # Right value
print("Current %s value: %.2f" % (args.axis, current_value))
print("Target position: from %.2f to %.2f\n" % (lvalue, rvalue))

if lvalue < min_value or rvalue > max_value:
    print('ERROR: the %s movement from %.2f to %.2f is not allowed.\n'
          'The %s position must be inside the range (%.2f, %.2f).'
          % (args.axis, lvalue, rvalue, args.axis, min_value, max_value))
    sys.exit(1)

points = []
point = lvalue
increment = 1  # 1mm
while point <= rvalue:
    points.append(point)
    point += increment

position = current_pos
for point in points:
    position[address] = point
    print("Going to position (%s)" % fmt_position(position))
    pfp.set_sync(position)
    wait_until_reached(property, position, increment)
    tpi = tp.getTpi()
    # TODO: write to file: position, TPI

# TODO: creare directory con data come nome, dove salvare file
# TODO: salvare posizione e TPI su file
# TODO: creare un plot positione/TPI e salvarlo
# TODO: mostare a video il plot


# pfp.set_sync(current_pos)  # Go to the original position
# wait_until_reached(property, current_pos, increment)
