// System includes

// ACS includes
#include <baci.h>
#include <ActiveSurfaceBossC.h>
#include <SRTActiveSurfaceBossC.h>
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
#include <QtGui/QApplication>

using namespace IRA;

class SRTActiveSurfaceCore : public QThread
{
    Q_OBJECT

	public:
        SRTActiveSurfaceCore(QObject *parent = 0);
        ~SRTActiveSurfaceCore();

	    void setASBoss(ActiveSurface::SRTActiveSurfaceBoss_var ASBoss);
	    void run(void);
        void stop(void);
        void setactuator(int circle, int actuator);
	    //void setallactuators();
	    //void setcircleORradius(int circle, int radius);
	    //void setactuatorcolor(int circle, int actuator, QColor color);
	    //void setactuatorstatuslabels(int circle, int actuator);
	    void reset(int circle, int actuator, int radius);
	    void up(int circle, int actuator, int radius);
	    void down(int circle, int actuator, int radius);
	    void top(int circle, int actuator, int radius);
	    void bottom(int circle, int actuator, int radius);
	    void stopUSD(int circle, int actuator, int radius);
	    void calibrate(int circle, int actuator, int radius);
	    void calVer(int circle, int actuator, int radius);
	    void stow(int circle, int actuator, int radius);
	    void setup(int circle, int actuator, int radius);
	    void refPos(int circle, int actuator, int radius);
	    void move(int circle, int actuator, int radius, long incr);
	    void setProfile(long int profile);
	    void correction(int circle, int actuator, int radius, double correction);
	    void update(double elev);
	    void clearactuatorslineedit();
        void enableAutoUpdate();
        void disableAutoUpdate();
        void recoverUSD(int circle, int actuator);
        void park();

	    bool monitor;
        int theCircle, theActuator, theRadius;
	    QString ActuatorActualPosition_str;
	    QString ActuatorCommandedPosition_str;
	    QString mm_ActuatorActualPosition_str;
	    QString mm_ActuatorCommandedPosition_str;
	    QString ActuatorDelay_str;
	    QString ActuatorFmin_str;
	    QString ActuatorFmax_str;
	    QString ActuatorAcceleration_str;
        int ActuatorStatusEnblLabelCode;
        int ActuatorStatusRunLabelCode;
        int ActuatorStatusCammLabelCode;
        int ActuatorStatusLoopLabelCode;
        int ActuatorStatusCalLabelCode;

    signals:
        int setGUIActuatorColor(int, int, const char*, bool);
        int setGUIAllActuators(bool callfromfunction = false);
        int setGUIcircleORradius(bool callfromfunction = false);
        int setGUIActuator(bool callfromfunction = false);
        int setGUIActuatorStatusEnblLabel();
        int setGUIActuatorValues();
        int setGUIActuatorStatusLabels();
        int setGUIasStatusCode(int);
        int setGUIasProfileCode(int);
        
	private:
	    ActiveSurface::SRTActiveSurfaceBoss_var tASBoss;
	    int actuatorcounter;
	    int circlecounter;
	    int totacts;
};
