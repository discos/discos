import unittest
from xml.etree import ElementTree
from Acspy.Util import ACSCorba

class TestProcedures(unittest.TestCase):

    def setUp(self):
        dal = ACSCorba.cdb()
        dao = dal.get_DAO('alma/Procedures/StationProcedures')
        self.procedures = ElementTree.fromstring(dao).getchildren()

    def test_reset_not_allowed_in_setup(self):
        """Vefify the setup does not execute the antennaReset command"""
        for procedure in self.procedures:
            if procedure.get('name').startswith('setup'):
                body = procedure[0]
                commands = [cmd.strip() for cmd in body.text.split()]
                self.assertNotIn('antennaReset', commands)

if __name__ == '__main__':
    unittest.main()
