#sequential schedule.  azimuth scans on 3c286 are done. Before that a tsys scan is done (no recording). After a an acquisition (on source) and a tsys is done
PROJECT:	Test						
OBSERVER:	ESCS						
SCANLIST:	Test.lis						
MODE:	SEQ
PROCEDURELIST:	Test.cfg						
BACKENDLIST:	Test.bck						
SCANID:	1000						
INITPROC:	NULL			
#ID	Label	durat	scan#	preScan	postScan	bckend

1	tsys	0.0	2	NULL	CALLTSYS	STD:MANAGEMENT/FitsZilla
2	3c286_az	17.5	3	NULL	POSTSCAN	STD:MANAGEMENT/FitsZilla
3	3c286	15.0	1	PRESCAN	POSTSCAN	STD:MANAGEMENT/FitsZilla