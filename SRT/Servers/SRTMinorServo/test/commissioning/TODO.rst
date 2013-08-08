Test tracking in funzione elevazione
====================================


Servo Park
==========
Provare il servoPark in combinazione con i servoSetup


Verificare il perche' non riesco comandare il mount dalla mia macchina
======================================================================


Test movimento lineare SRP
==========================
Test movimento SRP lineare quando le posizione vengono comandate in anticipo.


Test clear position della MSCU
==============================
Comandare una serie di posizioni future alla MSCU e fare il clear. Verificare
che non vengano eseguite.


Directory test
==============
Riordinare la directory dei test dei servo minori, rinominando in modo corretto i moduli.
Distinguere tra auto e non-auto.


Ricevitori
==========
Aggiornare 7GHz ed LP sulla base di quanto fatto per il KBand.


Notification Channel MinorServoBoss
===================================
Pubblicare il flag di tracking nel NC, ad ogni cambiamento e in ogni caso, ogni secondo.
Chiamare isReady() isOnFocus()? Stabilisco che il sistema e' onFocus() quando e' pronto e
la diff di posizione, rispetto a quella di fuoco, e' inferiore a un delta?
Stabilire la politica di tracking::

    isTracking = isOnFocus || isScanning

Quindi quando lo scan e' in atto avro' isTracking = true. Al termine dello scan avro'
isTracking



Interfaccia dei metodi definiti nelle prossime sezioni
======================================================
Implementare l'interfaccia dei metodi definiti nelle prossime sezioni, e fare il commit.


Informazioni sugli assi
=======================
Implementare il metodo del MinorServoBoss::

    void getAxesInfo(out ACS::stringSeq axes, out ACS::stringSeq units)

Il parametro `axes` avra' come elementi le descrizioni degli assi coinvolti. Ad esempio,
per la configurazione CCB sara'::

     axes = "SRP_XT", "SRP_YT", "SRP_ZT", "SRP_XR", "SRP_YR", "SRP_ZR", "GFR_ZR", "M3R_ZR"

dove T significa Translation e R Rotation.
Il parametro units contiene le unita' di misura degli assi, indicati con lo stesso ordine
che hanno in `axes`::

     units = "mm", "mm", "mm", "degree", "degree", "degree", "degree", "degree"

Leggere queste informazioni nella configurazione del CDB del Boss. Ad esempio, in CCB avremo oltre
che le posizioni dei servo anche i campi `AXES: ...`  e `UNITS: ...`.
Prima di implementarlo pensare a come scrivere le posizioni di getAxesPosition()


Richiesta della posizione dei servo ad un dato momento
======================================================
Implementare il metodo del MinorServoBoss::

    ACS::doubleSeq getAxesPosition(ACS::Time time)

Prende un tempo (passato) e restituisce la posizione dei servo all'istante indicato. Se l'istante
non esiste fa la media delle posizione all'istante prima e a quello dopo, pesata con la distanza (pesa
di piu' la posizione il cui tempo e' piu' vicino a time).
Gli assi sono nello stesso ordine di axes. Risoluzione temporale del thread che legge e aggiorna il 
vettore di queste posizioni: 100ms (sleep_time)
Prima di implementarlo pensare a come scrivere getAxesInfo()
La coda delle posizioni in memoria va svuotata con un park e anche con il setup. Usare un vector
e rimuovere le posizioni a partire dalla prima quando questo raggiunge una certa dimensione.


Lo scan degli assi
==================
Levare il metodo startScan() dall'interfaccia e rinominarlo, per poi wrapparlo con i seguenti metodi::

    startFocusScan(starting_time, range, total_time)
    startScan(ACS::time starting_time, double range, ACS::Time total_time, string code)

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

