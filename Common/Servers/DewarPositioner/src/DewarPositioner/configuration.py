import ComponentErrorsImpl

from xml.etree import ElementTree
from Acspy.Util import ACSCorba
from IRAPy import logger

class CDBConf(object):
    def __init__(self, path="alma/DataBlock/DewarPositioner/"):
        self.path = path
        self._make_conf()

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

    def _make_conf(self):
        """Create the mapping {'setupCode': 'derotatorName'}"""
        dal = ACSCorba.cdb()
        mapping_dao = dal.get_DAO(self.path + 'Mapping')
        mapping_root = ElementTree.fromstring(mapping_dao)
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


