// ACS includes
#include <MedicinaActiveSurfaceGUI.h>
#include <MedicinaActiveSurfaceCore.h>
#include <MedicinaActiveSurfaceClientEventLoop.h>

// QT includes
#include <QtCore/QObject>
#include <QtGui/QWidget>

class MedicinaActiveSurfaceGUI : public QWidget, public Ui_MedicinaActiveSurfaceGUI
{
    Q_OBJECT

    public:
        MedicinaActiveSurfaceGUI(QWidget *parent = 0);

        void setParameters(maci::SimpleClient* theClient, ActiveSurface::MedicinaActiveSurfaceBoss_var theASBoss);
        void clearactuatorslineedit();

    public slots:
        void Quit();
        void move();
        //void setprofile();
        void correction();
        void update();
        void reset();
        void stop();
        void up();
        void down();
        void top();
        void bottom();
        void calibrate();
        void calVer();
        void stow();
        void setup();
        void refPos();
        void recoverUSD();
        void setupAS();
        void startAS();
        void stowAS();
        void stopAS();
        void setallactuators();
        void setradius();
        void setcircle();
        void setactuator();

    private slots:
        void changeGUIActuatorColor(int, int, bool, bool);
        void changeGUIAllActuators(bool callfromfunction = false);
        void changeGUIcircleORradius(bool callfromfunction = false);
        void changeGUIActuator(bool callfromfunction = false);
        void changeGUIActuatorStatusEnblLabel();
        void changeGUIActuatorValues();
        void changeGUIActuatorStatusLabels();
        void changeGUIasStatusCode(int);
        void changeGUIasProfileCode(int);

    private:
        MedicinaActiveSurfaceCore myMedicinaActiveSurfaceCore;
        MedicinaActiveSurfaceClientEventLoop myMedicinaActiveSurfaceClientEventLoop;
};
