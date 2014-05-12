from xml.etree import ElementTree
from Acspy.Util import ACSCorba

class CDBConf(object):
    def __init__(self, path="alma/DataBlock/DewarPositioner/"):
        self.path = path
        self._make_mapping()

    def derotator_name(self, setup_code):
        """Return None if no derotator exists for that code"""
        return self.mapping.get(setup_code)

    def _make_mapping(self):
        """Create the mapping {'setupCode': 'derotatorName'}"""
        dal = ACSCorba.cdb()
        mapping_dao = dal.get_DAO(self.path + 'Mapping')
        mapping_root = ElementTree.fromstring(mapping_dao)
        self.mapping = {}
        for child in mapping_root.getchildren():
            code, name = child
            self.mapping[code.text.strip()] = name.text.strip()

