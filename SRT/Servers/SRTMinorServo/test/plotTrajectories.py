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
status_ty = []
status_tz = []
status_rx = []
status_ry = []
status_rz = []
starttime = None

with open(os.path.join(arguments.directory, 'status.txt'), 'r') as statusfile:
    for line in statusfile:
        args = line.strip().split()
        if not starttime:
            starttime = float(args[0])
        status_time.append(float(args[0]) - starttime)
        status_tx.append(float(args[1]))
        status_ty.append(float(args[2]))
        status_tz.append(float(args[3]))
        status_rx.append(float(args[4]))
        status_ry.append(float(args[5]))
        status_rz.append(float(args[6]))

trajectory_time = []
trajectory_tx = []
trajectory_ty = []
trajectory_tz = []
trajectory_rx = []
trajectory_ry = []
trajectory_rz = []

with open(os.path.join(arguments.directory, 'trajectory.txt'), 'r') as trajectoryfile:
    for line in trajectoryfile:
        args = line.strip().split()
        trajectory_time.append(float(args[0]) - starttime)
        trajectory_tx.append(float(args[1]))
        trajectory_ty.append(float(args[2]))
        trajectory_tz.append(float(args[3]))
        trajectory_rx.append(float(args[4]))
        trajectory_ry.append(float(args[5]))
        trajectory_rz.append(float(args[6]))

fig = plt.figure()
fig.canvas.manager.set_window_title('SRP Positions')
fig.suptitle('SRP Positions')
gs = fig.add_gridspec(3, 2, hspace=0)
((tx, rx), (ty, ry), (tz, rz)) = gs.subplots(sharex=True, sharey='col')
tx.plot(status_time, status_tx, 'r')
tx.plot(trajectory_time, trajectory_tx, 'b')
tx.set_ylabel('tx (mm)')

ty.plot(status_time, status_ty, 'r')
ty.plot(trajectory_time, trajectory_ty, 'b')
ty.set_ylabel('ty (mm)')

tz.plot(status_time, status_tz, 'r')
tz.plot(trajectory_time, trajectory_tz, 'b')
tz.set_xlabel('time (s)')
tz.set_ylabel('tz (mm)')

rx.plot(status_time, status_rx, 'r')
rx.plot(trajectory_time, trajectory_rx, 'b')
rx.set_ylabel('rx (deg)')

ry.plot(status_time, status_ry, 'r')
ry.plot(trajectory_time, trajectory_ry, 'b')
ry.set_ylabel('ry (deg)')

rz.plot(status_time, status_rz, 'r')
rz.plot(trajectory_time, trajectory_rz, 'b')
rz.set_xlabel('time (s)')
rz.set_ylabel('rz (deg)')

plt.get_current_fig_manager().window.attributes('-zoomed', True)
plt.show()
