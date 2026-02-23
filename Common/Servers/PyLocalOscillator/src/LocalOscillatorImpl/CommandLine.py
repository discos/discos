import socket
import ReceiversErrorsImpl

FREQCMD = "FREQ "
POWERCMD = "POWER "
QUERYERROR="SYST:ERR? \n"
QUERYFREQ = "FREQ?;" + QUERYERROR
QUERYPOWER = "POW?\n"
RFONCMD = "OUTP:STAT ON\n"
RFOFFCMD = "OUTP:STAT OFF\n"
QUERYRF = "OUTP:STAT?\n"
FREQUNIT = " MHZ\n"
POWERUNIT = " dBM\n"


class CommandLine:

    def __init__(self, ip, port):
        self.sock = None
        self.ip = ip
        self.port = port
        self._rf_on = True
        self.connected = False

    def __del__(self):
        self.close()

    def _connect(self):
        if self.sock is not None:
            return True
        if self.ip is None or self.port is None:
            return False
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.settimeout(2.0)
            self.sock.connect((self.ip, self.port))
            self._raw_send('*CLS\n')
            if self._rf_on:
                self._raw_send(RFONCMD)
            else:
                self._raw_send(RFOFFCMD)
            self.connected = True
            return True
        except socket.error as msg:
            if self.sock:
                self.sock.close()
            self.sock = None
            self.connected = False
            return False

    def _raw_send(self, msg):
        self.sock.sendall(msg.encode())

    def sendCmd(self, cmd):
        if not self._connect():
            exc = ReceiversErrorsImpl.SynthetiserErrorExImpl()
            exc.setDetails("Socket not connected")
            raise exc
        try:
            self._raw_send(cmd)
            return True
        except socket.error as ex:
            self.sock.close()
            self.sock = None
            self.connected = False
            exc = ReceiversErrorsImpl.SynthetiserErrorExImpl(exception=ex)
            exc.setDetails(f"Socket error while sending command: {cmd}")
            raise exc

    def query(self, cmd):
        if not self._connect():
            exc = ReceiversErrorsImpl.SynthetiserErrorExImpl()
            exc.setDetails("Socket not connected")
            raise exc
        try:
            self._raw_send(cmd)
            msg = self.sock.recv(1024).decode()
            return msg
        except socket.error as ex:
            self.sock.close()
            self.sock = None
            self.connected = False
            exc = ReceiversErrorsImpl.SynthetiserErrorExImpl(exception=ex)
            exc.setDetails(f"Socket error while querying: {cmd}")
            raise exc

    def setPower(self,power):
        err = self.sendCmd(POWERCMD + str(power) + POWERUNIT)
        msg = self.query(QUERYERROR)
        return msg, err

    def getPower(self):
        msg = self.query("POWER?;SYST:ERR?\n")
        try:
            commands = msg.split(';')
            err = commands[1]
            if err != '0,\"No error\"\n':
                exc = ReceiversErrorsImpl.SynthetiserErrorExImpl()
                exc.setDetails(f"Hardware error: {err}")
                raise exc
            val = float(commands[0])
            return err, val
        except (ValueError, IndexError) as ex:
            exc = ReceiversErrorsImpl.SynthetiserErrorExImpl(exception=ex)
            exc.setDetails(f"Malformed response: {msg}")
            raise exc

    def setFrequency(self,freq):
        err = self.sendCmd(FREQCMD + str(freq) + FREQUNIT)
        msg = self.query(QUERYERROR)
        return msg, err

    def getFrequency(self):
        msg = self.query(QUERYFREQ)
        try:
            commands = msg.split(';')
            err = commands[1]
            if err != '0,\"No error\"\n':
                exc = ReceiversErrorsImpl.SynthetiserErrorExImpl()
                exc.setDetails(err)
                raise exc
            val = float(commands[0]) / 1e6
            return err, val
        except (ValueError, IndexError) as ex:
            exc = ReceiversErrorsImpl.SynthetiserErrorExImpl(exception=ex)
            exc.setDetails(f"Malformed response: {msg}")
            raise exc

    def readStatus(self):
        msg = self.query(QUERYERROR)
        if msg != '0,\"No error\"\n':
            exc = ReceiversErrorsImpl.SynthetiserErrorExImpl()
            exc.setDetails(f"Hardware error: {msg}")
            raise exc
        return msg

    def rfOn(self):
        self._rf_on = True
        err = self.sendCmd(RFONCMD)
        msg = self.query(QUERYERROR)
        return msg, err

    def rfOff(self):
        self._rf_on = False
        err = self.sendCmd(RFOFFCMD)
        msg = self.query(QUERYERROR)
        return msg, err

    def isRfOn(self):
        try:
            return bool(int(self.query(QUERYRF).strip()))
        except ValueError as ex:
            exc = ReceiversErrorsImpl.SynthetiserErrorExImpl(exception=ex)
            exc.setDetails("Malformed response for RF state")
            raise exc

    def close(self):
        if self.sock:
            self.sock.close()
            self.sock = None
            self.connected = False
