#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
from __future__ import with_statement

import datetime
import argparse
import time
import sys
import os
from Acspy.Clients.SimpleClient import PySimpleClient
from argparse import ArgumentParser


def wait_until_reached(property, target_pos, increment):
    """Wait in order for the PFP to reach the position."""
    t0 = datetime.datetime.now()
    TIMEOUT = increment * 30  # Seconds
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
    radius = float(value)
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
axis_description = """
x (%d, %d);
y (%d, %d);
z (%d, %d);
""" % (axis['x']['min_value'], axis['x']['max_value'],
       axis['y']['min_value'], axis['y']['max_value'],
       axis['z']['min_value'], axis['z']['max_value'])

parser = argparse.ArgumentParser()
parser.add_argument(
    '-a', '--axis',
    type=str,
    choices=axis,
    required=True,
    help=axis_description)
parser.add_argument(
    '-r', '--radius',
    type=check_radius,
    required=True)
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
increment = 1  # mm
while point <= rvalue:
    points.append(point)
    point += increment

abs_file_name = os.path.abspath(__file__)
dirname = os.path.dirname(abs_file_name)
current_time = datetime.datetime.now()
out_file_name = current_time.strftime('pfpfocus_%Y_%m_%d_h%H_%M.txt')
abs_outfile_name = os.path.join(dirname, out_file_name)
with open(abs_outfile_name, 'w') as outfile:
    outfile.write('# %s POS TPIA_CH0 TPIA_CH1 TPIB_CH0 TPIB_CH1 TPIC_CH0 TPIC_CH1'
                   % args.axis)
    position = current_pos[:]
    for point in points:
        position[address] = point
        print("Going to position (%s)" % fmt_position(position))
        pfp.setPosition(position, 0)
        wait_until_reached(property, position, increment)
        tpi_a = total_power.getTpi()
        time.sleep(0.5)
        tpi_b = total_power.getTpi()
        time.sleep(0.5)
        tpi_c = total_power.getTpi()
        outfile.write('\n%.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f' %
                     (point, tpi_a[0], tpi_a[1], tpi_b[0], tpi_b[1],
                      tpi_c[0], tpi_c[1]))

pfp.setPosition(current_pos, 0)  # Go to the original position
