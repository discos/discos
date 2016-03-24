// System includes

// ACS includes
#include <maciSimpleClient.h>
#include <acsncSimpleConsumer.h>
#include <baci.h>
#include "XBackendsC.h"
#include <FitsWriterC.h>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <IRA>

// QT includes
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtGui/QColor>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
// QT includes
#include <QtCore/QObject>
#include <QtGui/QWidget>

#define MONITOR_TIME_TRIGGER 10000000 // 1 second


class XControlSystem : public QThread, public virtual POA_ACS::CBpattern
{
    Q_OBJECT

public:    
	bool BUSY;
	CString NameFile;
    XControlSystem(QObject *parent = 0);
    ~XControlSystem(){ };
    
    void setSimpleClient(maci::SimpleClient*);
    void setSender(Backends::XBackends_var);
    void setRecv(Management::FitsWriter_var);
    void run();
    void stop();

    void working(ACS::pattern value, const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
    void done(ACS::pattern value, const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
    CORBA::Boolean negotiate(ACS::TimeInterval time_to_transmit, const ACS::CBDescOut &desc) throw (CORBA::SystemException) {
	    return true;}
	
    signals:
	void upState( char status );
	
private:
    maci::SimpleClient* sc;
    bool eventEnd;
    Backends::XBackends_var tSender;
    Management::FitsWriter_var tRecv;
    void setGUIAntennaMountStatus(ACS::pattern);
};
