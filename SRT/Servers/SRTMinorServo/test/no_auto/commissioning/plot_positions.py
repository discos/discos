import numpy as np
import matplotlib.pyplot as plt
import matplotlib.text as text
import sys

x = []
expected = []
history = []
with open(sys.argv[1] + '.data') as f:
    for line in f:
        if line.startswith('#'):
            continue
        else:
            if line.startswith('Time'):
                # Time: exe_time + 00200 ms
                first, second = [token.strip() for token in line.split('+')]
                t = int(second.rstrip('ms')) / 1000
                x.append(t)
                continue
            elif line.startswith('Expected'):
                # Expected: 0.0000 0.0000 0.2000 0.0000 0.0000 0.0000
                first, second = [token.strip() for token in line.split(':')]
                pos = [float(token) for token in second.split()]
                expected.append(pos[2])
                continue
            elif line.startswith('History'):
                # History:  0.0004 -0.0002 0.0317 0.0000 0.0000 -0.0000
                first, second = [token.strip() for token in line.split(':')]
                pos = [float(token) for token in second.split()]
                history.append(pos[2])
                continue

t = np.array(x)
e = np.array(expected)
h = np.array(history)

fig, ax = plt.subplots()
plt.plot(t, e, 'k', t, h, 'k--')
plt.legend(('Expected position', 'Real position'),
           'upper center', shadow=True)
offset0 = -0.2
t0 = offset0 # Starting time
tf = x[-1] - x[0]  # Final time

y0h = h[0] + offset0 if h[0] <=0 else h[0] - offset0
yfh = h[-1] + 2 if h[-1] > 0 else h[-1] - 2
y0e = e[0] + offset0 if e[0] <=0 else e[0] - offset0
yfe = e[-1] + 2 if e[-1] > 0 else e[-1] - 2
y0 = y0e if y0e < y0h else y0h
yf = yfe if yfe > y0h else yfh

plt.ylim([y0, yf]) if y0 < yf else plt.ylim([yf, y0])
plt.xlim([t0, tf])
plt.grid(False)
plt.xlabel('Time (s)')
plt.ylabel('Position (mm)')
plt.title('SRP movement')

# match on arbitrary function
def myfunc(x):
    return hasattr(x, 'set_color') and not hasattr(x, 'set_facecolor')

for o in fig.findobj(myfunc):
    o.set_color('blue')

# match on class instances
for o in fig.findobj(text.Text):
    o.set_fontstyle('italic')


plt.show()
