Verificare il perche' non riesco comandare il mount dalla mia macchina
======================================================================
Chiedere MTM.


Test clear position della MSCU
==============================
Comandare una serie di posizioni future alla MSCU e fare il clear. Verificare
che non vengano eseguite.


Directory test
==============
Riordinare la directory dei test dei servo minori, rinominando in modo corretto i moduli.
Distinguere tra auto e non-auto.

Park e posizione comandata
==========================
Quando si fa il park, la posizione comandata deve essere quella di parcheggio?



Interfaccia dei metodi definiti nelle prossime sezioni
======================================================
Implementare l'interfaccia dei metodi definiti nelle prossime sezioni, e fare il commit.

Richiesta della posizione dei servo ad un dato momento
======================================================
OK: DONE!

Lo scan degli assi
==================
Levare il metodo startScan() dall'interfaccia e rinominarlo, per poi wrapparlo con i seguenti metodi::

    // Scansione del servo in fuoco primario, lungo l'asse zeta
    startFocusScan(starting_time, range, total_time)
    startScan(ACS::time starting_time, double range, ACS::Time total_time, string axis_code, string servo_name)

L'attributo `code` di startScan() e' uno dei codici degli assi (SRP_XT, ecc.), che poi devo mappare
in axis e servo da passare all'attuale startScan(), che e' stato rinominato.
Il metodo startFocusScan() fa uno scan in z (traslazione) del subriflettore o del PFP, a seconda di quello che e' in
fuoco.

Implementare i metodo::

    double getFocusScanPosition(ACS::Time t)

Questo restituisce la posizione TZ del subriflettore o PFP al tempo t indicato.

Implementare i metodo::

    double getScanPosition(ACS::Time t, string code)

Dal parametro code recupera il servo e l'asse (SRP_XT, GFR_ZR, ecc.) e restutisce la posizione di quell'asse di 
quel particolare servo, al tempo t.


Tabella Superficie Attiva
=========================
Vedere se temporaneamente possiamo far comunicare sotto altra forma
il manager di gavino-mng e quello della AS, in modo che AS conosca
l'elevazione ed gavino-mng sappia se la superficie attiva sta inseguendo
l'elevazione. In questo modo, prima di caricare la configurazione
vado a leggere lo stato della superficie attiva, e nel caso in cui
non sia in tracking allora carico la tabella alternativa.
Quanto non riesco a recuperare l'elevazione devo lanciare una eccezione perche' l'osservazione
non puo' continuare.


Vefifica efficacia correzioni in funzione dell'elevazione
=========================================================
Fare un plot che mostra l'efficienza di antenna in funzione dell'elevazione,
con tracking abilitato e con tracking disabilitato, e anche con posizione del
subriflettore allo zero (come e' sempre stato usato)


Ottenere informazioni sul tipo di polinomio utilizzato
======================================================
Implementare il metodo del MinorServoBoss::

    string getCorrectionsType(string servo_name)

Questo restituisce una stringa che puo' essere:

- "NONE" se non vi sono correzioni (il servo non e' dinamico)
- "AS-ACTIVE"
- "AS_NOT_ACTIVE"

Implementare clearemergency
===========================
