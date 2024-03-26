import unittest
from random import randrange as rr
from Acspy.Util import ACSCorba
from Supervisor.SupervisorImpl import SupervisorImpl


class SupervisorCLITest(unittest.TestCase):

    def setUp(self):
        self.s = SupervisorImpl()
        self.dao = ACSCorba.cdb().get_DAO_Servant("alma/ANTENNA/Supervisor")

    def test_init_setup(self):
        success, answer = self.s.command('supervisorGetSetup')
        self.assertTrue(success)
        self.assertRegex(answer, 'DEFAULT')

    def test_default_limits(self):
        windLimit = float(self.dao.get_field_data("WindLimit"))
        self.assertEqual(windLimit, self.s.windLimit)
        sunLimit = float(self.dao.get_field_data("SunLimitDEFAULT"))
        self.assertEqual(sunLimit, self.s.sunLimit)

    def test_commanded_setup(self):
        success, answer = self.s.command("supervisorSetup=LLP")
        self.assertTrue(success)
        success, answer = self.s.command("supervisorGetSetup")
        self.assertTrue(success)
        self.assertRegex(answer, "LLP")
        windLimit = float(self.dao.get_field_data("WindLimit"))
        self.assertEqual(windLimit, self.s.windLimit)
        sunLimit = float(self.dao.get_field_data("SunLimitLLP"))
        self.assertEqual(sunLimit, self.s.sunLimit)

    def test_wrong_setup(self):
        success, answer = self.s.command("supervisorSetup=FOO")
        self.assertTrue(success)
        success, answer = self.s.command("supervisorGetSetup")
        self.assertTrue(success)
        self.assertRegex(answer, "DEFAULT")
        self.test_default_limits()

    def test_default_windAvoidance(self):
        success, answer = self.s.command("supervisorGetWindAvoidance")
        self.assertTrue(success)
        self.assertEqual(answer, "ON")

    def test_setWindAvoidance(self):
        success, answer = self.s.command("supervisorSetWindAvoidance=OFF")
        self.assertTrue(success)
        success, answer = self.s.command("supervisorGetWindAvoidance")
        self.assertEqual(answer, "OFF")

    def test_wrong_mode_setWindAvoidance(self):
        success, answer = self.s.command('supervisorSetWindAvoidance=FOO')
        self.assertFalse(success)
        self.assertRegex(answer, "mode 'FOO' not in")

    def test_default_sunAvoidance(self):
        success, answer = self.s.command("supervisorGetSunAvoidance")
        self.assertTrue(success)
        self.assertEqual(answer, "ON")

    def test_setSunAvoidance(self):
        success, answer = self.s.command("supervisorSetSunAvoidance=OFF")
        self.assertTrue(success)
        success, answer = self.s.command("supervisorGetSunAvoidance")
        self.assertEqual(answer, "OFF")

    def test_wrong_mode_setSunAvoidance(self):
        success, answer = self.s.command('supervisorSetSunAvoidance=FOO')
        self.assertFalse(success)
        self.assertRegex(answer, "mode 'FOO' not in")

    def test_setWindSafeLimit(self):
        limit = rr(100, 200)
        success, answer = self.s.command(f"supervisorSetWindSafeLimit={limit}")
        self.assertTrue(success)
        success, answer = self.s.command("supervisorGetWindSafeLimit")
        self.assertTrue(success)
        self.assertEqual(float(answer), limit)

    def test_setSunSafeLimit(self):
        limit = rr(100, 200)
        success, answer = self.s.command(f"supervisorSetSunSafeLimit={limit}")
        self.assertTrue(success)
        success, answer = self.s.command("supervisorGetSunSafeLimit")
        self.assertTrue(success)
        self.assertEqual(float(answer), limit)


if __name__ == '__main__':
    unittest.main()
