import unittest
from socket import *       

class ExternalClientTest(unittest.TestCase):

    sockobj = socket(AF_INET, SOCK_STREAM)
    sockobj.settimeout(1)
    sockobj.connect(('192.168.200.200', 30000))

    def test_right_request(self):
        """Verify it works in case of right request"""
        self.sockobj.sendall('antennaParameters')
        answer = self._get_answers(n=1)
        self.assertRegexpMatches(answer, 'antennaParameters')

    def test_speed_dependency(self):
        """Verify it works in case of clients too fast"""
        # Send to commands, simulating a client too fast
        self.sockobj.sendall('antennaParameters'*2)
        answer = self._get_answers(n=1)
        self.assertRegexpMatches(answer, 'antennaParameters')

    def test_do_not_lose_answers(self):
        """Verify it does not lose any answer"""
        # Send to commands, simulating a client too fast
        self.sockobj.sendall('antennaParameters'*2)
        # We do not want to lose the answer related to the second request
        answers = self._get_answers(n=2) 
        self.assertEqual(answers.count('antennaParameters/'), 2)

    def _get_answers(self, n=1):
        """Return the answers.
        
        The parameter `n` is the number of answers to return.
        """
        answers = ""
        try:
            for i in range(n):
               while(True): 
                   answers += self.sockobj.recv(1)
                   if '\n' in answers:
                       break
        except socket.timeout, ex:
            print 'Socket timeout: %s' %ex.message
        finally:
            return answers

if __name__ == '__main__':
    unittest.main()
