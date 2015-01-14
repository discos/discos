Impostare posizione
===================
Il comando derotatorSetPosition, nel caso di posizione out of range,
non solleva eccezione (visto che viene sollevata nel thread), e quindi
da operator input non viene segnalato l'errore all'utente.


Comandi per lettura scrittura posizione + informazioni scan
===========================================================
Attualmente per impostare e leggere la posizione abbiamo i comandi::

    derotatorSetPosition 
    derotatorGetPosition

Il primo consente di impostare la posizione del derotatore (solo nella
FIXED e CUSTOM), il secondo di leggere la posizione attuale.

Vi propongo di creare anche qualche altro comando. Per conoscere la 
posizione initiale, noto l'asse di scansione (quindi una volta che
il derotatore e' configurato) vorrei introdurre::


    derotatorGetInitialPosition=AXIS

In questo caso, per le configurazioni in cui non sommiamo l'angolo parallatico
ma aggiorniamo utilizzando il delta, come posizione iniziale consideriamo 
Pi + D(0)? D(0) sarebbe l'angolo parallatico iniziale.

Per conoscere i parametri della scansione in corso::

    derotatorGetActualScanInfo
      -> AXIS
      -> SECTOR
      -> InitialPosition
      -> DynamicPosition 

Restituisce l'asse di scansione, il settore, la posizione iniziale e
il contributo dinamico.



