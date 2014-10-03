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
            'RewindingTimeout'
    )

    mappingAttributes = (
            'DerotatorName', 
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

    def setConfiguration(self, configurationCode):
        if self.setupCode:
            confPath = os.path.join(self.setupPath, configurationCode)
            # Set the path self.configurationPath if the directory exists
            self._setPath(name='configuration', path=confPath)
            self.configurationCode = configurationCode
        else:
            raeson = "cannot set the configuration: a setup is required"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        self._makeRecords(
            dictName='initialPosition',
            path=os.path.join(self.configurationPath, 'InitialPosition'))
        self.isConfigured = True

    def getInitialPosition(self, axisCode):
        """Take an axis and return its initial position"""
        if not self.isConfigured:
            raeson = "DewarPotitioner not configured"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        else:
            try:
                # initialPosition -> {'axisCode': ['position']}
                values = self.initialPosition[axisCode] 
                return float(values[0]) 
            except KeyError:
                raeson = "axis code %s does not exist" %axisCode
                logger.logError(raeson)
                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(raeson)
                raise exc

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
                self.attributes[name] = dao.get_field_data(name)
            except cdbErrType.CDBFieldDoesNotExistEx:
                raeson = "CDB field %s does not exist" %name
                logger.logWarning(raeson)
                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(raeson)
                raise exc

    def _makeRecords(self, dictName, path):
        """Make a dictionary of CDB records 

        The parameter `dictName` is the name we want to give the dictionary. 
        For instance, `dictName='initialPosition'`.
        The parameter `path` is a the relative path to the directory containing
        the table. For instance:

            path = 'alma/DataBlock/DewarPositioner/KKG/BSC/InitialPosition'

        The method `_makeRecords()` makes a dictionary {'primary_key': values},
        where `primary_key` is the primary key of the record, and `values` 
        is a list of the related values, stored as strings. For instance, for 
        the InitialPosition table we have records like these::

            <PositionRecord>
                <axisCode>AZ</axisCode>
                <position>2.0</position>
            </PositionRecord>
            
            <PositionRecord>
                <axisCode>EL</axisCode>
                <position>3.0</position>
            </PositionRecord>

        where `axisCode` is the primary key and there is just the `position`
        value. So in that case, if for instance dictName='initialPosition', 
        the method builds the dictionary::

            initialPosition = {
                'AZ': ['2.0'],
                'EL': ['3.0']
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
        except cdbErrType.CDBXMLErrorEx:
            children = () # In case of empty table, like the FIXED confi

        setattr(self, dictName, {})
        d = getattr(self, dictName)
        for child in children:
            items = [item.text.strip() for item in child]
            primary_key, values = items[0], items[1:]
            d[primary_key] = values # Put the record in the `dictName`

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
            # reason="setup alma/DataBlock/KKG not available"
            raeson = "%s %s not available" %(name, abs_path)
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

    def _setDefaults(self):
        self.isConfigured = False
        self.setupCode = '' # For instance, KKG
        self.configurationCode = '' # For instance, BSC
        self.setupPath = '' # Path to the directory code (ie. to KKG)
        self.mappingPath = '' # Mapping directory path
        self.configurationPath = '' # Configuration directory path (ie. to BSC)


