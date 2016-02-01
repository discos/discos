#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from IRAPy import logger


i = 0
try:
    while True:
        logger.logNotice("logging process: %i" % (i,))
        i += 1
except:
    logger.logNotice("logging process ends")
    sys.exit(0)

