# The following lines will look for the libPySRTMinorServoCommandLibrary in the
# $INTROOT/lib path instead of looking in the $INTROOT/lib/python/site-packages
# path. This is a workaround needed since the said library is a C++ shared
# library and it does not get automatically installed in the correct folder.
# By adding this workaround we're also able to rename the library with the
# module name SRTMinorServoCommandLibrary, trimming the 'libPy' header. I
# suggest to use this approach whenever a Python wrapper is needed.
import os
import sys
# Temporarily add the $INTROOT/lib path to Python libraries path
sys.path.append(os.path.join(os.environ['INTROOT'], 'lib'))
# Import the functions we need
from libPySRTMinorServoCommandLibrary import status, setup, stop, stow, preset, programTrack
# Remove the added path and unused imported modules
sys.path.remove(os.path.join(os.environ['INTROOT'], 'lib'))
del os
del sys
