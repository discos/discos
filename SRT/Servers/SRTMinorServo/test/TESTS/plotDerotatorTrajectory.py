#!/usr/bin/env python
import os
import numpy as np
import matplotlib.pyplot as plt
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument(
    'directory',
    help="Directory containing the 'status.txt' and 'trajectory.txt' files"
)

arguments = parser.parse_args()

status_time = []
status_rotation = []
starttime = None

with open(os.path.join(arguments.directory, 'status.txt'), 'r') as statusfile:
    for line in statusfile:
        args = line.strip().split()
        if not starttime:
            starttime = float(args[0])
        status_time.append(float(args[0]) - starttime)
        status_rotation.append(float(args[1]))

trajectory_time = []
trajectory_rotation = []

with open(os.path.join(arguments.directory, 'trajectory.txt'), 'r') as trajectoryfile:
    for line in trajectoryfile:
        args = line.strip().split()
        trajectory_time.append(float(args[0]) - starttime)
        trajectory_rotation.append(float(args[1]))

#plt.canvas.manager.set_window_title('Derotator positions')
plt.suptitle('Derotator positions')
plt.plot(status_time, status_rotation, 'r')
plt.plot(trajectory_time, trajectory_rotation, 'b')
plt.ylabel('rot (deg)')
plt.xlabel('time (sec)')

plt.get_current_fig_manager().window.attributes('-zoomed', True)
try:
    plt.show()
except KeyboardInterrupt:
    pass
