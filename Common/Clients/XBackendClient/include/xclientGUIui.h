#ifndef XCLIENTGUIUI_H_
#define XCLIENTGUIUI_H_
#include <QtGui/QMainWindow>
#include "xclientGUI.h"
#include <QtCore/QObject>

#include "headResult.h"
#include "Specifiche.h"
#include "DataIntegrazione.h"
#include "xplot.h"
// ACS includes
#include <maciSimpleClient.h>
#include <acsncSimpleConsumer.h>
#include <baci.h>
#include <XBackendsC.h>
#include <FitsWriterC.h>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <IRA>
#include "XControlSystem.h"
#include "Led.h"

class xclientGUIui: public QMainWindow
{
    Q_OBJECT
public:	
	xclientGUIui(QWidget *parent = 0);
	~xclientGUIui();
public:
	XControlSystem XThread;
	int N;
	Specifiche spec;/*!< Specifiche object */	
	vector<DataIntegrazione> RisultatiIntegrazione;/*!< vector of DataIntegrazione */
	Xplot *p;
	int Open();
	Backends::XBackends_var sender;
	Management::FitsWriter_var recv;
	maci::SimpleClient* sc;
	void setParameters(maci::SimpleClient* theClient,Backends::XBackends_var theSender,Management::FitsWriter_var theRecv);
	bool INT;
	QString QCatchAllError;
	
public slots:   
	void Rivisualizza(int N);
	void FileExit();
	void OpenXplot();	
	bool OpenFile(char path[],char name[]);
	void Check();
	void Integra(); 
	void StartInt();
	void AbortInt();
	void Broadcast();
	void Modifica(); 
	void Successivo_activated();
	void Precedente_activated();
	void UpdateStatus(char status);
	void changeGUICatchAllErrorCode();
	
signals:
    int setGUICatchAllErrorCode();	
    
protected:  
  char *server;
  int port;
  
private:
    void initAttributes() ;
    void visualizza(int n);
    void Salva(int n);
    void SetReadOnly(bool modo);

    Led *ledHW, *ledData, *ledSetting,*ledActive,*ledAbort,*ledOverflowData, *ledBusy;
	Ui_ClientClass ui;
   
};

#define _CATCH_ALL(ROUTINE) \
	catch (ComponentErrors::ComponentErrorsEx &E) { \
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,ROUTINE); \
		IRA::CString CMessage; \
		_EXCPT_TO_CSTRING(CMessage,impl); \
		impl.log(); \
        QCatchAllError = QString("%1").arg((const char*)CMessage); \
	} \
	catch (CORBA::SystemException &C) { \
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,ROUTINE); \
		impl.setName(C._name()); \
		impl.setMinor(C.minor()); \
		IRA::CString CMessage; \
		_EXCPT_TO_CSTRING(CMessage,impl); \
		impl.log(); \
        QCatchAllError = QString("%1").arg((const char*)CMessage); \
	} \
	catch (...) { \
		_EXCPT(ClientErrors::UnknownExImpl,impl,ROUTINE); \
		IRA::CString CMessage; \
		_EXCPT_TO_CSTRING(CMessage,impl); \
		impl.log(); \
        QCatchAllError = QString("%1").arg((const char*)CMessage); \
	} \
    emit setGUICatchAllErrorCode();

#endif /*XCLIENTGUIUI_H_*/
