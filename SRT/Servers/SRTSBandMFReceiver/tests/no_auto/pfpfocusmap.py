#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
"""Example:

    $ ./focusmap.py -xrange=-20:10 --yrange=-25:5 --zrange=-50:10

You can also use -x, -y and -z instead of --xrange, --yrange and --zrange:

    $ ./focusmap.py -x=-20:10 -y=-25:5 -z=-50:10
"""
from __future__ import with_statement

import datetime
import argparse
import time
import sys
import os
from Acspy.Clients.SimpleClient import PySimpleClient
from argparse import ArgumentParser


axis = {
    'x': {
        'min_value': -1480,
        'max_value': 1480,
    },
    'y': {
        'min_value': -26,
        'max_value': 2740,
    },
    'z': {
        'min_value': -199,
        'max_value': 40,
    }
}


def wait_until_reached(property, target_pos):
    """Wait in order for the PFP to reach the position."""
    t0 = datetime.datetime.now()
    TIMEOUT = 90
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


def check_axis_range(axis_name, value):
    """Take the axis name and a range, and return a tuple of floats.

    @param axis_name: it must be x, y or z
    @param value: a string in the form min_value:max_value
    
    For instance, if the value is '-15:15', it returns the tuple (-15.0, 15.0).
    """
    try:
        left, right = [float(item) for item in value.split(':')]
    except ValueError:
        raise argparse.ArgumentTypeError(
            "not allowed: it must be in the form min:max (i.e. -15:15).")

    try:
        min_value = axis[axis_name]['min_value']
        max_value = axis[axis_name]['max_value']
    except IndexError:
        raise argparse.ArgumentTypeError(
            "value not allowed: the axis must be x, y or z.")

    if left >= right:
        raise argparse.ArgumentTypeError(
            "the left value must be lower than the right one.")

    if left <= min_value or right >= max_value:
        raise argparse.ArgumentTypeError(
            "range not allowed: it must be between %.2f and %.2f." %
            (min_value, max_value))

    return (left, right)


def check_x_axis_range(value):
    return check_axis_range(axis_name='x', value=value)


def check_y_axis_range(value):
    return check_axis_range(axis_name='y', value=value)


def check_z_axis_range(value):
    return check_axis_range(axis_name='z', value=value)


parser = argparse.ArgumentParser(__doc__)
parser.add_argument(
    '-x', '--xrange',
    type=check_x_axis_range,
    required=True,
    help=("The x axis range, in the form min_value:max_value. "
          "The min_value must be higher than %.2f, and the max_value "
          "lower than %.2f. For instance: --xrange -15:15 means the "
          "movement in x will be between -15 and 15 units." %
          (axis['x']['min_value'], axis['x']['max_value'])))
parser.add_argument(
    '--xstep',
    type=float,
    required=False,
    default=5.0,
    help=("The step in x, between two consecutive x positions."))
parser.add_argument(
    '--ystep',
    type=float,
    required=False,
    default=1.0,
    help=("The step in y, between two consecutive y positions."))
parser.add_argument(
    '--zstep',
    type=float,
    required=False,
    default=5.0,
    help=("The step in z, between two consecutive z positions."))
parser.add_argument(
    '-y', '--yrange',
    type=check_y_axis_range,
    required=True,
    help=("The y axis range, in the form min_value:max_value. "
          "The min_value must be higher than %.2f, and the max_value "
          "lower than %.2f. For instance: --xrange -15:15 means the "
          "movement in x will be between -15 and 15 units." %
          (axis['y']['min_value'], axis['y']['max_value'])))
parser.add_argument(
    '-z', '--zrange',
    type=check_z_axis_range,
    required=True,
    help=("The z axis range, in the form min_value:max_value. "
          "The min_value must be higher than %.2f, and the max_value "
          "lower than %.2f. For instance: --xrange -15:15 means the "
          "movement in x will be between -15 and 15 units." %
          (axis['z']['min_value'], axis['z']['max_value'])))
args = parser.parse_args()

client = PySimpleClient()
pfp = client.getComponent("MINORSERVO/PFP")
total_power = client.getComponent('BACKENDS/TotalPower')
property = pfp._get_actPos()
current_pos, comp = property.get_sync()

xpoints = []
lpoint, rpoint = args.xrange
while lpoint <= rpoint:
    xpoints.append(lpoint)
    lpoint += args.xstep

ypoints = []
lpoint, rpoint = args.yrange
while lpoint <= rpoint:
    ypoints.append(lpoint)
    lpoint += args.ystep

zpoints = []
lpoint, rpoint = args.zrange
while lpoint <= rpoint:
    zpoints.append(lpoint)
    lpoint += args.zstep


positions = []
for xpoint in xpoints:
    for ypoint in ypoints:
        for zpoint in zpoints:
            positions.append((xpoint, ypoint, zpoint))

for pos in positions:
    print(pos)


abs_file_name = os.path.abspath(__file__)
dirname = os.path.dirname(abs_file_name)
current_time = datetime.datetime.now()
out_file_name = current_time.strftime('focusmap_%Y_%m_%d_h%H_%M.txt')
abs_outfile_name = os.path.join(dirname, out_file_name)
with open(abs_outfile_name, 'w') as outfile:
    outfile.write('# Columns: POSITION  TPI_CH0  TPI_CH1')
    for index, position in enumerate(positions):
        pfp.setPosition(position, 0)
        wait_until_reached(property, position)
        tpi = total_power.getTpi()
        fmt_pos = fmt_position(position)  # Formatted position (a string)
        message = '\n%s  %.2f  %.2f' % (fmt_pos, tpi[0], tpi[1]))
        outfile.write(message)
        print("%04d) %s" %(index, message))

pfp.setPosition(current_pos, 0)  # Go to the original position
pfp._release()
