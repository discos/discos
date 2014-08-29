.. _rewinding:

**************
Riavvolgimento
**************
Se ``STEP`` è l'angolo tra due feed (60 gradi per il banda K) e supponiamo 
che all'istante ``t`` il derotatore sia in posizione di fine corsa 
``FinalLimit``, allora all'istante ``t+1`` la nuova posizione comandata sarà::

    P(t+1) = FinalLimit + D(t+1) - NUMBER_OF_FEED*STEP 

Il tipo di riavvolgimento lo si imposta tramite il metodo 
``setRewindingMode(MODE)``. L'argomento ``MODE`` può essere ``AUTO`` (default) 
o ``MANUAL``.

Il metodo ``setAutoRewindingFeeds(NUMBER_OF_FEEDS)`` imposta il riavvolgimento
automatico pari al numero di feed passato come argomento. La chiamata a questo
metodo imposta anche la modalità ``AUTO``, qualora non fosse già impostata.
Qualora il numero di feeds fosse superiore a quello massimo consentito per il
riavvolgimento, verrà sollevata una eccezione.

Il metodo ``clearAutoRewindingFeeds()`` chiamato in modalità di rewinding AUTO,
fa si che il numero di feeds di rotazione del riavvolgimento automatico venga
calcolato dal sistema. In altre parole, annulla il ``setAutoRewindingFeeds``.

In caso di modalità ``MANUAL``, quando si arriva a fine corsa si ha::

    isTracking() #-> false
    isUpdating() #-> false
    isRewindingRequired() #-> true
    # Messaggio di WARNING a log


Il derotatore resterà fermo in attesa che venga chiamato il metodo 
``rewind()``::

    rewind(NUMBER_OF_FEEDS)

Durante il riavvolgimento::

    isRewinding() #-> true
    isTracking() #-> false
    isUpdating() #-> ?
