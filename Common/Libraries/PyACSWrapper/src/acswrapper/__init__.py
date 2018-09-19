import os

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


ACSDATA = os.getenv('ACSDATA')
logfile = os.path.join(ACSDATA, 'logs', '%s.log' % __name__)
