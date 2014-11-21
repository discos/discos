.. _oi:

*********************
Guida per l'astronomo
*********************
In questa sezione viene descritto come interagire con il ``DewarPositioner``
tramite *operator input*.

.. _oisetup:

Esecuzione del setup
====================
Il ``DewarPositioner`` gestisce tutti i derotatori alla stessa maniera, 
per cui in fase di *setup* è necessario indicare il codice del derotatore
che si intende utilizzare::

   > derotatorSetup=CODE

I codici sono i medesimi che vengono utilizzati per effettuare il setup dell'antenna.
Ad esempio, se si volesse utilizzare il derotatore del ricevitore
in banda K::

    > derotatorSetup=KKG

.. note:: Quando viene eseguito il *setup* del telescopio viene
          automaticamente eseguito anche il *setup* del ``DewarPositioner``.
          In altre parole, quando viene dato il comando ``setupKKG``,
          questo esegue a sua volta anche ``derotatorSetup=KKG``.

Al termine del *setup* il derotatore sarà pronto per essere utilizzato. Il
comando ``derotatorGetActualSetup`` restituisce il setup attuale, mentre il
comando ``derotatorIsReady`` ci dice se il derotatore è pronto per essere
movimentato::

    > derotatorGetActualSetup
    KKG
    > derotatorIsReady
    True

Durante il *setup* al derotatore viene comandata la posizione ``0``, che è quella 
scelta per l'allineamento iniziale. Possiamo verificarlo con il comando
``derotatorGetPosition``::

    > derotatorGetPosition
    0.0084d

Ad esempio, per il il derotatore del ricevitore in banda K la posizione iniziale è quella in cui i tre feed 1, 0, 4 sono 
paralleli all'orizzonte, con il feed 1 a est.
Il *setup* infine imposta i valori di default per la configurazione e la modalità di 
riavvolgimento::


    > derotatorGetConfiguration
    FIXED
    > derotatorGetRewindingMode
    AUTO

Parleremo delle configurazioni e del riavvolgimento nelle prossime sezioni.

.. _oiconfigurations:

Configurazione del DewarPositioner
==================================

Il ``DewarPositioner`` ha cinque configurazioni:
:ref:`fixed <fixed>`,
:ref:`best space coverage <bsc>`, :ref:`optimized <optimized>`,
:ref:`aligned <aligned>` e :ref:`custom <custom>`.
Il comando ``derotatorSetConfiguration`` consente all'utente di impostare
la configurazione desiderata, mentre il comando ``derotatorGetConfiguration``
di leggerla::

    > derotatorSetConfiguration=FIXED
    > derotatorGetConfiguration
    FIXED
    > derotatorGetConfiguration
    BSC
    > derotatorSetConfiguration=CUSTOM
    > derotatorGetConfiguration
    CUSTOM

Le configurazioni ``OPTIMIZED`` e ``ALIGNED`` non sono al momento disponibili::

    > derotatorSetConfiguration=OPTIMIZED
    Error - configuration OPTIMIZED not available
    > derotatorSetConfiguration=ALIGNED
    Error - configuration ALIGNED not available

Vediamo ora nel dettaglio le varie configurazioni, suddividendole in
:ref:`statiche <statics>` e :ref:`dinamiche <dinamics>`.


.. _statics:

Configurazioni *statiche*
-------------------------
Nelle configurazioni statiche la posizione del derotatore non cambia al
variare della posizione dell'antenna o dell'asse di scansione.


.. _fixed:

Configurazione *fixed*
~~~~~~~~~~~~~~~~~~~~~~
In questa configurazione, che è quella che viene impostata come default dal
*setup*, la posizione del derotatore viene mantenuta
fissa al variare della posizione dell'antenna, e questo è il motivo 
per cui le è stato assegnato il codice identificativo ``FIXED``. 

Nella configurazione ``FIXED`` è possibile impostare la posizione del
derotatore utilizzando il comando ``derotatorSetPosition``::

    > derotatorSetConfiguration=FIXED
    > derotatorSetPosition=30d
    > derotatorGetPosition
    30d

Se il derotatore si trova in     una certa posizione ``Px`` e viene impostata
la modalità ``FIXED``, viene tenuta la posizione ``Px``. Il derotatore quindi
non viene riportato in posizione di zero sinchè non viene comandata
una nuova posizione con ``derotatorSetPosition``::


    > derotatorGetPosition
    50d
    > derotatorSetConfiguration=FIXED
    > derotatorGetConfiguration
    FIXED
    > derotatorGetPosition
    50d
    > derotatorSetPosition=10d
    > derotatorGetPosition
    10d

.. _dinamics:

Configurazioni dinamiche
------------------------
Nelle configurazioni statiche la posizione del derotatore non viene
aggiornata al variare della posizione dell'antenna o dell'asse
di scansione. Nelle configurazioni *dinamiche* invece 
il ``DewarPositioner`` aggiorna la posizione del derotatore in funzione
della posizione dell'antenna, al fine di compensare l'angolo parallatico
(più un eventuale contributo del *galactic parallactic angle*, 
a seconda dell'asse di scansione). 

Nelle configurazioni dinamiche la posizione del derotatore è data
dalla seguente equazione::

    P = Pi + D(AZ, EL, AXIS)

dove ``Pi`` è una *posizione iniziale*, mentre ``D(AZ, EL, AXIS)``
è la cosidetta *funzione di derotazione*, che serve
per compensare l'angolo parallattico (o il contributo del
*galactic parallactic angle*).
Ciò che differenzia una configurazione
dinamica dall'altra è la posizione iniziale, mentre la 
funzione di derotazione non cambia, ed è data da:

    * ``D = 0`` quando ``AXIS`` è ``HOR_LON`` o ``HOR_LAT``
    * ``D = P(AZ, EL)`` quando ``AXIS`` è ``TRACK``, ``EQ_LON``, ``EQ_LAT`` 
      o ``GCIRCLE``
    * ``D = G(AZ, EL)`` quando ``AXIS`` è ``GAL_LON`` o ``GAL_LAT``

dove ``P(AZ, EL)`` è la funzione di compensazione dell'angolo parallatico,
mentre ``G(AZ, EL)`` è quella di compensazione del contributo dovuto al
*galactic parallactic angle* (GPA).

Quando viene impostata una configurazione, la posizione del derotatore non viene aggiornata,
visto che non è ancora noto l'asse di scansione. L'aggiornamento viene comandato da Nuraghe/ESCS
nel momento in cui inizia lo scan.


.. _bsc:

Configurazione *best space coverage*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Il codice associatò a questa configurazione è ``BSC``::

    > derotatorSetConfiguration=BSC
    > derotatorGetConfiguration
    BSC

Quando questa configurazione è attiva, il sistema prima posiziona il derotatore
in una posizione iniziale che indicheremo con ``Pis`` (il pedice *i* sta per
*initial*, mentre il secondo pedice indica il tipo di
configurazione, e in questo caso significa *space*),
dopodiché aggiunge a ``Pis`` il contributo alla *derotazione* (che indicheremo
con ``D``) dovuto alla
compensazione dell'angolo parallatico più eventuale contributo del
*galactic parallactic angle*, a seconda
dell'asse di scansione scelto. 
La posizione del derotatore, che in questa configurazione indichiamo 
con ``Ps``, è quindi data dalla seguente equazione:

.. code-block:: none

   Ps = Pis(AXIS) + D(AZ, EL, AXIS) # BSC (Best Space Coverage)

.. note:: Per un dato derotatore, il valore della posizione iniziale ``Pi`` 
          viene letto da una tabella di configurazione e
          dipende dall'asse di scansione, per cui abbiamo utilizzato
          la notazione ``Pi(AXIS)`` per indicare che ``Pi`` è funzione 
          dell'asse. Allo stesso modo, la funzione di compensazione
          dell'angolo (parallatico più eventuale contributo del GPA) dipende dai 
          valori dell'azimuth, dell'elevazione e dell'asse di scansione,
          per cui la abbiamo indicata con ``D(AZ, EL, AXIS)``.

I feed vengono disposti in modo tale da 
avere la miglior copertura spaziale della sorgente durante una scansione.
Tipicamente la miglior copertura viene ottenuta
equispaziando, quando possibile, i beam nella 
direzione ortogonale a quella di scansione (se si sta facendo una scansione in 
azimuth i feed vengono equispaziati in elevazione, in modo da ottimizzare la 
scansione dell'area osservata).

Quando è impostata la modalità ``BSC`` all'utente non è consentito il posizionamento
del derotatore::

    > derotatorSetConfiguration=BSC
    > derotatorSetPosition=50d
    Error - setPosition() not allowed in BSC configuration

In questa modalità il set di feed posizionati in modo da garantire la
massima copertura spaziale sono stabilti a priori (ad esempio per il
banda K sono i feed 1, 0 e 4), e questo significa che la configurazione
``BSC`` non è ottimizzata per garantire la massima escursione del derotatore.

.. _optimized:

Configurazione *optimized*
~~~~~~~~~~~~~~~~~~~~~~~~~~
Questa configurazione è analoga alla :ref:`best space coverage <bsc>` ma a differenza di
quest'ultima, all'inizio di ogni scan la posizione del derotatore
viene calcolata oltre che per ottenere la massima copertura spaziale del
multifeed lungo l'asse di scansione, anche per massimizzare
la durata dello scan prima che si renda necessario riavvolgere, per cui
la posizione iniziale va scelta in modo che il set di feed garantisca
la massima copertura spaziale durante lo scan, e che sia tale da
essere la più vicina possibile a uno dei fine corsa del derotatore (quello
dal quale ci si allontana durante lo scan).

La configurazione *optimized* è identificata con il codice ``OPTIMIZED``::

    > derotatorSetConfiguration=OPTIMIZED
    > derotatorGetConfiguration
    OPTIMIZED

Quando è impostata la modalità ``OPTIMIZED`` all'utente non è consentito il posizionamento
del derotatore::

    > derotatorSetConfiguration=OPTIMIZED
    > derotatorSetPosition=50d
    Error - setPosition() not allowed in OPTIMIZED configuration


.. _aligned:

Configurazione *aligned*
~~~~~~~~~~~~~~~~~~~~~~~~
In questa configurazione, il cui codice identificativo è ``ALIGNED``,
viene scelto il set di feed che si vuole allineare con l'asse di scansione.
In Nuraghe/ESCS vi sarà una tabella che riporterà, per ogni derotatore,
i possibili set. La posizione del derotatore è data da::

   Pa = Pia(AXIS) + D(AZ, EL, AXIS) 

.. attention:: Se il derotatore non compre un angolo di almento 360°, non
   è detto che sia possibile allineare un certo set di feed con un dato
   asse. In generale però se non è possibile allinearli con un asse, è 
   probabile che li si possa allineare con quello ortogonale.

Rispetto alle altre configurazioni dinamiche, nella configurazione *aligned*
vi è un ulteriore comando da utilizzare, chiamato ``derotatorSetAlignment``,
che prende come argomento una stringa identificativa dei feed che si 
vuole allineare.
Nella stringa i feed devono essere separati da un segno meno::

    > derotatorSetConfiguration=ALIGNED
    > derotatorSetAlignment=0-4

In questo caso viene scelto il set a cui appartengono
i feed 0 e 4 (ad esempio, nel caso del banda K verrebbe scelto il set ``{1, 0, 4}``).

.. note:: Se non viene scelto un allineamento, allora viene utilizzato
   un allineamento di default (nel caso del banda K è quello ``{1, 0, 4}``).

Concludiamo dicendo che così come per la configurazione ``BSC`` e ``OPTIMIZED``, 
anche la ``ALIGNED`` non consente l'utilizzo del comando ``derotatorSetPosition``.

.. _custom:

Configurazione *custom*
~~~~~~~~~~~~~~~~~~~~~~~
In questa configurazione la posizione iniziale può essere impostata 
dall'utente, e per tale motivo a questa configurazione è stato assegnato
il codice identificativo ``CUSTOM``. La posizione del derotatore è data da::

   Pc = Pic + D(AZ, EL, AXIS) 

Rispetto ai casi di configurazione dinamica appena visti, nella modalità
*custom* è necessario specificare la posizione iniziale, altrimenti
verrà utilizzata come ``Pic`` la posizione attuale. Ad esempio, se
si vuole avere una posizione iniziale di 30°::

    > derotatorSetConfiguration=CUSTOM
    > derotatorSetPosition=30d

Come al solito l'aggiornamento viene avviato da Nuraghe/ESCS nel momento
in cui viene comandata la scansione lundo un dato asse.


Interrompere l'aggiornamento
----------------------------
Se si vuole interrompere l'aggiornamento della posizione, si deve 
impostare la configurazione :ref:`fixed <fixed>`. In questo caso il derotatore si 
fermerà all'ultima posizione comandata.


