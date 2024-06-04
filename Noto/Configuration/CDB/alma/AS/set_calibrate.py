import os, fnmatch

path = os.path.dirname(os.path.abspath(__file__))

result = []
for root, dirs, files in os.walk(path):
    for name in files:
        if fnmatch.fnmatch(name, 'USD*.xml'):
            result.append(os.path.join(root, name))

counter = 0

for filename in result:
    counter += 1
    os.rename(filename, filename + '.bak')
    original = open(filename + '.bak', 'r')
    new = open(filename, 'w')
    for fileline in original:
        new.write(fileline.replace('calibrate="0"', 'calibrate="1"'))
    original.close()
    os.remove(filename + '.bak')
    new.close()

print(str(counter) + " files edited.")
