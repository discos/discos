import os
import ComponentErrorsImpl
import cdbErrType

from xml.etree import ElementTree
from Acspy.Util import ACSCorba
from IRAPy import logger


class CDBConf(object):

    componentAttributes = (
            'UpdatingTime', 
            'RewindingSleepTime', 
            'RewindingTimeout',
            'DefaultConfiguration',
            'DefaultRewindingMode',
            'Latitude',
    )

    mappingAttributes = (
            'DerotatorName', 
            'SetupPosition',
            'ParkPosition',
    )

    actionsAttributes = (
            'SetCustomPositionAllowed',
            'DynamicUpdatingAllowed',
            'AddInitialParallactic',
    )

    configurationRecords = (
            'UpdatingPosition',
    )

    def __init__(
            self, 
            componentPath='alma/RECEIVERS/DewarPositioner',
            datablockPath="alma/DataBlock/DewarPositioner/"):

        self.attributes = {} # {name(str): value(str)} 
        self.componentPath = componentPath
        self.datablockPath = datablockPath
        self._setDefaults()
        self._makeAttr(componentPath, CDBConf.componentAttributes)

    def setup(self, setupCode):
        """Set the DerotatorName attribute given from the CDB Mapping record"""
        setupPath = os.path.join(self.datablockPath, setupCode)
        self._setPath(name='setup', path=setupPath)
        mappingPath = os.path.join(setupPath, 'Mapping')
        self._setPath(name='mapping', path=mappingPath)
        self._makeAttr(self.mappingPath, CDBConf.mappingAttributes)
        self.setupCode = setupCode
        self.setConfiguration(self.getAttribute('DefaultConfiguration'))

    def setConfiguration(self, configurationCode):
        if self.setupCode:
            confPath = os.path.join(self.setupPath, configurationCode)
            # Set the path self.configurationPath if the directory exists
            self._setPath(name='configuration', path=confPath)
        else:
            raeson = "cannot set the configuration: a setup is required"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        # Create records
        self._clearRecords()
        for record in CDBConf.configurationRecords:
            self._makeRecords(
                dictName=record,
                path=os.path.join(self.configurationPath, record))
        # Create attributes
        actionsPath = os.path.join(self.configurationPath, 'Actions')
        self._setPath(name='actions', path=actionsPath)
        self._clearAttributes()
        self._makeAttr(self.actionsPath, CDBConf.actionsAttributes)
        self.configurationCode = configurationCode
        self._isConfigured = True

    def isConfigured(self):
        return self._isConfigured

    def getConfiguration(self):
        return self.configurationCode

    def getUpdatingConfiguration(self, axisCode):
        """Take an axis and return the dict {'initialPosition': X, 'functionName': Y)"""
        if not self._isConfigured:
            raeson = "DewarPositioner not configured"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        else:
            try:
                # UpdatingPosition -> {'axisCode': {'initialPosition': X, 'functionName': Y}}
                return self.UpdatingPosition[axisCode] 
            except IndexError:
                raeson = "wrong CDB configuration for %s" %axisCode
                logger.logError(raeson)

                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(raeson)
                raise exc
            except KeyError:
                raeson = "axis code %s does not exist" %axisCode
                logger.logError(raeson)
                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(raeson)
                raise exc
            except AttributeError:
                raeson = "this configuration does not have an updating position dictionary"
                logger.logError(raeson)
                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(raeson)
                raise exc


    def updateInitialPositions(self, position):
        for axis in self.UpdatingPosition:
            self.UpdatingPosition[axis]['initialPosition'] = '%.2f' %position


    def getAttribute(self, name):
        """Return the attribute as a string.

        The attribute we want to get from the CDB are listed in
        `CDBConf.componentAttributes` and `CDBConf.mappingAttributes`.
        """
        try:
            return self.attributes[name]
        except KeyError:
            raeson = "attribute %s not available" %name
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

    def _makeAttr(self, path, attributes):
        """Make a dictionary of the CDB attributes

        The parameter `path` is a the relative path to the directory. 
        For instance:

            path = 'alma/DataBlock/DewarPositioner/KKG/Mapping'.

        `attributes` is a sequence of attributes we want to set. For instance,
        if the Mapping.xml contains just the attribute 'DerotatorName':

            attributes = ('DerotatorName',)

        The method makes a dictionari {'attribute_name': 'attribute_value'},
        where the value, as the attribute_name, is stored as a string.
        """
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO_Servant(path)
        except cdbErrType.CDBRecordDoesNotExistEx:
            raeson = "CDB record %s does not exists" %path
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        for name in attributes:
            try:
                self.attributes[name] = dao.get_field_data(name).strip()
            except cdbErrType.CDBFieldDoesNotExistEx:
                raeson = "CDB field %s does not exist" %name
                logger.logWarning(raeson)
                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(raeson)
                raise exc
 
    def _makeRecords(self, dictName, path):
        """Make a dictionary of CDB records 

        The parameter `dictName` is the name we want to give the dictionary. 
        For instance, `dictName='UpdatingPosition'`.
        The parameter `path` is a the relative path to the directory containing
        the table. For instance:

            path = 'alma/DataBlock/DewarPositioner/KKG/BSC/UpdatingPosition'

        The method `_makeRecords()` makes a dictionary {'primary_key': values},
        where `primary_key` is the primary key of the record, and `values` 
        is a list of the related values, stored as strings. For instance, for 
        the UpdatingPosition table we have records like these::

            <UpdatingRecord>
                <axisCode>MNG_TRACK</axisCode>
                <initialPosition>2.0</initialPosition>
                <functionName>parallactic</functionName>
            </UpdatingRecord>
            
        where `axisCode` is the primary key and there are two values, one for `position`
        and one for the function name. So in that case, if for instance 
        dictName='InitialPosition', the method builds the dictionary::

            UpdatingPosition = {
                'MNG_TRACK': {'initialPosition': '2.0', 'functionName': 'parallactic'],
            }
        """
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO(path)
            children = ElementTree.fromstring(dao).getchildren()
        except cdbErrType.CDBRecordDoesNotExistEx:
            raeson = "CDB record %s does not exists" %path
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            children = () 

        setattr(self, dictName, {})
        d = getattr(self, dictName)
        for child in children:
            valuesDict = {}
            primaryKey, fields = child[0].text.strip(), child[1:]
            for field in fields:
                # Find the field name
                # e.g. field.tag = '...DewarPositionerUpdatingTable:1.0}axisCode'
                idx = field.tag.find('}') + 1
                fieldName = field.tag[idx:]
                fieldValue = field.text.strip()
                valuesDict[fieldName] = fieldValue
            d[primaryKey] = valuesDict


    def _setPath(self, name, path):
        ACS_CDB = os.getenv('ACS_CDB')
        if not os.path.isdir(ACS_CDB):
            raeson = "$ACS_CDB path do not point to an existing directory"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        else:
            abs_path = os.path.join(ACS_CDB, 'CDB', path)

        if os.path.isdir(abs_path):
            setattr(self, '%sPath' %name, path)
        else:
            # For instance: name='setup' and path='alma/DataBlock/KKG', so
            # raeson="setup alma/DataBlock/KKG not available"
            raeson = "%s %s not available" %(name, path)
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

    def clearConfiguration(self):
        self._isConfigured = False
        self.configurationCode = 'none' # For instance, BSC
        self.configurationPath = '' # Configuration directory path (ie. to BSC)
        self._clearRecords()
        self._clearAttributes()

    def _clearRecords(self):
        for name in CDBConf.configurationRecords:
            if hasattr(self, name):
                attr = getattr(self, name)
                attr.clear() # Clear the dictionary

    def _clearAttributes(self):
        for name in CDBConf.actionsAttributes:
            if hasattr(self, name):
                attr = getattr(self, name)
                attr.clear() # Clear the dictionary

    def _setDefaults(self):
        self.setupCode = '' # For instance, KKG
        self.setupPath = '' # Path to the directory code (ie. to KKG)
        self.mappingPath = '' # Mapping directory path
        self.clearConfiguration()


