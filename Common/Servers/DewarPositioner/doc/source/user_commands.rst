.. _commands:

*******
Comandi
*******
Elenco dei comandi accettati da ``DewarPositioner.command()``,
ovvero l'elenco dei comandi che possono essere dati da
*operator input*::

    derotatorSetup=CODE -> DewarPositioner.setup(CODE)
    derotatorPark -> DewarPositioner.park()
    derotatorSetConfiguration=CODE -> DewarPositioner.setConfiguration(CODE)
    derotatorGetConfiguration -> DewarPositioner.getConfiguration()
    derotatorIsConfigured -> DewarPositioner.isConfigured()
    derotatorIsUpdating -> DewarPositioner.isUpdating()
    derotatorRewind=FEEDS -> DewarPositioner.rewind(FEEDS)
    derotatorStartUpdating -> DewarPositioner.startUpdating()
    derotatorStopUpdating -> DewarPositioner.stopUpdating()
    derotatorSetRewindingMode=MODE -> DewarPositioner.setRewindingMode(MODE)
    ... # TODO

Come possiamo vedere, iniziano tutti con ``derotator``.
