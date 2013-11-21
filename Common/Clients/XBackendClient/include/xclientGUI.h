/********************************************************************************
** Form generated from reading ui file 'xclientGUI.ui'
**
** Created: Fri May 21 18:12:31 2010
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef XCLIENTGUI_H
#define XCLIENTGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_ClientClass
{
public:
    QAction *action_Exit;
    QAction *actionXPlot;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBoxSetting;
    QGridLayout *gridLayout1;
    QHBoxLayout *hboxLayout1;
    QVBoxLayout *vboxLayout1;
    QLabel *label;
    QLineEdit *freqLine;
    QVBoxLayout *vboxLayout2;
    QLabel *label_2;
    QSpinBox *gainBox;
    QVBoxLayout *vboxLayout3;
    QLabel *label_3;
    QSpinBox *ChInBox;
    QVBoxLayout *vboxLayout4;
    QLabel *label_6;
    QComboBox *bandBox;
    QVBoxLayout *vboxLayout5;
    QCheckBox *ModoPol;
    QHBoxLayout *hboxLayout2;
    QLabel *label_7;
    QLineEdit *SezioneCorrente;
    QToolButton *pushButtonPrec;
    QToolButton *pushButtonSucc;
    QSpacerItem *spacerItem1;
    QPushButton *pushButtonBroad;
    QHBoxLayout *hboxLayout3;
    QLabel *label_9;
    QLineEdit *intTimeLine;
    QLabel *label_10;
    QSpinBox *repeatBox;
    QCheckBox *Modo8Bit;
    QHBoxLayout *hboxLayout4;
    QGroupBox *groupBoxStatus;
    QGridLayout *gridLayout2;
    QWidget *Busy;
    QWidget *Abort;
    QWidget *Active;
    QWidget *Setting;
    QWidget *Overflow;
    QWidget *Data;
    QWidget *HW;
    QHBoxLayout *hboxLayout5;
    QSpacerItem *spacerItem2;
    QGridLayout *gridLayout3;
    QPushButton *pushButtonInt;
    QPushButton *pushButtonAbort;
    QPushButton *pushButtonStart;
    QSpacerItem *spacerItem3;
    QMenuBar *menubar;
    QMenu *menu_View;
    QMenu *menu_File;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ClientClass)
    {
    if (ClientClass->objectName().isEmpty())
        ClientClass->setObjectName(QString::fromUtf8("ClientClass"));
    ClientClass->resize(703, 379);
    action_Exit = new QAction(ClientClass);
    action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
    actionXPlot = new QAction(ClientClass);
    actionXPlot->setObjectName(QString::fromUtf8("actionXPlot"));
    centralwidget = new QWidget(ClientClass);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    gridLayout = new QGridLayout(centralwidget);
#ifndef Q_OS_MAC
    gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    gridLayout->setMargin(9);
#endif
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem, 1, 0, 1, 1);

    hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    hboxLayout->setMargin(0);
#endif
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
    vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    vboxLayout->setMargin(0);
#endif
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    groupBoxSetting = new QGroupBox(centralwidget);
    groupBoxSetting->setObjectName(QString::fromUtf8("groupBoxSetting"));
    gridLayout1 = new QGridLayout(groupBoxSetting);
#ifndef Q_OS_MAC
    gridLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    gridLayout1->setMargin(9);
#endif
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    hboxLayout1->setMargin(0);
#endif
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    vboxLayout1 = new QVBoxLayout();
#ifndef Q_OS_MAC
    vboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    vboxLayout1->setMargin(0);
#endif
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    label = new QLabel(groupBoxSetting);
    label->setObjectName(QString::fromUtf8("label"));

    vboxLayout1->addWidget(label);

    freqLine = new QLineEdit(groupBoxSetting);
    freqLine->setObjectName(QString::fromUtf8("freqLine"));

    vboxLayout1->addWidget(freqLine);


    hboxLayout1->addLayout(vboxLayout1);

    vboxLayout2 = new QVBoxLayout();
#ifndef Q_OS_MAC
    vboxLayout2->setSpacing(6);
#endif
    vboxLayout2->setMargin(0);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    label_2 = new QLabel(groupBoxSetting);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    vboxLayout2->addWidget(label_2);

    gainBox = new QSpinBox(groupBoxSetting);
    gainBox->setObjectName(QString::fromUtf8("gainBox"));
    gainBox->setMaximum(255);
    gainBox->setValue(20);

    vboxLayout2->addWidget(gainBox);


    hboxLayout1->addLayout(vboxLayout2);

    vboxLayout3 = new QVBoxLayout();
#ifndef Q_OS_MAC
    vboxLayout3->setSpacing(6);
#endif
    vboxLayout3->setMargin(0);
    vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
    label_3 = new QLabel(groupBoxSetting);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    vboxLayout3->addWidget(label_3);

    ChInBox = new QSpinBox(groupBoxSetting);
    ChInBox->setObjectName(QString::fromUtf8("ChInBox"));
    ChInBox->setMaximum(15);

    vboxLayout3->addWidget(ChInBox);


    hboxLayout1->addLayout(vboxLayout3);

    vboxLayout4 = new QVBoxLayout();
#ifndef Q_OS_MAC
    vboxLayout4->setSpacing(6);
#endif
    vboxLayout4->setMargin(0);
    vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
    label_6 = new QLabel(groupBoxSetting);
    label_6->setObjectName(QString::fromUtf8("label_6"));

    vboxLayout4->addWidget(label_6);

    bandBox = new QComboBox(groupBoxSetting);
    bandBox->setObjectName(QString::fromUtf8("bandBox"));

    vboxLayout4->addWidget(bandBox);


    hboxLayout1->addLayout(vboxLayout4);

    vboxLayout5 = new QVBoxLayout();
#ifndef Q_OS_MAC
    vboxLayout5->setSpacing(6);
#endif
    vboxLayout5->setMargin(0);
    vboxLayout5->setObjectName(QString::fromUtf8("vboxLayout5"));
    ModoPol = new QCheckBox(groupBoxSetting);
    ModoPol->setObjectName(QString::fromUtf8("ModoPol"));
    ModoPol->setChecked(true);

    vboxLayout5->addWidget(ModoPol);


    hboxLayout1->addLayout(vboxLayout5);


    gridLayout1->addLayout(hboxLayout1, 0, 0, 1, 1);

    hboxLayout2 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout2->setSpacing(6);
#endif
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    label_7 = new QLabel(groupBoxSetting);
    label_7->setObjectName(QString::fromUtf8("label_7"));

    hboxLayout2->addWidget(label_7);

    SezioneCorrente = new QLineEdit(groupBoxSetting);
    SezioneCorrente->setObjectName(QString::fromUtf8("SezioneCorrente"));
    SezioneCorrente->setMaximumSize(QSize(31, 16777215));
    SezioneCorrente->setReadOnly(true);

    hboxLayout2->addWidget(SezioneCorrente);

    pushButtonPrec = new QToolButton(groupBoxSetting);
    pushButtonPrec->setObjectName(QString::fromUtf8("pushButtonPrec"));

    hboxLayout2->addWidget(pushButtonPrec);

    pushButtonSucc = new QToolButton(groupBoxSetting);
    pushButtonSucc->setObjectName(QString::fromUtf8("pushButtonSucc"));

    hboxLayout2->addWidget(pushButtonSucc);

    spacerItem1 = new QSpacerItem(278, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem1);

    pushButtonBroad = new QPushButton(groupBoxSetting);
    pushButtonBroad->setObjectName(QString::fromUtf8("pushButtonBroad"));

    hboxLayout2->addWidget(pushButtonBroad);


    gridLayout1->addLayout(hboxLayout2, 1, 0, 1, 1);


    vboxLayout->addWidget(groupBoxSetting);

    hboxLayout3 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout3->setSpacing(6);
#endif
    hboxLayout3->setMargin(0);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    label_9 = new QLabel(centralwidget);
    label_9->setObjectName(QString::fromUtf8("label_9"));

    hboxLayout3->addWidget(label_9);

    intTimeLine = new QLineEdit(centralwidget);
    intTimeLine->setObjectName(QString::fromUtf8("intTimeLine"));

    hboxLayout3->addWidget(intTimeLine);

    label_10 = new QLabel(centralwidget);
    label_10->setObjectName(QString::fromUtf8("label_10"));

    hboxLayout3->addWidget(label_10);

    repeatBox = new QSpinBox(centralwidget);
    repeatBox->setObjectName(QString::fromUtf8("repeatBox"));
    repeatBox->setReadOnly(true);

    hboxLayout3->addWidget(repeatBox);

    Modo8Bit = new QCheckBox(centralwidget);
    Modo8Bit->setObjectName(QString::fromUtf8("Modo8Bit"));
    Modo8Bit->setEnabled(false);
    Modo8Bit->setCheckable(false);
    Modo8Bit->setChecked(false);

    hboxLayout3->addWidget(Modo8Bit);


    vboxLayout->addLayout(hboxLayout3);

    hboxLayout4 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout4->setSpacing(6);
#endif
    hboxLayout4->setMargin(0);
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));

    vboxLayout->addLayout(hboxLayout4);


    hboxLayout->addLayout(vboxLayout);

    groupBoxStatus = new QGroupBox(centralwidget);
    groupBoxStatus->setObjectName(QString::fromUtf8("groupBoxStatus"));
    groupBoxStatus->setMinimumSize(QSize(140, 210));
    gridLayout2 = new QGridLayout(groupBoxStatus);
#ifndef Q_OS_MAC
    gridLayout2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    gridLayout2->setMargin(9);
#endif
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    Busy = new QWidget(groupBoxStatus);
    Busy->setObjectName(QString::fromUtf8("Busy"));

    gridLayout2->addWidget(Busy, 6, 0, 1, 1);

    Abort = new QWidget(groupBoxStatus);
    Abort->setObjectName(QString::fromUtf8("Abort"));

    gridLayout2->addWidget(Abort, 5, 0, 1, 1);

    Active = new QWidget(groupBoxStatus);
    Active->setObjectName(QString::fromUtf8("Active"));

    gridLayout2->addWidget(Active, 4, 0, 1, 1);

    Setting = new QWidget(groupBoxStatus);
    Setting->setObjectName(QString::fromUtf8("Setting"));

    gridLayout2->addWidget(Setting, 3, 0, 1, 1);

    Overflow = new QWidget(groupBoxStatus);
    Overflow->setObjectName(QString::fromUtf8("Overflow"));

    gridLayout2->addWidget(Overflow, 2, 0, 1, 1);

    Data = new QWidget(groupBoxStatus);
    Data->setObjectName(QString::fromUtf8("Data"));

    gridLayout2->addWidget(Data, 1, 0, 1, 1);

    HW = new QWidget(groupBoxStatus);
    HW->setObjectName(QString::fromUtf8("HW"));

    gridLayout2->addWidget(HW, 0, 0, 1, 1);


    hboxLayout->addWidget(groupBoxStatus);


    gridLayout->addLayout(hboxLayout, 0, 0, 1, 1);

    hboxLayout5 = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout5->setSpacing(6);
#endif
    hboxLayout5->setMargin(0);
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout5->addItem(spacerItem2);

    gridLayout3 = new QGridLayout();
#ifndef Q_OS_MAC
    gridLayout3->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    gridLayout3->setMargin(0);
#endif
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    pushButtonInt = new QPushButton(centralwidget);
    pushButtonInt->setObjectName(QString::fromUtf8("pushButtonInt"));

    gridLayout3->addWidget(pushButtonInt, 1, 0, 1, 1);

    pushButtonAbort = new QPushButton(centralwidget);
    pushButtonAbort->setObjectName(QString::fromUtf8("pushButtonAbort"));
    pushButtonAbort->setEnabled(false);

    gridLayout3->addWidget(pushButtonAbort, 1, 1, 1, 1);

    pushButtonStart = new QPushButton(centralwidget);
    pushButtonStart->setObjectName(QString::fromUtf8("pushButtonStart"));
    pushButtonStart->setEnabled(false);

    gridLayout3->addWidget(pushButtonStart, 0, 1, 1, 1);


    hboxLayout5->addLayout(gridLayout3);

    spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout5->addItem(spacerItem3);


    gridLayout->addLayout(hboxLayout5, 2, 0, 1, 1);

    ClientClass->setCentralWidget(centralwidget);
    menubar = new QMenuBar(ClientClass);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 703, 25));
    menu_View = new QMenu(menubar);
    menu_View->setObjectName(QString::fromUtf8("menu_View"));
    menu_File = new QMenu(menubar);
    menu_File->setObjectName(QString::fromUtf8("menu_File"));
    ClientClass->setMenuBar(menubar);
    statusbar = new QStatusBar(ClientClass);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    ClientClass->setStatusBar(statusbar);

    menubar->addAction(menu_File->menuAction());
    menubar->addAction(menu_View->menuAction());
    menu_View->addAction(actionXPlot);
    menu_File->addAction(action_Exit);

    retranslateUi(ClientClass);

    bandBox->setCurrentIndex(1);


    QMetaObject::connectSlotsByName(ClientClass);
    } // setupUi

    void retranslateUi(QMainWindow *ClientClass)
    {
    ClientClass->setWindowTitle(QApplication::translate("ClientClass", "Xarcos Client", 0, QApplication::UnicodeUTF8));
    action_Exit->setText(QApplication::translate("ClientClass", "E&xit", 0, QApplication::UnicodeUTF8));
    actionXPlot->setText(QApplication::translate("ClientClass", "X&Plot", 0, QApplication::UnicodeUTF8));
    groupBoxSetting->setTitle(QApplication::translate("ClientClass", "Setting", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("ClientClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Central frequency (MHz)</p></body></html>", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("ClientClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:9pt;\">Gain</p></body></html>", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("ClientClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:9pt;\">ChIn</p></body></html>", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("ClientClass", "Band", 0, QApplication::UnicodeUTF8));
    bandBox->clear();
    bandBox->insertItems(0, QStringList()
     << QApplication::translate("ClientClass", "125.", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("ClientClass", "62.5", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("ClientClass", "31.2", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("ClientClass", "15.6", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("ClientClass", "7.81", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("ClientClass", "3.91", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("ClientClass", "1.95", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("ClientClass", ".976", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("ClientClass", ".488", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("ClientClass", "0", 0, QApplication::UnicodeUTF8)
    );
    ModoPol->setText(QApplication::translate("ClientClass", " Polarimetric Mode", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("ClientClass", "N Section", 0, QApplication::UnicodeUTF8));
    pushButtonPrec->setText(QApplication::translate("ClientClass", "<<", 0, QApplication::UnicodeUTF8));
    pushButtonSucc->setText(QApplication::translate("ClientClass", ">>", 0, QApplication::UnicodeUTF8));
    pushButtonBroad->setText(QApplication::translate("ClientClass", "Broadcast", 0, QApplication::UnicodeUTF8));
    label_9->setText(QApplication::translate("ClientClass", "Int.time [min]", 0, QApplication::UnicodeUTF8));
    label_10->setText(QApplication::translate("ClientClass", "Repeat", 0, QApplication::UnicodeUTF8));
    Modo8Bit->setText(QApplication::translate("ClientClass", "Quantization 8 Bit", 0, QApplication::UnicodeUTF8));
    groupBoxStatus->setTitle(QApplication::translate("ClientClass", "Status", 0, QApplication::UnicodeUTF8));
    pushButtonInt->setText(QApplication::translate("ClientClass", "Integration", 0, QApplication::UnicodeUTF8));
    pushButtonAbort->setText(QApplication::translate("ClientClass", "Abort", 0, QApplication::UnicodeUTF8));
    pushButtonStart->setText(QApplication::translate("ClientClass", "Start", 0, QApplication::UnicodeUTF8));
    menu_View->setTitle(QApplication::translate("ClientClass", "&View", 0, QApplication::UnicodeUTF8));
    menu_File->setTitle(QApplication::translate("ClientClass", "&File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ClientClass: public Ui_ClientClass {};
} // namespace Ui

#endif // XCLIENTGUI_H
