/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include <iostream>
#include <logging.h>
#include <acserr.h>
#include <MedicinaMount.h>

extern MedicinaMount* myMedicinaMount;

void MedicinaMountGUI::Quit()
{
    ACS_SHORT_LOG((LM_INFO,"MedicinaMountGUI::Quit"));
    // Stops Main Threads
    myMedicinaMount->stop();
    close();
}

void MedicinaMountGUI::Stow()
{
    ACS_SHORT_LOG((LM_INFO,"MedicinaMountGUI::Stow"));
    myMedicinaMount->stow();
}

void MedicinaMountGUI::UnStow()
{
    ACS_SHORT_LOG((LM_INFO,"MedicinaMountGUI::UnStow"));
    myMedicinaMount->unstow();
}

void MedicinaMountGUI::Stop()
{
    ACS_SHORT_LOG((LM_INFO,"MedicinaMountGUI::Stop"));
    myMedicinaMount->acustop();
}

void MedicinaMountGUI::ResetFailures()
{
    ACS_SHORT_LOG((LM_INFO,"MedicinaMountGUI::Reset Failures"));
    myMedicinaMount->resetfailures();
}

void MedicinaMountGUI::Preset()
{
    double az, el;
    QString AZstr, ELstr;
    AZstr = ComAZlineEdit->text();
    az = AZstr.toDouble();
    ELstr = ComELlineEdit->text();
    el = ELstr.toDouble();
    myMedicinaMount->Preset(az,el);
}

void MedicinaMountGUI::ProgramTrack()
{
    double az, el;
    QString AZstr, ELstr, Timestr;
    AZstr = ComAZlineEdit->text();
    az = AZstr.toDouble();
    ELstr = ComELlineEdit->text();
    el = ELstr.toDouble();
    ACS::Time tm;
    Timestr = TimeCommandedlineEdit->text();
    tm = Timestr.toLongLong();
    myMedicinaMount->ProgramTrack(az, el, tm, true);
}

void MedicinaMountGUI::Rates()
{
    double azrate, elrate;
    QString AZratestr, ELratestr;
    AZratestr = AZratelineEdit->text();
    azrate = AZratestr.toDouble();
    ELratestr = ELratelineEdit->text();
    elrate = ELratestr.toDouble();
    myMedicinaMount->Rates(azrate,elrate);
 }

void MedicinaMountGUI::SetTime()
{
    ACS::Time tm;
    QString Timestr;
    Timestr = ACUsetTimelineEdit->text();
    tm = Timestr.toLongLong();
    myMedicinaMount->SetTime(tm);
}

void MedicinaMountGUI::AzElMode()
{
    int AZmode;
    int ELmode;
    AZmode = AZModecomboBox->currentItem();
    ELmode = ELModecomboBox->currentItem();
    myMedicinaMount->changeAZELmode(AZmode, ELmode);
}

void MedicinaMountGUI::init()
{

}

void MedicinaMountGUI::destroy()
{

}
