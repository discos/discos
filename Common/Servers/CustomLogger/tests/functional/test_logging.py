# -*- coding: utf-8 -*-

from __future__ import with_statement
from threading import Thread
import os
import time
import subprocess
import signal
import sys

import unittest

from Acspy.Clients.SimpleClient import PySimpleClient
import Management
from IRAPy import logger

COMPONENT_NAME = "MANAGEMENT/CustomLogger"
SLEEP_TIME=1.0

class LoggingThread(Thread):
    def __init__(self, group=None, target=None, name=None, verbose=None):
        Thread.__init__(self)
        self._stop_now = False

    def run(self):
        i = 0
        while not self._stop_now:
            logger.logNotice("logging thread: %i" % (i,))
            i += 1

    def stop(self):
        self._stop_now = True


class CustomLoggerTests(unittest.TestCase):
    def setUp(self):
        self.client = PySimpleClient()
        self.custom_logger = self.client.getComponent(COMPONENT_NAME)

    def tearDown(self):
        self.client.releaseComponent(COMPONENT_NAME)
        self.client.disconnect()


class LogFileTests(CustomLoggerTests):
    def test_set_log_file(self):
        basepath = "/tmp/events"
        filename = "test_set.log"
        file_path = os.path.join(basepath, filename)
        self.custom_logger.setLogfile(basepath, filename)
        self.assertTrue(os.path.exists(file_path))
        self.assertEqual(self.custom_logger._get_filename().get_sync()[0], 
                         file_path)
        self.assertEqual(self.custom_logger._get_isLogging().get_sync()[0],
                         Management.MNG_TRUE)
        self.custom_logger.closeLogfile()
        os.remove(file_path)

    def test_close_log_file(self):
        basepath = "/tmp/events"
        filename = "test_close.log"
        file_path = os.path.join(basepath, filename)
        self.custom_logger.setLogfile(basepath, filename)
        self.custom_logger.closeLogfile()
        self.assertTrue(os.path.exists(file_path))
        self.assertEqual(self.custom_logger._get_filename().get_sync()[0], 
                         file_path)
        self.assertEqual(self.custom_logger._get_isLogging().get_sync()[0],
                         Management.MNG_FALSE)
        os.remove(file_path)

    def test_close_log_file_two_times(self):
        basepath = "/tmp/events"
        filename = "test_close.log"
        file_path = os.path.join(basepath, filename)
        self.custom_logger.setLogfile(basepath, filename)
        self.custom_logger.closeLogfile()
        try:
            time.sleep(SLEEP_TIME)
            self.custom_logger.closeLogfile()
            time.sleep(SLEEP_TIME)
            self.custom_logger.closeLogfile()
        except Exception, e:
            os.remove(file_path)
            self.fail("Exception raised closing logfile: %s" % (e,))
        finally:
            os.remove(file_path)

    def test_close_file_during_logging_thread(self):
        basepath = "/tmp/events"
        filename = "test_close_thread.log"
        file_path = os.path.join(basepath, filename)
        self.custom_logger.setLogfile(basepath, filename)
        #start a thread for continuous messaging
        logging_thread = LoggingThread()
        logging_thread.start()
        time.sleep(SLEEP_TIME)
        self.custom_logger.flush()
        try:
            self.custom_logger.closeLogfile()
        except Exception, e:
            logging_thread.stop()
            logging_thread.join()
            self.fail("Exception raised closing logfile: %s" % (e,))
        logging_thread.stop()
        logging_thread.join()
        self.assertTrue(os.path.exists(file_path))
        self.assertEqual(self.custom_logger._get_filename().get_sync()[0], 
                         file_path)
        self.assertEqual(self.custom_logger._get_isLogging().get_sync()[0],
                         Management.MNG_FALSE)
        os.remove(file_path)

    def test_logging_with_closed_logfile(self):
        basepath = "/tmp/events"
        filename = "test_logging_with_closed_logfile.log"
        file_path = os.path.join(basepath, filename)
        self.custom_logger.setLogfile(basepath, filename)
        self.custom_logger.closeLogfile()
        #start a thread for continuous messaging
        logging_thread = LoggingThread()
        logging_thread.start()
        time.sleep(SLEEP_TIME)
        self.custom_logger.flush()
        logging_thread.stop()
        logging_thread.join()
        self.assertTrue(os.path.exists(file_path))
        self.assertEqual(self.custom_logger._get_isLogging().get_sync()[0],
                         Management.MNG_FALSE)
        os.remove(file_path)

    def test_set_file_during_logging_thread(self):
        basepath_first = "/tmp/events"
        filename_first = "test_set_first.log"
        file_path_first = os.path.join(basepath_first, filename_first)
        basepath_second = "/tmp/events"
        filename_second = "test_set_second.log"
        file_path_second = os.path.join(basepath_second, filename_second)
        self.custom_logger.setLogfile(basepath_first, filename_first)
        #start a thread for continuous messaging
        logging_thread = LoggingThread()
        logging_thread.start()
        time.sleep(SLEEP_TIME)
        #self.custom_logger.flush()
        try:
            self.custom_logger.setLogfile(basepath_second, filename_second)
        except Exception, e:
            logging_thread.stop()
            logging_thread.join()
            self.fail("Exception raised setting logfile: %s" % (e,))
        time.sleep(SLEEP_TIME)
        self.custom_logger.closeLogfile()
        logging_thread.stop()
        logging_thread.join()
        #grab the index number from log events in separate files
        with open(file_path_first, "rt") as first_file:
            first_index = int(first_file.readlines()[-3].strip().split()[-1])
        with open(file_path_second, "rt") as second_file:
            second_index = int(second_file.readline().strip().split()[-1])
        self.assertEqual(first_index + 1, second_index)
        os.remove(file_path_first)
        os.remove(file_path_second)

    def test_close_file_during_logging_process(self):
        basepath = "/tmp/events"
        filename = "test_close_process.log"
        file_path = os.path.join(basepath, filename)
        self.custom_logger.setLogfile(basepath, filename)
        #start a process for continuous messaging
        logging_process = subprocess.Popen([sys.executable, "./functional/logging_process.py"])
        time.sleep(SLEEP_TIME)
        self.custom_logger.flush()
        try:
            self.custom_logger.closeLogfile()
        except Exception, e:
            os.kill(logging_process.pid, signal.SIGKILL)
            self.fail("Exception raised closing logfile: %s" % (e,))
        os.kill(logging_process.pid, signal.SIGKILL)
        self.assertTrue(os.path.exists(file_path))
        self.assertEqual(self.custom_logger._get_filename().get_sync()[0], 
                         file_path)
        self.assertEqual(self.custom_logger._get_isLogging().get_sync()[0],
                         Management.MNG_FALSE)
        os.remove(file_path)

    def test_set_file_during_logging_thread(self):
        basepath_first = "/tmp/events"
        filename_first = "test_set_first_process.log"
        file_path_first = os.path.join(basepath_first, filename_first)
        basepath_second = "/tmp/events"
        filename_second = "test_set_second_process.log"
        file_path_second = os.path.join(basepath_second, filename_second)
        self.custom_logger.setLogfile(basepath_first, filename_first)
        #start a thread for continuous messaging
        logging_process = subprocess.Popen([sys.executable, "./functional/logging_process.py"])
        time.sleep(SLEEP_TIME)
        #self.custom_logger.flush()
        try:
            self.custom_logger.setLogfile(basepath_second, filename_second)
        except Exception, e:
            os.kill(logging_process.pid, signal.SIGKILL)
            self.fail("Exception raised setting logfile: %s" % (e,))
        time.sleep(SLEEP_TIME)
        self.custom_logger.closeLogfile()
        os.kill(logging_process.pid, signal.SIGKILL)
        #grab the index number from log events in separate files
        with open(file_path_first, "rt") as first_file:
            first_index = int(first_file.readlines()[-3].strip().split()[-1])
        with open(file_path_second, "rt") as second_file:
            second_index = int(second_file.readline().strip().split()[-1])
        self.assertEqual(first_index + 1, second_index)
        os.remove(file_path_first)
        os.remove(file_path_second)


class LogMethodsTests(CustomLoggerTests):
    def setUp(self):
        CustomLoggerTests.setUp(self)
        self.base_path = "/tmp/events"
        self.filename = "log_methods_tests.log"
        self.custom_logger.setLogfile(self.base_path, self.filename)

    def tearDown(self):
        self.custom_logger.closeLogfile()
        CustomLoggerTests.tearDown(self)
        os.remove(os.path.join(self.base_path, self.filename))

    def test_non_ascii_log(self):
        message = "#èé§°çç{}ò@à#ùßafdæßðđ»¢æßðæ€¶ł"
        logger.logNotice(message)
        time.sleep(SLEEP_TIME)
        self.custom_logger.flush()
        self.assertEqual(self.custom_logger._get_isLogging().get_sync()[0],
                         Management.MNG_TRUE)
        self.custom_logger.closeLogfile()
        with open(os.path.join(self.base_path, self.filename), "rt") as logfile:
            last_log = logfile.readlines()[-1].strip()
            self.assertTrue(last_log.endswith("??????????????????????????"), 
                            msg = "last log: %s" % (last_log,))

    def test_square_brackets_log(self):
        message = "ciao [Marco]"
        logger.logNotice(message)
        time.sleep(SLEEP_TIME)
        self.custom_logger.flush()
        self.assertEqual(self.custom_logger._get_isLogging().get_sync()[0],
                         Management.MNG_TRUE)
        self.custom_logger.closeLogfile()
        with open(os.path.join(self.base_path, self.filename), "rt") as logfile:
            last_log = logfile.readlines()[-1].strip()
            self.assertTrue(last_log.endswith("ciao {Marco}"), 
                            msg = "last log: %s" % (last_log,))
    
