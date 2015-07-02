import os
import shutil
import subprocess

import unittest2


class TestGetTemplateForTest(unittest2.TestCase):
    """Test the getTemplateForTest command"""
    
    def setUp(self):
        self.target_dir = '__tmp__'
        if os.path.exists(self.target_dir):
            shutil.rmtree(self.target_dir)
        subprocess.call('getTemplateForTest %s' %self.target_dir, shell=True)
 
    def tearDown(self):
        if os.path.exists(self.target_dir):
            shutil.rmtree(self.target_dir)

    def test_no_svn_files(self):
        """Verify getTemplateForTest does not put .svn dirs in test directory"""
        for root, dirs, files in os.walk(self.target_dir):
            self.assertNotIn('.svn', dirs)


if __name__ == '__main__':
    unittest2.main()
