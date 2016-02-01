.. _oipark:

*************************
Parcheggio del derotatore
*************************
Il derotatore viene parcheggiato con il comando ``derotatorPark``,
il quale da luogo alla chiamata al metodo ``DewarPositioner.park()``.
Il *park* fa il *reset* della configurazione (cancella la configurazione attuale 
e comandata) e della modalità di riavvolgimento. Infine posiziona il derotatore in 
parcheggio. Al termine del ``park()`` il ``DewarPositioner`` non sarà più 
configurato, per cui ``isConfigured()`` restituirà False. Ecco una sessione di
lavoro con l'interprete interattivo di Python::

    >>> DewarPositioner.isConfigured() # Non è stato fatto il setup
    False
    >>> DewarPositioner.getConfiguration()
    ''
    >>> DewarPositioner.setup('KKG') 
    >>> DewarPositioner.isConfigured()
    True
    >>> DewarPositioner.getConfiguration() # Configurazione di default
    'FIXED'
    >>> DewarPositioner.getPosition() # Posizione di default
    0
    >>> DewarPositioner.setPosition(30)
    >>> DewarPositioner.getPosition()
    30
    >>> DewarPositioner.park()
    >>> DewarPositioner.getConfiguration()
    ''
    >>> DewarPositioner.getPosition()
    0

