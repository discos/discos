import unittest
from config import Configuration
        
class TestConfiguration(unittest.TestCase):
    def setUp(self):
        self.c = Configuration()
    def test_rightSetting(self):
        """Verify the class works properly for a right couple of name and value."""
        self.c.cycles = 100
        self.assertEqual(getattr(self.c, 'cycles'), 100)
    def test_wrongValue(self):
        """Verify a TypeError is raised when the attribute value is not of the right type."""
        self.assertRaises(TypeError, setattr, self.c, 'cycles', 10.2)
        self.c.lower_freq = 100.0
        self.assertRaises(AttributeError, setattr, self.c, 'upper_freq', 50.0)
        self.assertRaises(AttributeError, setattr, self.c, 'observer_name', 'STR')
    def test_checks(self):
        """Verify an AttributeError is raised in the case the value does not passes the check."""
        self.assertRaises(AttributeError, setattr, self.c, 'cycles', 0)
        self.assertRaises(AttributeError, setattr, self.c, 'acquisition_time', 11)
    def test_wrongName(self):
        """Verify an AttributeError is raised when the attribute name is not allowed."""
        self.assertRaises(AttributeError, setattr, self.c, 'foo', '100')
    def test_delAttribute(self):
        """Verify an attribute cannot be deleted."""
        self.assertRaises(AttributeError, delattr, self.c, 'stats')

if __name__ == '__main__':
    unittest.main()
