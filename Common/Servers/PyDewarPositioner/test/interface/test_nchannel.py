import threading
import time
import unittest
import mocker

import Receivers
from Management import MNG_WARNING, MNG_FAILURE, MNG_OK
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Nc.Consumer import Consumer
from DewarPositioner import DewarPositionerImpl


class NotificationChannelTest(unittest.TestCase):

    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.consumer= Consumer(Receivers.DEWAR_POSITIONER_DATA_CHANNEL)
    def tearDown(self):
        if self.positioner.isConfigured():
            self.positioner.park()

    def test_notconfigured(self):
        """Test the channel when the positioner is not yet configured"""
        def dataHandler(data):
            self.assertEqual(data.ready, False)
            self.assertEqual(data.tracking, False)
            self.assertEqual(data.updating, False)
            self.assertEqual(data.slewing, False)
            self.assertEqual(data.status, MNG_OK)

        time.sleep(0.5)
        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                dataHandler
        )
        self.consumer.consumerReady()
        time.sleep(2)
        self.consumer.disconnect()

    def test_configured(self):
        """Test the channel when the positioner is configured"""
        self.positioner.setup('KKG')
        def dataHandler(data):
            self.assertEqual(data.ready, True)
            self.assertEqual(data.tracking, True)
            self.assertEqual(data.updating, False)
            self.assertEqual(data.slewing, False)
            self.assertEqual(data.status, MNG_OK)

        time.sleep(1)
        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                dataHandler
        )
        self.consumer.consumerReady()
        time.sleep(1)
        self.consumer.disconnect()


    def test_updating(self):
        """Test the channel when the positioner is updating"""
        self.positioner.setup('KKG')
        def dataHandler(data):
            self.assertEqual(data.ready, True)
            self.assertEqual(data.tracking, True)
            self.assertEqual(data.updating, True)
            self.assertEqual(data.slewing, False)
            self.assertEqual(data.status, MNG_OK)

        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                dataHandler
        )
        self.consumer.consumerReady()
        time.sleep(0.1)
        self.consumer.disconnect()



if __name__ == '__main__':
    unittest.main()
