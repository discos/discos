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

Dopo il setup() i tre feed 1, 0, 4 sono allineati e paralleli
all'orizzonte. 

Impostare l'offset
==================
Il metodo ``setOffset(OFFSET)`` imposta un offset che viene sommato
alle posizioni da comandare al derotatore.
La chiamata al metodo ``setOffset()`` ha effetto immediato, nel senso 
che la nuova posizione viene immediatamente comandata::

    >>> DewarPositioner.getPosition()
    10
    >>> DewarPositioner.seOffset(3)
    >>> DewarPositioner.getPosition()
    13
    >>> DewarPositioner.getOffset()
    3

Durante il ``setup()`` viene impostato il valore di default ``OFFSET=0``.


Impostare la modalita' di updating
==================================
Il metodo ``setUpdatingMode(MODE)`` abilita l'aggiornamento della posizione
del derotatore (lo abilita, non avvia la movimentazione) e imposta il tipo
di aggiornamento, assegnato a ``MODE``, che puo' essere:

   1. ``FIXED``: il derotatore non aggiorna la sua posizione in funzione
      dell'angolo parallatico
   2. ``SIMPLE``: il derotatore aggiorna la sua posizione in funzione 
      dell'angolo parallatico; all'inizio di ogni scan il derotatore viene
      posizionato in modo da ottimizzare la copertura spaziale 
      del multifeed lungo l'asse di scansione
   3. ``OPTIMIZED``: e' analoga alla ``SIMPLE``, ma la posizione del
      derotatore all'inizio di ogni scan e' ottimizzata anche per ottenere
      la massima durata dello scan prima che si renda necessario riavvolgere

Il metodo ``getUpdatingMode()`` restituisce la modalita' di aggiornamento
impostata, mentre il metodo ``isConfiguredForUpdating()`` restituisce ``True``
se e' stata impostata la modalita' di aggiornamento. Se la modalita' di
aggiornamento non e' impostata, allora ``getUpdatingMode()`` restituisce una 
stringa vuota. Quindi, riassumendo::

    >>> getUpdatingMode() # Restituisce una stringa vuota
    '' 
    >>> setup('KKG')
    >>> getUpdatingMode() # Restituisce ancora una stringa vuota
    ''
    >>> isConfiguredForUpdating() # Restituisce False
    False
    >>> setUpdatingMode('OPTIMIZED')
    >>> getUpdatingMode() # Restituisce 'OPTIMIZED'
    'OPTIMIZED'
    >>> isConfiguredForUpdating() # Restituisce True
    True
    >>> clearUpdatingMode()
    >>> getUpdatingMode()
    ''
    >>> setUpdatingMode('WRONGMODE') # Modo non valido, solleva eccezione
    Traceback (most recent call last):
        ...
    ValidationErrorEx: code WRONGMODE unknown

Prima di approdondire il funzionamento delle tre modalita', vediamo di
definire alcuni termini che utilizzeremo durante la trattazione:

  * ``AXIS``: rappresenta l'asse di scansione (``Managment::TScanAxis``), e puo' 
    essere ``SIDERALE``, ``GLON``, ``GLAT``, ``AZ``, ``EL``, ``RA``, ``DEC``, 
    ``GREATCIRCLE``
  * ``SECTOR``: puo' assumere i valori ``NORD`` o ``SUD`` 
    (da definire nella interfaccia IDL)
  * ``TABLE(RECEIVER, AXIS)``: tabella che fornisce un parametro ``C``
    utilizzato per individuare la posizione iniziale del derotatore 
    prima di ogni scan; il valore di ``C`` dipende dalla geometria dei feed
    e dall'asse di scansione
  * ``STEP`` e' l'angolo compreso tra due feed adiacenti (ad esempio, 60 gradi
    per il ricevitore 22GHz di SRT). 
    
Modalita' FIXED
----------------
Il derotatore non aggiorna la posizione in funzione dell'angolo parallatico.
All'inizio di ogni scan, a seconda dell'asse di scansione viene 
ricavato il valore di ``C`` dalla tabella ``TABLE(RECEIVER, AXIS)``, 
e la posizione del derotatore durante lo scan risultera' data da::

    Pf(t) = OFFSET + C    (1)

Modalita' SIMPLE
----------------
Se chiamiamo ``D(t)`` la derotazione in funzione dell'angolo parallattico, 
allora in questa configurazione la posizione del derotatore sara' data da::

    Ps(t) = OFFSET + C + D(t) = Pf(t) + D(t)    (2)

dove ``C``, come per la modalita' ``FIXED``, viene letto da tabella.

Modalita' OPTIMIZED
-------------------
In questo caso all'inizio di ogni scan la posizione del derotatore
viene ottimizzata sia per ottenere la massima copertura spaziale del 
multifeed lungo l'asse di scansione, sia per massimizzare
la durata dello scan prima che si renda necessario riavvolgere.
In questo caso la posizione da comandare e' data da::

    Po(t) = OFFSET + C + D(t) + K = K + Ps(t)    (3)

Il parametro ``K`` e' quello che consente di ottenere una posizione iniziale
tale che si abbia la massima durata dello scan prima che sia necessario
riavvolgere. Il valore di ``K``, in modulo, e' dato da::

    abs(K) = NUMBER_OF_FEEDS*STEP

dove ``NUMBER_OF_FEEDS`` e' la nostra incognita, e rappresenta il numero di 
feed di cui dobbiamo ruotare il derotatore per garantire la massima escursione
durante lo scan. Vediamo come calcolarlo, con un esempio.
Supponiamo di utilizzare il derotatore del
22GHz di SRT, la cui posizione puo' andare da -106 a +106 gradi, e il cui ``STEP``
e' pari a 60 gradi. Dobbiamo iniziare una scansione con ``AXIS == GLON``, 
e supponiamo che da tabella si abbia ``C = 40 gradi``.
Supponiamo infine che durante la scansione il cielo ruoti CCW. 
Il derotatore ruotera' anche esso CCW, per cui dobbiamo sceglere il 
valore di ``NUMBER_OF_FEEDS`` che
garantisca la massima escursione, ovvero tale che 
``OFFSET + C + NUBER_OF_FEEDS*STEP`` sia il piu' vicino possibile al 
limite positivo +106 gradi, ma non lo superi. Se ``OFFSET = 0``
allora questo e' dato da ``NUMBER_OF_FEEDS = 1``, visto che si ha:

.. code-block:: none

    C + NUMBER_OF_FEEDS*STEPS + OFFSET == 40 + NUMBER_OF_FEEDS*60 + 0 < 106

ovvero:

.. code-block:: none
    
    NUMBER_OF_FEEDS < (106-40)/60  --> NUMBER_OF_FEEDS = 1

Quindi in questo caso ``K = NUMBER_OF_FEEDS*STEP = 60 gradi``.
Se ad esempio ``OFFSET = 35 gradi``, allora si ha ``NUMBER_OF_FEEDS = 0``,
ovvero ``K = 0`` visto che:

.. code-block:: none

    C + NUMBER_OF_FEEDS*STEPS + OFFSET == 40 + NUMBER_OF_FEEDS*60 + 35 < 106 

ovvero:

.. code-block:: none

    NUMBER_OF_FEEDS < (106-75)/60


Per quanto riguarda il segno di ``K``, questo dipende da ``SECTOR``:

  * se ``SECTOR == SUD`` il cielo ruota CW, per cui ``SGN(K) == -1``
  * se ``SECTOR == NORD`` il cielo ruota CCW per cui ``SGN(K) == +1``

Ad esempio, consideriamo l'esempio di prima, con ``C = 40 gradi``, ``OFFSET = 0``,
ma con ``SECTOR = SUD``, ovvero rotazione CW. In questo caso avremo
``NUMBER_OF_FEEDS = 2``:

.. code-block:: none

    40 -NUMBER_OF_FEEDS*60 + 0 > -106  -->  NUMBER_OF_FEEDS < 146/60 

per cui:

.. code-block:: none

    K = -NUMBER_OF_FEEDS*STEP = -2*60 = -120


Quindi, riassumendo, se ``SECTOR == SUD``::


    Po(t) = OFFSET + C + D(t) - NUMBER_OF_FEEDS*STEP = Ps(t) - NUMBER_OF_FEEDS*STEP


mentre se ``SECTOR == NORD``::

    Po(t) = OFFSET + C + D(t) + NUMBER_OF_FEEDS*STEP = Ps(t) + NUMBER_OF_FEEDS*STEP


Avviare l'aggiornamento
=======================
L'aggiornamento viene avviato con il metodo ``startUpdating()``. Questo viene
chiamato passandogli i parametri della scansione, ovvero l'asse, la direzione
e il settore di azimuth:: 

    startUpdating(AXIS, SECTION)

La posizione del derotatore verra' aggiornata con l'equazione (1) se 
la modalita' di aggiornamento e' ``FIXED``, con la (2) se e' ``SIMPLE``
e con la (3) se e' ``OPTIMIZED``.

Se non e' stata ancora selezionata la modalita' di aggiornamento, solleva 
una eccezione di tipo ``ComponentErrors::NotAllowedEx``.

Cambio modalita'
----------------
Quando si vuole cambiare modalita' di aggiornamento bisogna
fermare l'aggiornamento e riavviarlo::

    setUpdatingMode('OPTIMIZED')
    startUpdating(AXIS, SECTION) # Avvio aggiornamento (OPTIMIZED)

    # Se ora voglio cambiare modalita' di aggiornamento:
    stopUpdating() # Fermo l'aggiornamento
    setUpdatingMode('FIXED') # Cambio la modalita'
    startUpdating(AXIS, SECTION) # Avvio aggiornamento (FIXED)


Interrompere l'aggiornamento
============================
Il metodo ``stopUpdating()`` interrompe l'aggiornamento della posizione.


Riavvolgimento
==============
Se ``STEP`` e' l'angolo tra due feed (60 gradi per il banda K) e supponiamo 
che all'istante ``t`` il derotatore sia in posizione di fine corsa 
``FinalLimit``, allora all'istante ``t+1`` la nuova posizione comandata sara'::

    P(t+1) = FinalLimit + D(t+1) - NUMBER_OF_FEED*STEP 

Il tipo di riavvolgimento lo si imposta tramite il metodo 
``setRewindingMode(MODE)``. L'argomento ``MODE`` puo' essere ``AUTO`` (default) 
o ``MANUAL``.

Il metodo ``setAutoRewindingFeeds(NUMBER_OF_FEEDS)`` imposta il riavvolgimento
automatico pari al numero di feed passato come argomento. La chiamata a questo
metodo imposta anche la modalita' ``AUTO``, qualora non fosse gia' impostata.
Qualora il numero di feeds fosse superiore a quello massimo consentito per il
riavvolgimento, verra' sollevata una eccezione.

Il metodo ``clearAutoRewindingFeeds()`` chiamato in modalita' di rewinding AUTO,
fa si che il numero di feeds di rotazione del riavvolgimento automatico venga
calcolato dal sistema. In altre parole, annulla il ``setAutoRewindingFeeds``.

In caso di modalita' ``MANUAL``, quando si arriva a fine corsa si ha::

    isTracking() #-> false
    isUpdating() #-> false
    isRewindingRequired() #-> true
    # Messaggio di WARNING a log


Il derotatore restera' fermo in attesa che venga chiamato il metodo 
``rewind()``::

    rewind(NUMBER_OF_FEEDS)

Durante il riavvolgimento::

    isRewinding() #-> true
    isTracking() #-> false
    isUpdating() #-> ?

Stima del tempo residuo di derotazione
======================================
Il metodo ``getRemainingTime()`` restituisce una stima del tempo residuo
di derotazione.

Parcheggio del derotatore
=========================
Il metodo ``park()`` disabilita il derotatore *resettando* la modalita'
di aggiornamento, cancellando gli offset, la configurazione attuale e comandata,
la modalita' di riavvolgimento ecc. Infine posiziona il derotatore in 
parcheggio. Al termine del park() il sistema non sara' piu' configurato, per cui
isConfigured() restituira' False.

Recuperare la posizione ad un dato tempo
========================================
Il metodo getPositionFromHistory(t) restituisce la posizione del derotatore al
tempo t.


Elenco completo dei metodi
==========================
Di seguito l'elenco completo dei metodi:: 

    setup(CODE)
    getActalSetup()
    getCommandedSetup()
    isReady()
    isConfigured()
    isConfiguredForUpdating()
    park()
    setOffset(OFFSET)
    clearOffset()
    getOffset()
    getPosition()
    setUpdatingMode(MODE) # MODE: FIXED, SIMPLE or OPTIMIZED
    clearUpdatingMode()
    getUpdatingMode()
    startUpdating(AXIS, AZ_SECTION)
    stopUpdating()
    isTracking()
    isUpdating()
    isSlewing()
    setRewindingMode(MODE) # MODE: MANUAL or AUTO (default)
    clearRewindingMode()
    setAutoRewindingFeeds(NUMBER_OF_FEEDS)
    clearAutoRewindingFeeds()
    getRewindingMode()
    rewind(NUMBER_OF_FEEDS)
    isRewindingRequired()
    isRewinding()
    getRemainingTime()


Comandi
=======
Elenco dei comandi accettati da DewarPositioner.command()::

    derotatorSetup=CODE -> DewarPositioner.setup(CODE)
    derotatorPark -> DewarPositioner.park()
    derotatorRewind=FEEDS -> DewarPositioner.rewind(FEEDS)
    derotatorStartUpdating -> DewarPositioner.startUpdating()
    derotatorStopUpdating -> DewarPositioner.stopUpdating()
    derotatorSetOffset=OFFSET -> DewarPositioner.setOffset(OFFSET)
    derotatorClearOffset -> DewarPositioner.clearOffset()
    derotatorSetUpdatingMode=MODE -> DewarPositioner.setUpdatingMode(MODE)
    derotatorClearUpdatingMode -> DewarPositioner.clearUpdatingMode()
    derotatorSetRewindingMode=MODE -> DewarPositioner.setRewindingMode(MODE)
    

Esempio di utilizzo 
===================
Supponiamo di voler usare il banda K con derotatore in modalita' di rewind 
``AUTO`` (default) e modalita' di aggiornamento ``OPTIMIZED``. I metodi del 
``DewarPositioner`` da chiamare risultano::

    setup('KKG')
    setUpdatingMode(OPTIMIZED)
    startUpdating(axis, direction, azimuth_section)


Il metodo ``DewarPositioner.setup('KKG')`` viene chiamato dal setup globale::

    setupKKG #-> DewarPositioner.setup('KKG')

Il metodo ``setUpdatingMode()`` viene chiamato quando l'astronomo da operator
input da il comando ``derotatorSetUpdatingMode``::

    derotatorSetUpdatingMode=OPTIMIZED #-> DewarPositioner.setUpdatingMode(OPTIMIZED)

Il metodo ``startUpdating()`` verra' chiamato invece dallo scheduler. Quindi
in definitiva, per utilizzare la modalita' oggetto di questo esempio, da
operator input::

    setupKKG
    derotatorSetUpdatingMode=OPTIMIZED

