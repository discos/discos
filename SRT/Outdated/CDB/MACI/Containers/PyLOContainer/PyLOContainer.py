<?xml version="1.0" encoding="ISO-8859-1"?>
<Container xmlns="urn:schemas-cosylab-com:Container:1.0"
           xmlns:cdb="urn:schemas-cosylab-com:CDB:1.0" 
           xmlns:baci="urn:schemas-cosylab-com:BACI:1.0" 
           xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
           xmlns:log="urn:schemas-cosylab-com:LoggingConfig:1.0" 
           ImplLang="py"
           Timeout="30.0"
           UseIFR="true"
           ManagerRetry="10"
           Recovery="false">

    <Autoload>
        <cdb:_ string="baci" />
    </Autoload>

    <LoggingConfig 
		centralizedLogger="Log"
		minLogLevel="5"
		minLogLevelLocal="5"
		dispatchPacketSize="0"
		immediateDispatchLevel="8"
		flushPeriodSeconds="1"
	>
    </LoggingConfig>

</Container>
