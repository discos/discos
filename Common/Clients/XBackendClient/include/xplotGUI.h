/********************************************************************************
** Form generated from reading ui file 'xplotGUI.ui'
**
** Created: Fri May 21 18:12:31 2010
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef XPLOTGUI_H
#define XPLOTGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_XplotGUIClass
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout1;
    QWidget *mywidget;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout2;
    QVBoxLayout *vboxLayout1;
    QGridLayout *gridLayout3;
    QLabel *label;
    QLineEdit *Xx;
    QLabel *label_2;
    QLineEdit *Yy;
    QCheckBox *checkOverFlow;
    QHBoxLayout *hboxLayout1;
    QLabel *label_8;
    QLineEdit *SezioneCorrente;
    QToolButton *pushButtonPrec;
    QToolButton *pushButtonSucc;
    QSpacerItem *spacerItem;
    QPushButton *pushButtonZoom;

    void setupUi(QWidget *XplotGUIClass)
    {
    if (XplotGUIClass->objectName().isEmpty())
        XplotGUIClass->setObjectName(QString::fromUtf8("XplotGUIClass"));
    XplotGUIClass->resize(738, 360);
    gridLayout = new QGridLayout(XplotGUIClass);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(11);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(11);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    groupBox = new QGroupBox(XplotGUIClass);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout1 = new QGridLayout(groupBox);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    mywidget = new QWidget(groupBox);
    mywidget->setObjectName(QString::fromUtf8("mywidget"));

    gridLayout1->addWidget(mywidget, 0, 0, 1, 1);


    hboxLayout->addWidget(groupBox);

    groupBox_2 = new QGroupBox(XplotGUIClass);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    groupBox_2->setMinimumSize(QSize(165, 0));
    groupBox_2->setMaximumSize(QSize(130, 16777215));
    gridLayout2 = new QGridLayout(groupBox_2);
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(11);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(11);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    gridLayout3 = new QGridLayout();
    gridLayout3->setSpacing(6);
    gridLayout3->setMargin(11);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    label = new QLabel(groupBox_2);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout3->addWidget(label, 0, 0, 1, 1);

    Xx = new QLineEdit(groupBox_2);
    Xx->setObjectName(QString::fromUtf8("Xx"));
    Xx->setMinimumSize(QSize(66, 0));
    Xx->setMaximumSize(QSize(100, 16777215));
    Xx->setReadOnly(true);

    gridLayout3->addWidget(Xx, 0, 1, 1, 1);

    label_2 = new QLabel(groupBox_2);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout3->addWidget(label_2, 1, 0, 1, 1);

    Yy = new QLineEdit(groupBox_2);
    Yy->setObjectName(QString::fromUtf8("Yy"));
    Yy->setMinimumSize(QSize(66, 0));
    Yy->setMaximumSize(QSize(100, 16777215));
    Yy->setReadOnly(true);

    gridLayout3->addWidget(Yy, 1, 1, 1, 1);


    vboxLayout1->addLayout(gridLayout3);

    checkOverFlow = new QCheckBox(groupBox_2);
    checkOverFlow->setObjectName(QString::fromUtf8("checkOverFlow"));

    vboxLayout1->addWidget(checkOverFlow);


    gridLayout2->addLayout(vboxLayout1, 0, 0, 1, 1);


    hboxLayout->addWidget(groupBox_2);


    vboxLayout->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(11);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    label_8 = new QLabel(XplotGUIClass);
    label_8->setObjectName(QString::fromUtf8("label_8"));

    hboxLayout1->addWidget(label_8);

    SezioneCorrente = new QLineEdit(XplotGUIClass);
    SezioneCorrente->setObjectName(QString::fromUtf8("SezioneCorrente"));
    SezioneCorrente->setMaximumSize(QSize(31, 16777215));
    SezioneCorrente->setReadOnly(true);

    hboxLayout1->addWidget(SezioneCorrente);

    pushButtonPrec = new QToolButton(XplotGUIClass);
    pushButtonPrec->setObjectName(QString::fromUtf8("pushButtonPrec"));

    hboxLayout1->addWidget(pushButtonPrec);

    pushButtonSucc = new QToolButton(XplotGUIClass);
    pushButtonSucc->setObjectName(QString::fromUtf8("pushButtonSucc"));

    hboxLayout1->addWidget(pushButtonSucc);

    spacerItem = new QSpacerItem(428, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    pushButtonZoom = new QPushButton(XplotGUIClass);
    pushButtonZoom->setObjectName(QString::fromUtf8("pushButtonZoom"));

    hboxLayout1->addWidget(pushButtonZoom);


    vboxLayout->addLayout(hboxLayout1);


    gridLayout->addLayout(vboxLayout, 0, 0, 1, 1);


    retranslateUi(XplotGUIClass);

    QMetaObject::connectSlotsByName(XplotGUIClass);
    } // setupUi

    void retranslateUi(QWidget *XplotGUIClass)
    {
    XplotGUIClass->setWindowTitle(QApplication::translate("XplotGUIClass", "Xarcos Plot", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("XplotGUIClass", "Plot region", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("XplotGUIClass", "Measure Tp", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("XplotGUIClass", "Xx", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("XplotGUIClass", "Yy", 0, QApplication::UnicodeUTF8));
    checkOverFlow->setText(QApplication::translate("XplotGUIClass", "Overflow", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("XplotGUIClass", "N Section", 0, QApplication::UnicodeUTF8));
    pushButtonPrec->setText(QApplication::translate("XplotGUIClass", "<<", 0, QApplication::UnicodeUTF8));
    pushButtonSucc->setText(QApplication::translate("XplotGUIClass", ">>", 0, QApplication::UnicodeUTF8));
    pushButtonZoom->setText(QApplication::translate("XplotGUIClass", "Zoom", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(XplotGUIClass);
    } // retranslateUi

};

namespace Ui {
    class XplotGUIClass: public Ui_XplotGUIClass {};
} // namespace Ui

#endif // XPLOTGUI_H
