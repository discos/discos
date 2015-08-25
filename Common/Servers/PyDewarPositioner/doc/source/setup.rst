*************************
Setup del DewarPositioner
*************************
Il ``DewarPositioner`` è un component di Nuraghe/ESCS che gestisce
tutti i derotatori alla stessa maniera. Per poter utilizzare
un particolare derotatore è quindi necessario eseguire il
*setup* del ``DewarPositioner`` indicando il derotatore che
si intende utilizzare.

Il codice che identifica il derotatore è quello associato al
relativo ricevitore.
Ad esempio, il codice associato al derotatore del ricevitore in
banda K è **KKG**.

Esecuzione del setup
====================
Il *setup* può essere eseguito sia in modo programmatico (da un
programma) utilizzando le API del component, sia in modo 
interattivo da parte di un utente
che interagisce con Nuraghe/ESCS tramite l'*operator input*.

.. _oisetup:

Setup tramite Operator Input
----------------------------
L'astronomo interagisce con Nuraghe tramite la console dell'*operator input*,
per cui per poter eseguire il *setup* del ``DewarPositioner`` deve
utilizzare da *operator input* l'apposito comando ``derotatorSetup`` 
passandogli il codice che identifica il derotatore::

   derotatorSetup=CODE

Ad esempio, se si volesse utilizzare il derotatore del ricevitore
in banda K::

   derotatorSetup=KKG

.. note:: Quando viene eseguito il *setup* del telescopio viene
          automaticamente eseguito anche il *setup* del ``DewarPositioner``.
          In altre parole, quando viene dato il comando ``setupKKG``,
          questo esegue a sua volta anche ``derotatorSetup=KKG``.


Setup utilizzando le API
------------------------
Il *setup* viene eseguito dal metodo ``DewarPositioner.setup()``,
che prende l'argomento ``CODE`` di tipo stringa. Ad esempio, nel 
caso del derotatore del banda K::

   DewarPositioner.setup('KKG')

Il *setup* tramite *operator input* (vedi
:ref:`oisetup`) da quindi luogo alla chiamata
a ``DewarPositioner.setup()``:

.. code-block:: none

   derotatorSetup=KKG -> DewarPositioner.setup('KKG')

.. _setup_actions:

Azioni compiute dal *setup*
===========================
Quando viene eseguito il *setup*, il ``DewarPositioner`` ottiene il 
riferimento al derotatore associato a ``CODE``,
e opera su di esso basandosi sull'interfaccia del 
``GenericDerotator`` (ad esempio, ``DewarPositioner.setup('KKG')`` ottiene il 
riferimento al component ``KBandDerotator``).
Fatto ciò, vengono impostati i seguenti valori di default::

    DewarPositioner.setConfiguration('FIXED')
    DewarPositioner.setPosition(0) 
    DewarPositioner.setRewindingMode('AUTO')

La posizione ``0`` è quella scelta per l'allineamento iniziale. Ad esempio,
per il `KBandDerotator` è quella in cui i tre feed 1, 0, 4 sono 
paralleli all'orizzonte, con il feed 1 a est.
Discuteremo del metodo ``setConfiguration()`` nella sezione 
:ref:`configurations`, mentre il metodo ``setRewindingMode()`` è
discusso in :ref:`rewinding`.

.. note:: Il derotatore del banda K è stato allineato meccanicamente con
          i tre feed 1, 0 e 4 paralleli all'orizzonte, e in tale posizione
          è stato letto un numero di conteggi dell'encoder del motore
          pari a 18210. In tale posizione, il numero di conteggi letti dal
          sensore di posizione è pari a 731719.
          Il limiti negativo e positivo corrispondono, rispettivamente,
          ad un numero di conteggi dell'encoder del motore pari a -19330 e
          +19330. Dal punto di vista dell'interfaccia del derotatore (e
          quindi della posizione vista dall'osservatore), se poniamo come
          riferimento di zero la posizione dei tre feed 1, 0 e 4 paralleli
          all'orizzonte (731719 step del sensore di posizione), allora
          il limite negativo è pari a -6.03 gradi, mentre il limite positivo
          è pari a 204.97 gradi.

