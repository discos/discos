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
status_tx = []
status_tz = []
status_rt = []
starttime = None

with open(os.path.join(arguments.directory, 'status.txt'), 'r') as statusfile:
    for line in statusfile:
        args = line.strip().split()
        if not starttime:
            starttime = float(args[0])
        status_time.append(float(args[0]) - starttime)
        status_tx.append(float(args[1]))
        status_tz.append(float(args[2]))
        status_rt.append(float(args[3]))

trajectory_time = []
trajectory_tx = []
trajectory_tz = []
trajectory_rt = []

with open(os.path.join(arguments.directory, 'trajectory.txt'), 'r') as trajectoryfile:
    for line in trajectoryfile:
        args = line.strip().split()
        trajectory_time.append(float(args[0]) - starttime)
        trajectory_tx.append(float(args[1]))
        trajectory_tz.append(float(args[2]))
        trajectory_rt.append(float(args[3]))

fig, axs = plt.subplots(3, sharex=True)
fig.canvas.manager.set_window_title('PFP Positions')
fig.suptitle('PFP Positions')

axs[0].plot(status_time, status_tx, 'r')
axs[0].plot(trajectory_time, trajectory_tx, 'b')
axs[0].set_ylabel('tx (mm)')

axs[1].plot(status_time, status_tz, 'r')
axs[1].plot(trajectory_time, trajectory_tz, 'b')
axs[1].set_ylabel('tz (mm)')

axs[2].plot(status_time, status_rt, 'r')
axs[2].plot(trajectory_time, trajectory_rt, 'b')
axs[2].set_ylabel('rt (deg)')
axs[2].set_xlabel('time (s)')

plt.get_current_fig_manager().window.attributes('-zoomed', True)
try:
    plt.show()
except KeyboardInterrupt:
    pass
