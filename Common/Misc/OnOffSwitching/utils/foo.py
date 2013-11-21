import time
while True:
    try:
        print('foo')
        time.sleep(1)
    except KeyboardInterrupt, e:
        break

print('ok')
