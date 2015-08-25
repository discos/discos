*******************************
Nuova interpretazione posizione
*******************************

La posizione è data da::

    Pis + Pip + Pdp

dove:

  * Pis e' la posizione iniziale statica (nel senso che non cambia con azimuth ed
    elevazione, ma dipende solamente dall'asse di scansione e dalla configurazione),
    letta dal CDB, che dipende solamente dall'asse di scansione, 
  * Pip è la l'angolo parallatico iniziale, necessario per posizionare il derotatore nella 
    configurazione richiesta per l'asse di scansione (il valore dell'angolo parallatico 
    ad inizio scansione, che non dipende dall'asse ma solo dal puntamento (Az, El, Sector), 
    Pdt è il delta di angolo parallatico rispetto a Pit.

Introduciamo i comandi::

    derotatorGetCurrentScanInfo
      axis
      sector
      iStaticPos
      iParallacticPos (Pis)
      dParallacticPos (Pdt)


