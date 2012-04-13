#!/usr/bin/env python
# @(#) $Id: XBackendsErrorsImpl.py,v 1.1 2010/06/21 09:08:59 bliliana Exp $
#
#    ALMA - Atacama Large Millimiter Array
#    (c) Associated Universities, Inc. Washington DC, USA,  2001
#    (c) European Southern Observatory, 2002
#    Copyright by ESO (in the framework of the ALMA collaboration)
#    and Cosylab 2002, All rights reserved
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
######################################################################
'''
Some form of custom documentation goes here...
'''
######################################################################
from   Acspy.Common.Err import ACSError
import ACSErr
import XBackendsErrors
from Acspy.Common.TimeHelper import getTimeStamp
######################################################################

class BaseException:
    '''
    Class serves as a base exception for all error type/code exception
    pairs defined within this module. The reason this is provided is so 
    that one can generically catch ACS Error System based Python 
    exceptions using a single Python "except BaseException, e:" type
    statement.
    '''
    pass
######################################################################
class NoSettingExImpl(XBackendsErrors.NoSettingEx, ACSError, BaseException):
    '''
    Some form of custom documentation goes here...
    '''
    #-----------------------------------------------------------------
    def __init__(self,
                 nvSeq = None,
                 exception = None,
                 create = 1,
                 severity = None):
        '''
        Constructor
        
        An instance of this class is derived from the CORBA class of 
        similar name. The difference between the two is that this class
        provides many additional helper methods from Acspy.Common.Err.

        There are three different combinations of keyword parameter
        uses that make sense here:

            __init__()
              Using the default values creates a new exception which 
              does not include any previous error traces
            
            __init__(exception=someOldException)
              Specifying a previous ACS Error System exception or 
              without changing the value of create 
              creates a new exception which does in fact include 
              previous error traces from someOldException.

            __init__(exception=someOldException, create=0)
              Used to reconstruct someOldException without adding any
              new error trace information. It is absolutely critical
              that someOldException be of the same CORBA type as this
              class implements!

            nvSeq default keyword parameter
              This sequence of name/values is only used when a new 
              exception is being created. In simple terms, the only
              time you can use it is when the create keyword parameter
              has the value of 1

            severity default keyword parameter
              This CORBA type corresponds to ACSErr.Severity. The
              only time you can use it is when the create keyword parameter
              has the value of 1

        Parameters:
        - nvSeq is a sequence of ACSErr.NameValue pairs used to add
        additional information about the exception. Only used when
        create has a value of 1/True
        - exception is an ACS Error System based CORBA exception
        Provide this to extract previous error trace 
        information and put this into the new object being constructed
        - create is a boolean value which defines whether or not traceback
        information should be extracted from the call to create this exception
        and added to it's error trace. If you're simply trying to recreate
        a remote CORBA exception locally and figure out what went wrong
        most likely you want create to have a value of 0. However, if you
        intend on rethrowing the exception a value of 1 makes more sense.
        - severity is used to set the severity of exception. Only used when
        create has a value of 1/True
        '''
        if nvSeq == None:
            nvSeq = []
        self.shortDescription = "Device in not Setting"
        description = self.shortDescription
        ACSError.__init__(self,  
                          ACSErr.XBackendsErrors,
                          XBackendsErrors.NoSetting,
                          exception,
                          description,
                          nvSeq,                          
                          create,
                          severity)
        XBackendsErrors.NoSettingEx.__init__(self, self.errorTrace)
        return
    #--------------------------------------------------------------------------        
    def getXBackendsErrorsEx(self):
        '''
        Returns this exception converted into an XBackendsErrorsEx
        '''
        return XBackendsErrors.XBackendsErrorsEx(self.getErrorTrace())


######################################################################
class NoSettingCompletionImpl(ACSErr.Completion, ACSError):
    '''
    Some form of custom documentation goes here...
    '''
    #-----------------------------------------------------------------
    def __init__(self,
                 nvSeq = None,
                 exception = None,
                 create = 1,
                 severity = None):
        '''
        Constructor
        
        An instance of this class is derived from ACSErr.Completion. 
        It provides many helper methods from Acspy.Common.Err.
        
        There are three different combinations of keyword parameter
        uses that make sense here:
            
            __init__()
              Using the default values creates a new Completion which 
              does not include any previous error traces
            
            __init__(exception=acsException)
              Specifying a previous ACS Error System exception without
              changing the value of create creates a new Completion which
              does in fact include previous error traces from
              acsException.
            
            __init__(exception=acsException, create=0)
              Used to reconstruct acsException without adding any
              new error trace information.

            nvSeq default keyword parameter
              This sequence of name/values is only used when a new 
              Completion is being created. In simple terms, the only
              time you can use it is when the create keyword parameter
              has the value of 1

            severity default keyword parameter
              This CORBA type corresponds to ACSErr.Severity. The
              only time you can use it is when the create keyword parameter
              has the value of 1

        Parameters:
        - nvSeq is a sequence of ACSErr.NameValue pairs used to add
        additional information about the Completion. Only used when
        create has a value of 1
        - exception is an ACS Error System based CORBA exception. 
	Provide this to extract previous error trace information and put this into
        the new object being constructed
        - create is a boolean value which defines whether or not traceback
        information should be extracted from the call to create this Completion
        and added to it's error trace. If you're simply trying to recreate
        a remote CORBA exception locally and figure out 
        what went wrong most likely you want create to have a value of 0. 
        However, if you intend on returning the Completion a value of 1 makes 
        more sense.
        - severity is used to set the severity of the completion. Only used when
        create has a value of 1/True
        '''
        if nvSeq == None:
            nvSeq = []
        self.shortDescription = "Device in not Setting"
        description = self.shortDescription
        ACSError.__init__(self,
                          ACSErr.XBackendsErrors,
                          XBackendsErrors.NoSetting,
                          exception,
                          description,
                          nvSeq,
                          create,
                          severity)

        #Create the CORBA object
        ACSErr.Completion.__init__(self,
                                   self.getTimeStamp(),
                                   self.getErrorType(),
                                   self.getErrorCode(),
                                   [self.errorTrace])
        return


######################################################################
class NoErrorCompletionImpl(ACSErr.Completion, ACSError):
    '''
    Some form of custom documentation goes here...
    '''
    #-----------------------------------------------------------------
    def __init__(self,
                 nvSeq = None,
                 exception = None,
                 create = 0,
                 severity = None):
        '''
        Constructor
        
        An instance of this class is derived from ACSErr.Completion. 
        It provides many helper methods from Acspy.Common.Err.
        
        There are three different combinations of keyword parameter
        uses that make sense here:
            
            __init__()
              Using the default values creates a new Completion which 
              does not include any previous error traces
            
            __init__(exception=acsException)
              Specifying a previous ACS Error System exception without
              changing the value of create creates a new Completion which
              does in fact include previous error traces from
              acsException.
            
            __init__(exception=acsException, create=0)
              Used to reconstruct acsException without adding any
              new error trace information.

            nvSeq default keyword parameter
              This sequence of name/values is only used when a new 
              Completion is being created. In simple terms, the only
              time you can use it is when the create keyword parameter
              has the value of 1

            severity default keyword parameter
              This CORBA type corresponds to ACSErr.Severity. The
              only time you can use it is when the create keyword parameter
              has the value of 1

        Parameters:
        - nvSeq is a sequence of ACSErr.NameValue pairs used to add
        additional information about the Completion. Only used when
        create has a value of 1
        - exception is an ACS Error System based CORBA exception. 
	Provide this to extract previous error trace information and put this into
        the new object being constructed
        - create is a boolean value which defines whether or not traceback
        information should be extracted from the call to create this Completion
        and added to it's error trace. If you're simply trying to recreate
        a remote CORBA exception locally and figure out 
        what went wrong most likely you want create to have a value of 0. 
        However, if you intend on returning the Completion a value of 1 makes 
        more sense.
        - severity is used to set the severity of the completion. Only used when
        create has a value of 1/True
        '''
        if nvSeq == None:
            nvSeq = []
        self.shortDescription = "No error"
        description = self.shortDescription
        ACSError.__init__(self,
                          ACSErr.XBackendsErrors,
                          XBackendsErrors.NoError,
                          exception,
                          description,
                          nvSeq,
                          create,
                          severity)

        #Create the CORBA object
        ACSErr.Completion.__init__(self,
                                   getTimeStamp().value,
                                   0,
                                   0,
                                   [])
        return

    def isOK(self):
        return 1

    def getDescription(self):
        return self.shortDescription

    def getErrorCode(self):
        return 0

    def getErrorType(self):
        return 0


######################################################################
if __name__ == "__main__":

    try:
        raise NoSettingExImpl
    except XBackendsErrors.NoSettingEx, e:
        print "Caught the correct type of exception:", e
        g = NoSettingExImpl(exception=e)
        g.Print()
    except Exception, e:
        print "Caught the wrong type of exception:", e


    joe = NoErrorCompletionImpl()
    joe.Print()


    joe = NoSettingCompletionImpl()
    joe.Print()


    print
