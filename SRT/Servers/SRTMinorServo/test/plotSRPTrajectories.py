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
status_ez1 = []
status_ez2 = []
status_ez3 = []
status_ey1 = []
status_ey2 = []
status_ex1 = []
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
        status_ez1.append(float(args[7]))
        status_ez2.append(float(args[8]))
        status_ez3.append(float(args[9]))
        status_ey1.append(float(args[10]))
        status_ey2.append(float(args[11]))
        status_ex1.append(float(args[12]))

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
gs = fig.add_gridspec(6, 2, hspace=0)
((tx, rx), (ty, ry), (tz, rz), (ez1, ey1), (ez2, ey2), (ez3, ex1)) = gs.subplots(sharex=True, sharey='col')
tx.plot(status_time, status_tx, 'r')
tx.plot(trajectory_time, trajectory_tx, 'b')
tx.set_ylabel('tx (mm)')

ty.plot(status_time, status_ty, 'r')
ty.plot(trajectory_time, trajectory_ty, 'b')
ty.set_ylabel('ty (mm)')

tz.plot(status_time, status_tz, 'r')
tz.plot(trajectory_time, trajectory_tz, 'b')
tz.set_ylabel('tz (mm)')

ez1.plot(status_time, status_ez1, 'b')
ez1.set_ylabel('ez1 (mm)')

ez2.plot(status_time, status_ez2, 'b')
ez2.set_ylabel('ez2 (mm)')

ez3.plot(status_time, status_ez3, 'b')
ez3.set_ylabel('ez3 (mm)')
ez3.set_xlabel('time (s)')


rx.plot(status_time, status_rx, 'r')
rx.plot(trajectory_time, trajectory_rx, 'b')
rx.set_ylabel('rx (deg)')

ry.plot(status_time, status_ry, 'r')
ry.plot(trajectory_time, trajectory_ry, 'b')
ry.set_ylabel('ry (deg)')

rz.plot(status_time, status_rz, 'r')
rz.plot(trajectory_time, trajectory_rz, 'b')
rz.set_ylabel('rz (deg)')

ey1.plot(status_time, status_ey1, 'b')
ey1.set_ylabel('ey1 (mm)')

ey2.plot(status_time, status_ey2, 'b')
ey2.set_ylabel('ey2 (mm)')

ex1.plot(status_time, status_ex1, 'b')
ex1.set_ylabel('ex1 (mm)')
ex1.set_xlabel('time (s)')


plt.get_current_fig_manager().window.attributes('-zoomed', True)
plt.show()
