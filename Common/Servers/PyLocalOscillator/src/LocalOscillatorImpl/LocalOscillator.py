#! /usr/bin/env python
#*******************************************************************************
# ALMA - Atacama Large Millimiter Array
# (c) UNSPECIFIED - FILL IN, 2015 
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
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
#
# "@(#) $Id$"
#
# who       when      what
# --------  --------  ----------------------------------------------
# spoppi  2015-10-14  created
#

#************************************************************************
#   NAME
# 
#   SYNOPSIS
# 
#   DESCRIPTION
#
#   FILES
#
#   ENVIRONMENT
#
#   RETURN VALUES
#
#   CAUTIONS
#
#   EXAMPLES
#
#   SEE ALSO
#
#   BUGS     
#
#------------------------------------------------------------------------
#
import Receivers__POA
import Receivers
import ReceiversErrorsImpl
import Acspy.Util.ACSCorba
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent
from Acspy.Servants.ContainerServices import ContainerServices 
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle 
from Acspy.Util.BaciHelper import addProperty
from LocalOscillatorImpl.devios import amplitudeDevIO, frequencyDevIO, isLockedDevIO, connectedDevIO
from LocalOscillatorImpl import CommandLine
from IRAPy import logger

 
class LocalOscillator(Receivers__POA.LocalOscillator, CharacteristicComponent, ContainerServices, ComponentLifecycle):

    def __init__(self):
        CharacteristicComponent.__init__(self)
        ContainerServices.__init__(self)
        self.cl = None
        self.freq = 0.0  
        self.power = 0.0
      
    def cleanUp(self):
        if self.cl:
            self.cl.close()
   
    def initialize(self):
        name = self.getName()
        dal = Acspy.Util.ACSCorba.cdb()
        dao = dal.get_DAO_Servant("alma/" + name)
        IP = dao.get_string("IP")
        PORT = int(dao.get_double("PORT"))

        self.cl = CommandLine.CommandLine(IP, PORT)

        addProperty(self, 'frequency', devio_ref=frequencyDevIO(self.cl))
        addProperty(self, 'amplitude', devio_ref=amplitudeDevIO(self.cl))
        addProperty(self, 'isLocked', devio_ref=isLockedDevIO(self, self.cl))
        addProperty(self, 'connected', devio_ref=connectedDevIO(self.cl))

        try:
            self.cl.rfOn()
            logger.logNotice("Synthetiser connected and RF turned on.")
        except ReceiversErrorsImpl.SynthetiserErrorExImpl as ex:
            logger.logWarning(
                "Could not reach hardware during initialization,"
                f"starting disconnected. {ex.getDetails()}"
            )
   
    def set(self, rf_power, rf_freq):
        try:
            self.cl.setPower(rf_power)
            self.cl.setFrequency(rf_freq)
            self.freq = rf_freq
            self.power = rf_power
            logger.logNotice('SYNT FREQ  set to %f ' %self.freq)
            logger.logNotice('SYNT POWER set to %f ' %self.power)
        except ReceiversErrorsImpl.SynthetiserErrorExImpl as exc:
            raise exc.getReceiversErrorsEx()

    def get(self):
        try:
            msg, power = self.cl.getPower()
            msg, freq = self.cl.getFrequency()
            return (power, freq)
        except ReceiversErrorsImpl.SynthetiserErrorExImpl as exc:
            raise exc.getReceiversErrorsEx()
  
    def rfon(self):
        try:
            self.cl.rfOn()
        except ReceiversErrorsImpl.SynthetiserErrorExImpl as exc:
            raise exc.getReceiversErrorsEx()
  
    def rfoff(self):
        try:
            self.cl.rfOff()
        except ReceiversErrorsImpl.SynthetiserErrorExImpl as exc:
            raise exc.getReceiversErrorsEx()
