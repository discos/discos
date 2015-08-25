.. _rewinding:

**************
Riavvolgimento
**************

.. important:: Tutto da rivedere, perchè non è detto che lo step tra i feed
   sia costante (il banda S non è simmetrico). Valutare a questo punto la
   modalità auto... E' facile per l'astronomo capire quali feed hanno preso
   il posto dei precedenti? Il riavvolgimento non deve avvenire per
   numero di FEEDS, ma per disposizioni simmetriche. Questo rimette in gioco
   anche la modalità OPTIMIZED, che e' quindi da rivedere.

Nelle configurazioni dinamiche, quando il derotatore arriva a fine corsa 
tipicamente è necessario effettuare un *riavvolgimento*, che
può essere di tipo automatico (``AUTO``) oppure manuale 
(``MANUAL``).
Il tipo di riavvolgimento lo si imposta da *operator input* tramite il comando
``derotatorSetRewindingMode``::

    > derotatorSetRewindingMode=AUTO
    > derotatorSetRewindingMode=MANUAL

e per default è automatico, come indicato in :ref:`setup_actions`.
Questo comando è legato al metodo ``DewarPositioner.setRewindingMode()``, che prende
un argomento di tipo stringa::

    derotatorSetRewindingMode=AUTO --> DewarPositioner.setRewindingMode('AUTO')
 
Durante il riavvolgimento si ha::

    >>> DewarPositioner.isRewinding()
    True
    >>> DewarPositioner.isTracking()
    False
    >>> DewarPositioner.isUpdating()
    True


Riavvolgimento automatico
=========================
In caso di modalità di riavvolgimento automatica è possibile utilizzare il
comando ``derotatorSetAutoRewindingFeeds`` per impostare il riavvolgimento
automatico pari a un dato numero di feed::

    > derotatorSetRewindingMode=AUTO
    > derotatorSetAutoRewindingFeeds=2

Questo da luogo alla chiamata al metodo ``DewarPositioner.setAutoRewindingFeeds()``::

    derotatorSetAutoRewindingFeeds=2 --> DewarPositioner.setAutoRewindingFeeds(2)

Se questo metodo viene chiamato in modalità ``AUTO``, allora viene sollevata
una eccezione di tipo ``NotAllowedEx``::

    >>> DewarPositioner.setRewindingMode('MANUAL')
    >>> DewarPositioner.setAutoRewindingFeeds(2)
    Traceback (most recent call last):
    ...
    NotAllowedEx: not allowed in MANUAL rewinding mode

Se il numero di feed è negativo o superiore a quello massimo consentito per il
riavvolgimento, allora il metodo ``DewarPositioner.setAutoRewindingFeeds()`` 
solleva una eccezione::

    >>> DewarPositioner.setAutoRewindingFeeds(10)
    Traceback (most recent call last):
    ...
    NotAllowedEx: cannot rewind 10 feeds

Se in modalità ``AUTO`` non viene chiamato ``DewarPositioner.setAutoRewindingFeeds()``,
allora viene impostato un valore di *auto rewinding* di default, che dipende
dalla configurazione utilizzata, come vedremo in seguito. Per scegliere la configurazione
di default si usa un valore di *auto rewinding* pari a ``0``, per cui se
l'utente tramite *operator input* ha già impostato un valore ma poi decide di
voler utilizzare quello di default, può usare il comando::

    > derotatorSetAutoRewindingFeeds=0

Riassumendo, i comandi::

    > setRewindingMode=AUTO
    > setAutoRewindingFeeds=2

danno luogo alla chiamata ai seguenti metodi::

    >>> DewarPositioner.setRewindingMode('AUTO')
    >>> DewarPositioner.setAutoRewindingFeeds(0) # Imposta valore default
    >>> DewarPositioner.setAutoRewindingFeeds(2) # Sovrascrive il precedente

.. note:: I metodi ``setRewindingMode()`` e ``setAutoRewindingFeeds()`` possono
   essere chiamati solamente nelle configurazioni dinamiche, mentre per
   quelle statiche sollevano una eccezione di tipo ``NotAllowedEx``.

Se ``STEP`` è l'angolo tra due feed (60 gradi per il banda K) e supponiamo 
che all'istante ``t`` il derotatore sia in posizione di fine corsa 
``FinalLimit``, allora all'istante ``t+1`` la nuova posizione comandata sarà::

    P(t+1) = FinalLimit + D(t+1) - NUMBER_OF_FEEDS*STEP 

dove ``NUMBER_OF_FEEDS`` dipende dalla configurazione dinamica impostata.

Configurazioni BSC e OPTIMIZED
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In caso di configurazione ``BSC`` e ``OPTIMIZED``, il riavvolgimento automatico
per default viene fatto posizionando al posto dei feed utilizzati per il ``BSC``
degli altri feed tali che continuino a garantire la miglior copertura spaziale
e che non cambi la configurazione e che diano luogo 
da garantire la massima escursione del derotatore.

.. important:: E' semplice per l'astronomo capire quali feed hanno preso il posto
   dei precedenti?


Configurazione ALIGNED
~~~~~~~~~~~~~~~~~~~~~~

Riavvolgimento manuale
======================
In caso di modalità di riavvolgimento manuale, quando si arriva a fine corsa si ha::

    isTracking() #-> false
    isUpdating() #-> false
    isRewindingRequired() #-> true
    # Messaggio di WARNING a log


Il derotatore resterà fermo in attesa che venga chiamato il metodo 
``rewind()``::

    rewind(NUMBER_OF_FEEDS)

.. note:: Il numero di feed scelto per il riavvolgimento deve essere congruente con la
   configurazione utilizzata. Chiariremo meglio questo concetto nelle prossime sezioni.


Aligned
=======
In questa configurazione non è possibile effettuare il riavvolgimento,
per cui i metodi ``DewarPositioner.setRewindingMode()`` e 
``DewarPositioner.setAutoRewindingFeeds()`` se chiamati sollevano una eccezione::

    >>> DewarPositioner.setConfiguration('ALIGNED') 
    >>> DewarPositioner.setRewindingMode('AUTO')
    Traceback (most recent call last):
    ...
    NotAllowedEx: the ALIGN configuration does not allow the rewinding

Se si raggiunge il fine corsa, questo va segnalato sia tramite il flag *warning* 
dello status, sia scrivendo un messaggio a log. Inoltre::

    >>> DewarPositioner.isTracking() 
    False
    >>> DewarPositioner.isUpdating()
    False
    isRewindingRequired()
    True

Quando si raggiunge il fine corsa ci si trova quindi in una condizione di
stallo, per cui è necessario o cambiare configurazione oppure utilizzare
un nuovo set di feeds.



Il metodo setRewindingMode() va chiamato dopo che e' stata scelta una modalità
(quindi sempre, perche' di default c'e' una modalita' impostata)

