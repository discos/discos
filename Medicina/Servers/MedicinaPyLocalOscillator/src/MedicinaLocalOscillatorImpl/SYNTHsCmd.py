# SYNTHs commands string class

from MedicinaLocalOscillatorImpl import CommandLineError

class SYNTHsCmd:
    """ SYNTH HPIB command string structure
     There is a bridge converting eth to and from HPIB protocol allowing eth
     routing for instruments with HPIB comm capabilities.
     Sent cmd string are decoded by a dedicated server on subnet to effectively 
     route commands to instruments. This way is possible to talk with N differents
     instruments without a particular knowledge on routing.

     Warning: at the moment command string are put into xml conf file and there
     is no checking about their compliance with translation service ( on dedicated 
     server)
     """

    def __init__(self):
        """ init internal structs         
        """                
        self.m_keys= ['FREQ_CMD', 'FREQ_ANS_OK', \
                        'AMP_CMD', 'AMP_ANS_OK', \
                        'RF_ON_CMD', 'RF_ON_ANS_OK', \
                        'RF_OFF_CMD', 'RF_OFF_ANS_OK',\
                        'FREQ_QUERY', 'FREQ_QUERY_NOK',\
                        'AMP_QUERY', 'AMP_QUERY_NOK']
        self.m_cmd_dict= dict.fromkeys(self.m_keys)

    def getCmdList(self):
        """ Return a list with needed command names to look for inside
        xml LO conf
        """
        return self.m_keys

    def setCmd(self, p_key, p_string):
        """ Cmd string set

        p_key: string
        command mnemonic string from LO xml file

        p_string: string
        command effective string
        """
        if p_key not in self.m_cmd_dict.keys():
            l_msg= "Given key is not among SYNTHs command list"
            l_exc= CommandLineError(l_msg)
            raise l_exc
        else:
            self.m_cmd_dict[p_key]= p_string
        
    def getCmd(self, p_key):
        """
        Cmd effective string getter
        
        Return
        cmd string
        """
        try:
            return self.m_cmd_dict[p_key]
        except KeyError as e:            
            raise CommandLineError(str(e))

    def getCmdDict(self):
        """
        Getter complete dictionary :
        { 'cmd_name' : 'cmd_string' }
        """
        return self.m_cmd_dict
    
