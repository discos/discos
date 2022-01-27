#!/usr/bin/env python
#*******************************************************************************
# ALMA - Atacama Large Millimiter Array
# (c) Associated Universities Inc., 2002 
# (c) European Southern Observatory, 2002
# Copyright by ESO (in the framework of the ALMA collaboration)
# and Cosylab 2002, All rights reserved
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
# MA 02111-1307  USA
#
# @(#) $Id: acspyexmplDynamicHelloWorld.py,v 1.10 2006/03/30 16:58:36 dfugate Exp $


'''
DESCRIPTION
This client consists of an example which asks manager to create a dynamic
<a href="../../idl/html/interfaceHelloWorld_1_1HelloWorld.html">Hello World</a>
component for it. This is done by changing some of the default parameters to the
getComponent method of the ContainerServices class. Once the reference to the newly
created component has been retrieved, a method is invoked to show that the
everything is working.

WHAT CAN I GAIN FROM THIS EXAMPLE?
- PySimpleClient usage.
- Accessing (remote) components.
- Using PySimpleClient to create and reference dynamic components.
- Manipulating default parameters of ContainerServices methods.

LINKS
- <a href="../../idl/html/interfaceHelloWorld_1_1HelloWorld.html">HelloWorld IDL Documentation</a>
'''

# Import command-line arguments
from sys import argv

# Import the SimpleClient class
from Acspy.Clients.SimpleClient import PySimpleClient

import Antenna
from math import radians,degrees

import Acspy.Common.TimeHelper
import time
'''
Date__(UT)__HR:MN     R.A.___(ICRF)___DEC  R.A._(a-appar)_DEC.  Azi____(a-app)___Elev
**************************************************************************************
2022-Jan-21 00:00:00.000  m  336.65825 -10.79656  336.94218 -10.68735  312.371754 -52.320793
 2021-Apr-15 00:00 2459319.500000000     328.035447923 -13.662097629  327.791152902 -13.167532812   84.489404675 -27.482527702  5.49776328729022 -23.8801089
                                         328.03548968344006, -13.662066250524141

 328.03548968344006, -13.66206625052414
'''



epoch = Acspy.Common.TimeHelper.TimeUtil()

struct_time = time.strptime("2021-Apr-15 00:00", "%Y-%b-%d %H:%M")

tm=time.mktime(struct_time)



acstime=epoch.py2epoch(tm)

print (acstime.value)
# Make an instance of the PySimpleClient
simpleClient = PySimpleClient()

# Obtain a reference to a dynamic component using the ContainerServices
# getComponent method.
hwRef = simpleClient.getDynamicComponent(None,
                                         "IDL:alma/Antenna/SolarSystemBody:1.0",
                                         "SolarSystemBodyImpl",None)

if hwRef != None:
    simpleClient.getLogger().logInfo("Retrieved valid reference from manager.")
    #Do something useful with the reference.
    hwRef.setBodyName('Jupiter')
    sourceID = J2000RightAscension=  J2000Declination= rightAscension= declination= julianEpoch= gLongitude= gLatitude= azimuth= elevation= parallacticAngle=0.
    
    userAzimuthOffset=userElevationOffset= userRightAscensionOffset= userDeclinationOffset= userLongitudeOffset= userLatitudeOffset=axis=angularSize=0.
    distance= radialVelocity= vradFrame= vradDefinition=0.
#    hwRef.getAttributes(sourceID, J2000RightAscension, J2000Declination, rightAscension, declination, julianEpoch, gLongitude, gLatitude, azimuth, elevation, parallacticAngle, userAzimuthOffset, userElevationOffset, userRightAscensionOffset,
#    userDeclinationOffset, userLongitudeOffset, userLatitudeOffset, axis, angularSize, distance, radialVelocity, vradFrame, vradDefinition)
#print(att)

#    az,el,ra,dec,l,lon,lat = hwRef.getAllCoordinates(acstime.value)

    ra,dec=hwRef.getJ2000EquatorialCoordinate(acstime.value)

#    print ("az,el",degrees(az),degrees(el))
    print ("ra,dec",degrees(ra),degrees(dec))
    
    
    att=hwRef.getAttributes()
    dec2000=att.J2000Declination 
    ra2000=att.J2000RightAscension
    print(degrees(ra2000),degrees(dec2000))
    

    simpleClient.getLogger().logInfo("Method of dynamic component successfully invoked. Have a nice day!")
else:
    simpleClient.getLogger().logAlert("Bad reference retrieved from manager")

simpleClient.disconnect()
print "The end __oOo__"

