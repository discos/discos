#!/usr/bin/env python
import time
from ZMQLibrary import ZMQPublisher

pub = ZMQPublisher("python")
#pub = ZMQPublisher("python", '127.0.0.1', 16001)

print(f"Started Python ZMQPublisher on topic '{pub.topic}'")

try:
    while True:
        dictionary = {
            'string': 'string value',
            'integer': 10,
            'timestamp': time.time()
        }
        pub.publish(dictionary)
        time.sleep(1)
except KeyboardInterrupt:
    print()
