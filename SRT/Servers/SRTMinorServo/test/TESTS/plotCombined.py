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

srp_status_time = []
derotator_status_time = []
status_tx = []
status_ty = []
status_tz = []
status_rx = []
status_ry = []
status_rz = []
status_rotation = []
starttime = None

with open(os.path.join(arguments.directory, 'SRP', 'status.txt'), 'r') as statusfile:
    for line in statusfile:
        args = line.strip().split()
        if not starttime:
            starttime = float(args[0])
        srp_status_time.append(float(args[0]) - starttime)
        status_tx.append(float(args[1]))
        status_ty.append(float(args[2]))
        status_tz.append(float(args[3]))
        status_rx.append(float(args[4]))
        status_ry.append(float(args[5]))
        status_rz.append(float(args[6]))

with open(os.path.join(arguments.directory, 'DEROTATOR', 'status.txt'), 'r') as statusfile:
    for line in statusfile:
        args = line.strip().split()
        derotator_status_time.append(float(args[0]) - starttime)
        status_rotation.append(float(args[1]))

trajectory_time = []
trajectory_tx = []
trajectory_ty = []
trajectory_tz = []
trajectory_rx = []
trajectory_ry = []
trajectory_rz = []
trajectory_rotation = []

with open(os.path.join(arguments.directory, 'SRP', 'trajectory.txt'), 'r') as trajectoryfile:
    for line in trajectoryfile:
        args = line.strip().split()
        trajectory_time.append(float(args[0]) - starttime)
        trajectory_tx.append(float(args[1]))
        trajectory_ty.append(float(args[2]))
        trajectory_tz.append(float(args[3]))
        trajectory_rx.append(float(args[4]))
        trajectory_ry.append(float(args[5]))
        trajectory_rz.append(float(args[6]))

with open(os.path.join(arguments.directory, 'DEROTATOR', 'trajectory.txt'), 'r') as trajectoryfile:
    for line in trajectoryfile:
        args = line.strip().split()
        trajectory_rotation.append(float(args[1]))

fig, axs = plt.subplots(7, sharex=True)
fig.canvas.manager.set_window_title('Program Track Positions')
fig.suptitle('Program Track Positions')

axs[0].plot(srp_status_time, status_tx, 'r')
axs[0].plot(trajectory_time, trajectory_tx, 'b')
axs[0].set_ylabel('tx (mm)')
axs[0].sharey(axs[1])

axs[1].plot(srp_status_time, status_ty, 'r')
axs[1].plot(trajectory_time, trajectory_ty, 'b')
axs[1].set_ylabel('ty (mm)')
axs[1].sharey(axs[2])

axs[2].plot(srp_status_time, status_tz, 'r')
axs[2].plot(trajectory_time, trajectory_tz, 'b')
axs[2].set_ylabel('tz (mm)')

axs[3].plot(srp_status_time, status_rx, 'r')
axs[3].plot(trajectory_time, trajectory_rx, 'b')
axs[3].set_ylabel('rx (deg)')
axs[3].sharey(axs[4])

axs[4].plot(srp_status_time, status_ry, 'r')
axs[4].plot(trajectory_time, trajectory_ry, 'b')
axs[4].set_ylabel('ry (deg)')
axs[4].sharey(axs[5])

axs[5].plot(srp_status_time, status_rz, 'r')
axs[5].plot(trajectory_time, trajectory_rz, 'b')
axs[5].set_ylabel('rz (deg)')

axs[6].plot(derotator_status_time, status_rotation, 'r')
axs[6].plot(trajectory_time, trajectory_rotation, 'b')
axs[6].set_ylabel('derot (deg)')
axs[6].set_xlabel('time (s)')


plt.get_current_fig_manager().window.attributes('-zoomed', True)
try:
    plt.show()
except KeyboardInterrupt:
    pass
