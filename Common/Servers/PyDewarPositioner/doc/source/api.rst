.. _api:

**************************
Elenco completo dei metodi
**************************
Esempio di riferimento a :meth:`positioner.Positioner.getStatus()`. 
Di seguito l'elenco completo dei metodi:: 

    setup(CODE) # CODE: KKG, ...
    getActualSetup()
    getCommandedSetup()
    isReady()
    isConfigured()
    park()
    setPosition(POSITION)
    getPosition()
    setConfiguration(CODE) # CODE: FIXED, BSC, ...
    clearConfiguration()
    getConfiguration()
    startUpdating(AXIS, AZ_SECTION)
    stopUpdating()
    isTracking()
    isUpdating()
    isSlewing()
    setRewindingMode(MODE) # MODE: MANUAL or AUTO (default)
    clearRewindingMode()
    setAutoRewindingFeeds(NUMBER_OF_FEEDS)
    getRewindingMode()
    rewind(NUMBER_OF_FEEDS)
    isRewindingRequired()
    isRewinding()
    getStatus() # Restituisce lo status pubblicato dal NC
    ... # TODO

.. autoclass:: positioner.Positioner
    :members:
    :undoc-members:
