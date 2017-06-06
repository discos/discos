import threading
import time
import unittest
import mocker
import Receivers
from Management import MNG_WARNING, MNG_FAILURE, MNG_OK
from Acspy.Nc.Consumer import Consumer
from Acspy.Nc.Supplier import Supplier
from DewarPositioner import DewarPositionerImpl


class PublisherTest(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.supplier = Supplier(Receivers.DEWAR_POSITIONER_DATA_CHANNEL)

    @classmethod
    def tearDownClass(cls):
        cls.supplier.disconnect()

    def setUp(self):
        self.m = mocker.Mocker()
        self.positioner = self.m.mock()
        self.consumer= Consumer(Receivers.DEWAR_POSITIONER_DATA_CHANNEL)

    def tearDown(self):
        self.m.restore()
        self.m.verify()

    def test_ready(self):
        """Test the supplier notifies properly the ready flag"""
        self.positioner.getStatus()
        self.m.result('000001')
        self.m.count(1, None)
        self.positioner.isSetup()
        self.m.result(True)
        self.m.count(0, None)
        self.m.replay()
        control = DewarPositionerImpl.Control()
        thread = threading.Thread(
                name='Publisher',
                target=DewarPositionerImpl.DewarPositionerImpl.publisher,
                args=(self.positioner, self.supplier, control)
        )
        thread.start()

        time.sleep(0.1)
        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                lambda data: self.assertEqual(data.ready, True)
        )
        self.consumer.consumerReady()
        time.sleep(1)
        control.stop = True
        self.consumer.disconnect()

    def test_tracking(self):
        """Test the supplier notifies properly the tracking flag"""
        self.positioner.getStatus()
        self.m.result('000011')
        self.m.count(1, None)
        self.positioner.isSetup()
        self.m.result(True)
        self.m.count(0, None)
        self.m.replay()
        control = DewarPositionerImpl.Control()
        thread = threading.Thread(
                name='Publisher',
                target=DewarPositionerImpl.DewarPositionerImpl.publisher,
                args=(self.positioner, self.supplier, control)
        )
        thread.start()

        time.sleep(0.1)
        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                lambda data: self.assertEqual(data.tracking, True)
        )
        self.consumer.consumerReady()
        time.sleep(1)
        control.stop = True
        self.consumer.disconnect()

    def test_status_warning(self):
        """Test the supplier notifies properly the warning status flag"""
        self.positioner.getStatus()
        self.m.result('010000')
        self.m.count(1, None)
        self.positioner.isSetup()
        self.m.result(True)
        self.m.count(0, None)
        self.m.replay()
        control = DewarPositionerImpl.Control()
        thread = threading.Thread(
                name='Publisher',
                target=DewarPositionerImpl.DewarPositionerImpl.publisher,
                args=(self.positioner, self.supplier, control)
        )
        thread.start()

        time.sleep(0.1)
        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                lambda data: self.assertEqual(data.status, MNG_WARNING)
        )
        self.consumer.consumerReady()
        time.sleep(1)
        control.stop = True
        self.consumer.disconnect()

    def test_status_failure(self):
        """Test the supplier notifies properly the failure status flag"""
        self.positioner.getStatus()
        self.m.result('110000')
        self.m.count(1, None)
        self.positioner.isSetup()
        self.m.result(True)
        self.m.count(0, None)
        self.m.replay()
        control = DewarPositionerImpl.Control()
        thread = threading.Thread(
                name='Publisher',
                target=DewarPositionerImpl.DewarPositionerImpl.publisher,
                args=(self.positioner, self.supplier, control)
        )
        thread.start()

        time.sleep(0.1)
        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                lambda data: self.assertEqual(data.status, MNG_FAILURE)
        )
        self.consumer.consumerReady()
        time.sleep(1)
        control.stop = True
        self.consumer.disconnect()

    def test_status_ok(self):
        """Test the supplier notifies properly the OK status flag"""
        self.positioner.getStatus()
        self.m.result('000001')
        self.m.count(1, None)
        self.positioner.isSetup()
        self.m.result(True)
        self.m.count(0, None)
        self.m.replay()
        control = DewarPositionerImpl.Control()
        thread = threading.Thread(
                name='Publisher',
                target=DewarPositionerImpl.DewarPositionerImpl.publisher,
                args=(self.positioner, self.supplier, control)
        )
        thread.start()

        time.sleep(0.1)
        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                lambda data: self.assertEqual(data.status, MNG_OK)
        )
        self.consumer.consumerReady()
        time.sleep(1)
        control.stop = True
        self.consumer.disconnect()

    def test_all_active(self):
        """The supplier notifies properly when all the device status bit are high"""
        self.positioner.getStatus()
        self.m.result('111111')
        self.m.count(1, None)
        self.positioner.isSetup()
        self.m.result(True)
        self.m.count(0, None)
        self.m.replay()
        control = DewarPositionerImpl.Control()
        thread = threading.Thread(
                name='Publisher',
                target=DewarPositionerImpl.DewarPositionerImpl.publisher,
                args=(self.positioner, self.supplier, control)
        )
        thread.start()

        def dataHandler(data):
            self.assertEqual(data.ready, True)
            self.assertEqual(data.tracking, True)
            self.assertEqual(data.updating, True)
            self.assertEqual(data.slewing, True)
            self.assertEqual(data.status, MNG_FAILURE)

        time.sleep(0.1)
        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                dataHandler
        )
        self.consumer.consumerReady()
        time.sleep(1)
        control.stop = True
        self.consumer.disconnect()

    def test_all_inactive(self):
        """The supplier notifies properly when all the device status bit are low"""
        self.positioner.getStatus()
        self.m.result('000000')
        self.m.count(1, None)
        self.positioner.isSetup()
        self.m.result(True)
        self.m.count(0, None)
        self.m.replay()
        control = DewarPositionerImpl.Control()
        thread = threading.Thread(
                name='Publisher',
                target=DewarPositionerImpl.DewarPositionerImpl.publisher,
                args=(self.positioner, self.supplier, control)
        )
        thread.start()

        def dataHandler(data):
            self.assertEqual(data.ready, False)
            self.assertEqual(data.tracking, False)
            self.assertEqual(data.updating, False)
            self.assertEqual(data.slewing, False)
            self.assertEqual(data.status, MNG_OK)

        time.sleep(0.1)
        self.consumer.addSubscription(
                Receivers.DewarPositionerDataBlock, 
                dataHandler
        )
        self.consumer.consumerReady()
        time.sleep(1)
        control.stop = True
        self.consumer.disconnect()


if __name__ == '__main__':
    unittest.main()
