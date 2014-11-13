import unittest2
from DewarPositioner.cdbconf import CDBConf
from ComponentErrors import ValidationErrorEx
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Util import ACSCorba


class CDBConfTest(unittest2.TestCase):
    def setUp(self):
        self.conf = CDBConf()
        self.dal = ACSCorba.cdb()
        # self.conf.setup('KKG')

    def test_getAttribute(self):
        """Test the getAttribute() method"""
        # It must raise an exception in case of wrong attribute name
        self.assertRaises(ValidationErrorEx, self.conf.getAttribute, 'foo')
        # It must get all the attributes listed in CDBConf.componentAttributes
        dao = self.dal.get_DAO_Servant(self.conf.componentPath)
        for name in self.conf.componentAttributes:
            value = dao.get_field_data(name)
            self.assertEqual(value, self.conf.getAttribute(name))

    def test_setConfiguration(self):
        """Test the setConfiguration() method"""
        # Vefify the configurationCode and path are not set before the setup
        self.assertEqual(self.conf.configurationCode, 'none')
        self.assertEqual(self.conf.configurationPath, '')
        # Vefify it raises an exception in case a setup is required
        self.assertRaises(ValidationErrorEx, self.conf.setConfiguration, 'BSC')
        # After the setup, it must raise an exception in case of wrong configuration
        setupCode = 'KKG'
        self.conf.setup(setupCode)
        self.assertRaises(ValidationErrorEx, self.conf.setConfiguration, 'foo')
        # Verify it sets properly the configuration
        confCode = 'BSC'
        self.conf.setConfiguration(confCode)
        self.assertEqual(self.conf.configurationCode, confCode)
        self.assertRegexpMatches(self.conf.configurationPath, confCode)
        self.assertTrue(any(self.conf.UpdatingPosition))
        confCode = 'FIXED'
        self.conf.setConfiguration(confCode)
        self.assertEqual(self.conf.configurationCode, confCode)
        self.assertRegexpMatches(self.conf.configurationPath, confCode)
        self.assertTrue(any(self.conf.UpdatingPosition))


    def test_getUpdatingConfiguration(self):
        """Test the getUpdatingConfiguration() method"""
        # It raises an exception in case a setConfiguration() is required
        self.assertRaises(ValidationErrorEx, self.conf.getUpdatingConfiguration, '')
        # Once configured, it raises an exception in case of wrong axis name
        self.conf.setup('KKG')
        self.conf.setConfiguration('BSC')
        self.assertRaises(ValidationErrorEx, self.conf.getUpdatingConfiguration, 'foo')
        # Get the position
        self.assertEqual(
                self.conf.getUpdatingConfiguration('JUST_FOR_TEST'), 
                {'initialPosition': '9.0', 'functionName': 'gpa'})
        self.conf.setConfiguration('FIXED')
        self.assertEqual(
                self.conf.getUpdatingConfiguration('JUST_FOR_TEST'), 
                {'initialPosition': '0', 'functionName': 'none'})


    def test_updateInitialPositions(self):
        """Test the updateInitialPositions() method"""
        self.conf.setup('KKG')
        self.conf.setConfiguration('BSC')
        # Get the position
        self.assertEqual(
                self.conf.getUpdatingConfiguration('JUST_FOR_TEST'), 
                {'initialPosition': '9.0', 'functionName': 'gpa'})
        self.conf.updateInitialPositions(100)
        self.assertEqual(
                self.conf.getUpdatingConfiguration('JUST_FOR_TEST'), 
                {'initialPosition': '100.00', 'functionName': 'gpa'})


    def test_setup(self):
        """Test the setup method"""
        # Vefify the Mapping attribute is not set before the setup
        self.assertRaises(ValidationErrorEx, self.conf.getAttribute, 'DerotatorName')
        # The default paths and codes must be empty strings
        self.assertEqual(self.conf.setupCode, '')
        self.assertEqual(self.conf.setupPath, '')
        self.assertEqual(self.conf.mappingPath, '')
        setupCode = 'KKG'
        self.conf.setup(setupCode)
        # Paths and codes must contain the setup code
        self.assertEqual(self.conf.setupCode, setupCode)
        self.assertRegexpMatches(self.conf.setupPath, setupCode)
        self.assertRegexpMatches(self.conf.mappingPath, setupCode)
        # It must get all the attributes listed in CDBConf.mappingAttributes
        dao = self.dal.get_DAO_Servant(self.conf.mappingPath)
        for name in self.conf.mappingAttributes:
            value = dao.get_field_data(name)
            self.assertEqual(value, self.conf.getAttribute(name))

if __name__ == '__main__':
    unittest2.main()
