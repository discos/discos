Set the ACS_CDB to the testing CDB::

    $ export ACS_CDB=$HOME/Nuraghe/ACS/trunk/SRT/

Start the local oscillator and SRTLPBand containers::

    $ acsStartContainer -py PyLOContainer
    $ acsStartContainer -cpp SRTSPBandContainer

Run the receiver board server (the test will do it automatically)::

    $ receiverboard-sim start

When terminated, stop the server::

    $ receiverboard-sim stop
