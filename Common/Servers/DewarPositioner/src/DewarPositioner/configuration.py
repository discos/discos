import ComponentErrorsImpl

from xml.etree import ElementTree
from Acspy.Util import ACSCorba
from IRAPy import logger

class CDBConf(object):
    def __init__(
            self, data_path="alma/DataBlock/DewarPositioner/", 
            component_path='alma/RECEIVERS/DewarPositioner'):

        dal = ACSCorba.cdb()
        self.mapping_dao = dal.get_DAO(data_path + 'Mapping')
        self.component_dao = dal.get_DAO_Servant(component_path)
        self._make_conf()
        self._make_attr()

    def get_attribute(self, attrname):
        try:
            return getattr(self, attrname)
        except AttributeError, ex: 
            raeson = "invalid attribute name"
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

    def setup(self, setup_code):
        self.setup_code = setup_code

    def get_entry(self, entry_name):
        """Return the 'entry_name' of the DewarPositioner CDB table."""
        try:
            conf = self.mapping[self.setup_code]
        except KeyError:
            raeson = "code %s unknown" %self.setup_code
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except AttributeError, ex: # The mapping attribute is not yet created
            raeson = "invalid CDB configuration"
            logger.logError(raeson)
            logger.logDebug('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        try:
            return conf[entry_name]
        except KeyError:
            raeson = "no CDB entry called '%s'" %entry_name
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

    def _make_attr(self):
        self.updating_time = self.component_dao.get_double('UpdatingTime')
        self.rewinding_sleep_time = self.component_dao.get_double('RewindingSleepTime')
        self.rewinding_timeout = self.component_dao.get_double('RewindingTimeout')

    def _make_conf(self):
        """Create the mapping {'setupCode': 'derotatorName'}"""
        dal = ACSCorba.cdb()
        mapping_root = ElementTree.fromstring(self.mapping_dao)
        self.mapping = {}
        for child in mapping_root.getchildren():
            code, name, starting_position = child
            try:
                conf = {
                        'derotator_name': name.text.strip(), 
                        'starting_position': float(starting_position.text)
                }
            except (ValueError, TypeError), ex:
                raeson = "Invalid CDB entry"
                logger.logError('%s: %s' %(raeson, ex.message))
                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(raeson)
                raise exc
            except Exception, ex:
                raeson = "Unknown error in the CDB configuration"
                logger.logError('%s: %s' %(raeson, ex.message))
                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(raeson)
                raise exc

            self.mapping[code.text.strip()] = conf


