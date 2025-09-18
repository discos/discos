// ACS includes
#include <NotoActiveSurfaceGUI.h>
#include <NotoActiveSurfaceCore.h>
#include <NotoActiveSurfaceClientEventLoop.h>

// QT includes
#include <QtCore/QObject>
#include <QtGui/QWidget>

class NotoActiveSurfaceGUI : public QWidget, public Ui_NotoActiveSurfaceGUI
{
    Q_OBJECT

    public:
        NotoActiveSurfaceGUI(QWidget *parent = 0);

        void setParameters(maci::SimpleClient* theClient, ActiveSurface::ActiveSurfaceBoss_var theASBoss);
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
        NotoActiveSurfaceCore myNotoActiveSurfaceCore;
        NotoActiveSurfaceClientEventLoop myNotoActiveSurfaceClientEventLoop;
};
