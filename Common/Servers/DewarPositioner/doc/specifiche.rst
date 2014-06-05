.. toctree::
   :maxdepth: 2

Setup del posizionatore
=======================
Il metodo ``setup()`` viene chiamato passandogli un argomento ``CODE`` 
di tipo stringa. Ottiene il riferimento al derotatore associato a ``CODE``,
e opera su di esso basandosi sull'interfaccia del ``GenericDerotator``.
Ad esempio, ``setup('KKG')`` ottiene il riferimento al component 
``KBandDerotator``.

Il setup imposta i valori di default::

    setOffset(0)
    setRewindingMode(AUTO)


Impostare l'offset
==================
Il metodo ``setOffset(OFFSET)`` serve a definire la geometria iniziale, 
e si riferisce al frame AZ/EL, per cui ``OFFSET=0`` (gradi) corrisponde 
alla geometria in cui i tre feed 1, 0, 4 sono allineati e perpedicolari 
all'asse di elevazione (**asse di elevazione o z**?). Durante il ``setup()`` 
viene impostato il valore di default ``OFFSET=0``.


Impostare la modalita' di tracking
==================================
Il metodo ``setTrackingMode(MODE)`` abilita il tracking del derotatore (lo
abilita, non avvia la movimentazione). 
``MODE`` puo' essere ``FIXED`` o ``OPTIMIZED``.

Il metodo ``getTrackingMode()`` restituisce la modalita' di tracking
impostata.


Modalita' FIXED
---------------
Se chiamiamo ``D(t)`` la derotazione in funzione dell'angolo parallattico, 
allora in questa configurazione la posizione del derotatore e' data da::

    Pf(t) = OFFSET + D(t)    (1)

Modalita' OPTIMIZED
-------------------
Il ``DewarPositioner`` calcola la posizione iniziale ``K`` del derotatore 
per ottimizzare la copertura spaziale del multifeed in base all'*asse* di 
scansione, alla *direzione* di scansione e alla posizione (*settore*) di 
azimuth. La posizione da comandare sara' quindi::

    Po(t) = OFFSET + K + D(t) = K + Pf(t)    (2)

Cambio modalita'
----------------
Quando si vuole cambiare modalita' di tracking bisogna
fermare il tracking e riavviarlo::

    setTrackingMode('OPTIMIZED')
    startTracking() # Avvia il tracking AUTO

    # Se ora voglio cambiare modalita' di tracking:
    stopTracking()
    setTrackingMode('FIXED')
    startTracking()


Avviare il tracking
===================
Il tracking viene avviato con il metodo ``startTracking()``. Questo viene
chiamato passandogli i parametri della scansione, ovvero l'asse, la direzione
e il settore di azimuth, indipendentemente dal fatto che la modalita'
impostata sia ``FIXED`` o ``OPTIMIZED``::

    startTracking(axis, direction, azimuth_section)

La posizione del derotatore verra' aggiornata con l'equazione (1) se 
la modalita' di tracking e' ``FIXED``, con la (2) se e' ``OPTIMIZED``.

Se il sistema e' gia' in tracking e viene chiamato ``startTracking()``,
il posizionamento avverra' calcolando K (in caso di ``OPTIMIZED``)
sulla base dei nuovi argomenti.


Interrompere il tracking
========================
Il metodo ``stopTracking()`` interrompe il tracking.

Riavvolgimento
==============
Se ``STEP`` e' l'angolo tra due feed (60 gradi per il banda K) e supponiamo 
che all'istante ``t`` il derotatore sia in posizione di fine corsa 
``FinalLimit``, allora all'istante ``t+1`` la nuova posizione comandata sara'::

    P(t+1) = FinalLimit + D(t) - NUMBER_OF_FEED*STEP (step=60 gradi per il KBand). 

Il tipo di riavvolgimento lo si imposta tramite il metodo 
``setRewindingMode(MODE)``. L'argomento ``MODE`` puo' essere ``AUTO`` (default) 
o ``MANUAL``.

Il metodo ``setAutoRewinding(NUMBER_OF_FEEDS)`` imposta il riavvolgimento
automatico pari al numero di feed passato come argomento. La chiamata a questo
metodo imposta anche la modalita' ``AUTO``, qualora non fosse gia' impostata.

In caso di modalita' ``MANUAL``, quando si arriva a fine corsa si ha::

    isTracking() #-> false
    isRewindingRequired() #-> true
    # Messaggio di WARNING a log


Il derotatore restera' fermo in attesa che venga chiamato il metodo 
``rewind()``::

    rewind(NUMBER_OF_FEEDS)

Durante il riavvolgimento::

    isRewinding() #-> true
    isTracking() #-> false

Stima del tempo residuo di derotazione
======================================
Il metodo ``getRemainingTime()`` restituisce una stima del tempo residuo
di derotazione.

Parcheggio del derotatore
=========================
Il metodo ``park()`` disabilita il derotatore *resettando* la modalita'
di tracking, cancellando gli offset, la configurazione attuale e comandata,
la modalita' di riavvolgimento ecc. Infine posiziona il derotatore in 
parcheggio.

Avvio del tracking
==================
Il metodo ``startTracking()`` avvia il tracking del derotatore sulla base della
modalita' selezionata. Se non e' stata ancora selezionata la modalita' di tracking,
solleva una eccezione di tipo NotAllowedExImpl.

Elenco completo dei metodi
==========================
Di seguito l'elenco completo dei metodi:: 

    # setup(CODE)
    # getActalSetup()
    # getCommandedSetup()
    # isReady()
    # isConfigured()
    # park()

    # setOffset(OFFSET)
    # clearOffset()
    # getOffset()

    # getPosition()

    # setTrackingMode(MODE) # MODE: FIXED or OPTIMIZED
    # getTrackingMode()
    startTracking(AXIS, DIRECTION, AZ_SECTION)
    stopTracking()
    # isTracking()
    # isSlewing()

    # setRewindingMode(MODE) # MODE: MANUAL or AUTO (default)
    setAutoRewinding(NUMBER_OF_FEEDS)
    # getRewindingMode()
    rewind(NUMBER_OF_FEEDS)
    isRewindingRequired()
    isRewinding()

    getRemainingTime()


Esempio di utilizzo 
===================
Supponiamo di voler usare il banda K con derotatore in modalita' di rewind 
``AUTO`` (default) e modalita' di tracking ``OPTIMIZED``. I metodi del 
``DewarPositioner`` da chiamare risultano::

    setup('KKG')
    setTrackingMode(OPTIMIZED)
    startTracking(axis, direction, azimuth_section)


Il metodo ``DewarPositioner.setup('KKG')`` viene chiamato dal setup globale::

    setupKKG #-> DewarPositioner.setup('KKG')

Il metodo ``setTrackingMode()`` viene chiamato quando l'astronomo da operator
input da il comando (ad esempio) ``setDerotatorTrackingMode``::

    setDerotatorTrackingMode=OPT #-> DewarPositioner.setTrackingMode(OPTIMIZED)

Il metodo ``startTracking()`` verra' chiamato invece dallo scheduler. Quindi
in definitiva, per utilizzare la modalita' oggetto di questo esempio, da
operator input::

    setupKKG
    setDerotatorTrackingMode=OPT

