from __future__ import division
import datetime
import ephem

file_name = 'example.txt'
outfile = 'horizons_test.txt'
format_ = '%Y-%b-%d %H:%M'
# ... Convert from date to str... add an incremental offset and replace...
for line in open(file_name) as f:
    items = line.split()
    if len(items) not in (21, 22):
        raise Exception('Unexpected number of items in %s' %file_name)
    datestr = items[0] + ' ' + items[1]
    delotstr = items[17] if len(items) == 21 else items[18]
    vrad = float(delotstr)
    date = datetime.datetime.strptime(datestr, format_)
    horizons.append((date, vrad))


flab = 22235.08
fi = 145.0 # MHz, frequenza inferiore della banda di lavoro del backend
fs = 207.0 # MHz, frequenza superiore della banda di lavoro del backend

for date, vrad in horizons:
    vobs = flab * (1 - vrad/(ephem.c/1000))
    lo = vobs - ((fs + fi)/2)
    print(lo)



