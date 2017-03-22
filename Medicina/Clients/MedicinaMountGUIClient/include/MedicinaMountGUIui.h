// ACS includes
#include <MedicinaMountGUI.h>
#include <MedicinaMount.h>
#include <MedicinaMountClientEventLoop.h>

// QT includes
#include <QtCore/QObject>
#include <QtGui/QWidget>

class MedicinaMountGUI : public QWidget, public Ui_MedicinaMountGUI
{
    Q_OBJECT

    public:
        MedicinaMountGUI(QWidget *parent = 0);

        void setParameters(maci::SimpleClient* theClient, Antenna::MedicinaMount_var theMount);

    public slots:
        void Quit();
        void Stow();
        void UnStow();
        void Stop();
        void Rates();
        void AzElMode();
        void Preset();
        void ProgramTrack();
        void ResetFailures();
        void SetTime();

    private slots:
        void changeGUIAntennaMountStatusColor();
        void changeGUIACUStatusColor();
        void changeGUIPositionAndErrorValue();
        void changeGUIACUTimeValue();
        void changeGUIAntennaSectionValue();
        void changeGUIServoSystemColors();
        void changeGUIAzimuthElevationServoStatusColors();
        void changeGUIAzimuthElevationOperatingModeColors();
        void changeGUIPropertyErrorCode();
        void changeGUICatchAllErrorCode();

    private:
        MedicinaMount myMedicinaMount;
        MedicinaMountClientEventLoop myMedicinaMountClientEventLoop;
};
