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
#include <SRTActiveSurfaceEventLoop.h>
#include <SRTActiveSurfaceCore.h>

extern eventLoop* myeventLoop;
extern SRTActiveSurfaceCore* mySRTActiveSurfaceCore;

int passwordcheck = 0;
int thecircle, theactuator, theradius;

void SRTActiveSurfaceGUI::Quit()
{
    ACS_SHORT_LOG((LM_INFO,"SRTActiveSurfaceGUI:: Quit"));
    mySRTActiveSurfaceCore->monitor = false;
    ACE_OS::sleep (2);
    mySRTActiveSurfaceCore->terminate();
    ACE_OS::sleep (2);
    myeventLoop->stop();
    ACE_OS::sleep (2);
    myeventLoop->terminate();
    ACE_OS::sleep (2);
    close();
}

// ALL ACTUATORS
void SRTActiveSurfaceGUI::setallactuators()
{
    mySRTActiveSurfaceCore->setallactuators();
    thecircle = 0; theactuator = 0; theradius = 0;
}

// RADIUS
void SRTActiveSurfaceGUI::setradius1()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 1);
    thecircle = 0; theactuator = 0; theradius = 1;
}
void SRTActiveSurfaceGUI::setradius2()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 2);
    thecircle = 0; theactuator = 0; theradius = 2;
}
void SRTActiveSurfaceGUI::setradius3()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 3);
    thecircle = 0; theactuator = 0; theradius = 3;
}
void SRTActiveSurfaceGUI::setradius4()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 4);
    thecircle = 0; theactuator = 0; theradius = 4;
}
void SRTActiveSurfaceGUI::setradius5()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 5);
    thecircle = 0; theactuator = 0; theradius = 5;
}
void SRTActiveSurfaceGUI::setradius6()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 6);
    thecircle = 0; theactuator = 0; theradius = 6;
}
void SRTActiveSurfaceGUI::setradius7()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 7);
    thecircle = 0; theactuator = 0; theradius = 7;
}
void SRTActiveSurfaceGUI::setradius8()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 8);
    thecircle = 0; theactuator = 0; theradius = 8;
}
void SRTActiveSurfaceGUI::setradius9()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 9);
    thecircle = 0; theactuator = 0; theradius = 9;
}
void SRTActiveSurfaceGUI::setradius10()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 10);
    thecircle = 0; theactuator = 0; theradius = 10;
}
void SRTActiveSurfaceGUI::setradius11()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 11);
    thecircle = 0; theactuator = 0; theradius = 11;
}
void SRTActiveSurfaceGUI::setradius12()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 12);
    thecircle = 0; theactuator = 0; theradius = 12;
}
void SRTActiveSurfaceGUI::setradius13()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 13);
    thecircle = 0; theactuator = 0; theradius = 13;
}
void SRTActiveSurfaceGUI::setradius14()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 14);
    thecircle = 0; theactuator = 0; theradius = 14;
}
void SRTActiveSurfaceGUI::setradius15()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 15);
    thecircle = 0; theactuator = 0; theradius = 15;
}
void SRTActiveSurfaceGUI::setradius16()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 16);
    thecircle = 0; theactuator = 0; theradius = 16;
}
void SRTActiveSurfaceGUI::setradius17()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 17);
    thecircle = 0; theactuator = 0; theradius = 17;
}
void SRTActiveSurfaceGUI::setradius18()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 18);
    thecircle = 0; theactuator = 0; theradius = 18;
}
void SRTActiveSurfaceGUI::setradius19()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 19);
    thecircle = 0; theactuator = 0; theradius = 19;
}
void SRTActiveSurfaceGUI::setradius20()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 20);
    thecircle = 0; theactuator = 0; theradius = 20;
}
void SRTActiveSurfaceGUI::setradius21()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 21);
    thecircle = 0; theactuator = 0; theradius = 21;
}
void SRTActiveSurfaceGUI::setradius22()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 22);
    thecircle = 0; theactuator = 0; theradius = 22;
}
void SRTActiveSurfaceGUI::setradius23()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 23);
    thecircle = 0; theactuator = 0; theradius = 23;
}
void SRTActiveSurfaceGUI::setradius24()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 24);
    thecircle = 0; theactuator = 0; theradius = 24;
}
void SRTActiveSurfaceGUI::setradius25()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 25);
    thecircle = 0; theactuator = 0; theradius = 25;
}
void SRTActiveSurfaceGUI::setradius26()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 26);
    thecircle = 0; theactuator = 0; theradius = 26;
}
void SRTActiveSurfaceGUI::setradius27()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 27);
    thecircle = 0; theactuator = 0; theradius = 27;
}
void SRTActiveSurfaceGUI::setradius28()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 28);
    thecircle = 0; theactuator = 0; theradius = 28;
}
void SRTActiveSurfaceGUI::setradius29()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 29);
    thecircle = 0; theactuator = 0; theradius = 29;
}
void SRTActiveSurfaceGUI::setradius30()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 30);
    thecircle = 0; theactuator = 0; theradius = 30;
}
void SRTActiveSurfaceGUI::setradius31()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 31);
    thecircle = 0; theactuator = 0; theradius = 31;
}
void SRTActiveSurfaceGUI::setradius32()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 32);
    thecircle = 0; theactuator = 0; theradius = 32;
}
void SRTActiveSurfaceGUI::setradius33()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 33);
    thecircle = 0; theactuator = 0; theradius = 33;
}
void SRTActiveSurfaceGUI::setradius34()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 34);
    thecircle = 0; theactuator = 0; theradius = 34;
}
void SRTActiveSurfaceGUI::setradius35()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 35);
    thecircle = 0; theactuator = 0; theradius = 35;
}
void SRTActiveSurfaceGUI::setradius36()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 36);
    thecircle = 0; theactuator = 0; theradius = 36;
}
void SRTActiveSurfaceGUI::setradius37()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 37);
    thecircle = 0; theactuator = 0; theradius = 37;
}
void SRTActiveSurfaceGUI::setradius38()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 38);
    thecircle = 0; theactuator = 0; theradius = 38;
}
void SRTActiveSurfaceGUI::setradius39()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 39);
    thecircle = 0; theactuator = 0; theradius = 39;
}
void SRTActiveSurfaceGUI::setradius40()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 40);
    thecircle = 0; theactuator = 0; theradius = 40;
}
void SRTActiveSurfaceGUI::setradius41()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 41);
    thecircle = 0; theactuator = 0; theradius = 41;
}
void SRTActiveSurfaceGUI::setradius42()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 42);
    thecircle = 0; theactuator = 0; theradius = 42;
}
void SRTActiveSurfaceGUI::setradius43()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 43);
    thecircle = 0; theactuator = 0; theradius = 43;
}
void SRTActiveSurfaceGUI::setradius44()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 44);
    thecircle = 0; theactuator = 0; theradius = 44;
}
void SRTActiveSurfaceGUI::setradius45()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 45);
    thecircle = 0; theactuator = 0; theradius = 45;
}
void SRTActiveSurfaceGUI::setradius46()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 46);
    thecircle = 0; theactuator = 0; theradius = 46;
}
void SRTActiveSurfaceGUI::setradius47()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 47);
    thecircle = 0; theactuator = 0; theradius = 47;
}
void SRTActiveSurfaceGUI::setradius48()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 48);
    thecircle = 0; theactuator = 0; theradius = 48;
}
void SRTActiveSurfaceGUI::setradius49()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 49);
    thecircle = 0; theactuator = 0; theradius = 49;
}
void SRTActiveSurfaceGUI::setradius50()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 50);
    thecircle = 0; theactuator = 0; theradius = 50;
}
void SRTActiveSurfaceGUI::setradius51()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 51);
    thecircle = 0; theactuator = 0; theradius = 51;
}
void SRTActiveSurfaceGUI::setradius52()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 52);
    thecircle = 0; theactuator = 0; theradius = 52;
}
void SRTActiveSurfaceGUI::setradius53()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 53);
    thecircle = 0; theactuator = 0; theradius = 53;
}
void SRTActiveSurfaceGUI::setradius54()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 54);
    thecircle = 0; theactuator = 0; theradius = 54;
}
void SRTActiveSurfaceGUI::setradius55()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 55);
    thecircle = 0; theactuator = 0; theradius = 55;
}
void SRTActiveSurfaceGUI::setradius56()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 56);
    thecircle = 0; theactuator = 0; theradius = 56;
}
void SRTActiveSurfaceGUI::setradius57()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 57);
    thecircle = 0; theactuator = 0; theradius = 57;
}
void SRTActiveSurfaceGUI::setradius58()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 58);
    thecircle = 0; theactuator = 0; theradius = 58;
}
void SRTActiveSurfaceGUI::setradius59()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 59);
    thecircle = 0; theactuator = 0; theradius = 59;
}
void SRTActiveSurfaceGUI::setradius60()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 60);
    thecircle = 0; theactuator = 0; theradius = 60;
}
void SRTActiveSurfaceGUI::setradius61()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 61);
    thecircle = 0; theactuator = 0; theradius = 61;
}
void SRTActiveSurfaceGUI::setradius62()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 62);
    thecircle = 0; theactuator = 0; theradius = 62;
}
void SRTActiveSurfaceGUI::setradius63()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 63);
    thecircle = 0; theactuator = 0; theradius = 63;
}
void SRTActiveSurfaceGUI::setradius64()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 64);
    thecircle = 0; theactuator = 0; theradius = 64;
}
void SRTActiveSurfaceGUI::setradius65()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 65);
    thecircle = 0; theactuator = 0; theradius = 65;
}
void SRTActiveSurfaceGUI::setradius66()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 66);
    thecircle = 0; theactuator = 0; theradius = 66;
}
void SRTActiveSurfaceGUI::setradius67()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 67);
    thecircle = 0; theactuator = 0; theradius = 67;
}
void SRTActiveSurfaceGUI::setradius68()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 68);
    thecircle = 0; theactuator = 0; theradius = 68;
}
void SRTActiveSurfaceGUI::setradius69()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 69);
    thecircle = 0; theactuator = 0; theradius = 69;
}
void SRTActiveSurfaceGUI::setradius70()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 70);
    thecircle = 0; theactuator = 0; theradius = 70;
}
void SRTActiveSurfaceGUI::setradius71()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 71);
    thecircle = 0; theactuator = 0; theradius = 71;
}
void SRTActiveSurfaceGUI::setradius72()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 72);
    thecircle = 0; theactuator = 0; theradius = 72;
}
void SRTActiveSurfaceGUI::setradius73()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 73);
    thecircle = 0; theactuator = 0; theradius = 73;
}
void SRTActiveSurfaceGUI::setradius74()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 74);
    thecircle = 0; theactuator = 0; theradius = 74;
}
void SRTActiveSurfaceGUI::setradius75()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 75);
    thecircle = 0; theactuator = 0; theradius = 75;
}
void SRTActiveSurfaceGUI::setradius76()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 76);
    thecircle = 0; theactuator = 0; theradius = 76;
}
void SRTActiveSurfaceGUI::setradius77()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 77);
    thecircle = 0; theactuator = 0; theradius = 77;
}
void SRTActiveSurfaceGUI::setradius78()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 78);
    thecircle = 0; theactuator = 0; theradius = 78;
}
void SRTActiveSurfaceGUI::setradius79()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 79);
    thecircle = 0; theactuator = 0; theradius = 79;
}
void SRTActiveSurfaceGUI::setradius80()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 80);
    thecircle = 0; theactuator = 0; theradius = 80;
}
void SRTActiveSurfaceGUI::setradius81()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 81);
    thecircle = 0; theactuator = 0; theradius = 81;
}
void SRTActiveSurfaceGUI::setradius82()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 82);
    thecircle = 0; theactuator = 0; theradius = 82;
}
void SRTActiveSurfaceGUI::setradius83()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 83);
    thecircle = 0; theactuator = 0; theradius = 83;
}
void SRTActiveSurfaceGUI::setradius84()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 84);
    thecircle = 0; theactuator = 0; theradius = 84;
}
void SRTActiveSurfaceGUI::setradius85()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 85);
    thecircle = 0; theactuator = 0; theradius = 85;
}
void SRTActiveSurfaceGUI::setradius86()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 86);
    thecircle = 0; theactuator = 0; theradius = 86;
}
void SRTActiveSurfaceGUI::setradius87()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 87);
    thecircle = 0; theactuator = 0; theradius = 87;
}
void SRTActiveSurfaceGUI::setradius88()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 88);
    thecircle = 0; theactuator = 0; theradius = 88;
}
void SRTActiveSurfaceGUI::setradius89()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 89);
    thecircle = 0; theactuator = 0; theradius = 89;
}
void SRTActiveSurfaceGUI::setradius90()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 90);
    thecircle = 0; theactuator = 0; theradius = 90;
}
void SRTActiveSurfaceGUI::setradius91()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 91);
    thecircle = 0; theactuator = 0; theradius = 91;
}
void SRTActiveSurfaceGUI::setradius92()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 92);
    thecircle = 0; theactuator = 0; theradius = 92;
}
void SRTActiveSurfaceGUI::setradius93()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 93);
    thecircle = 0; theactuator = 0; theradius = 93;
}
void SRTActiveSurfaceGUI::setradius94()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 94);
    thecircle = 0; theactuator = 0; theradius = 94;
}
void SRTActiveSurfaceGUI::setradius95()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 95);
    thecircle = 0; theactuator = 0; theradius = 95;
}
void SRTActiveSurfaceGUI::setradius96()
{
    mySRTActiveSurfaceCore->setcircleORradius(0, 96);
    thecircle = 0; theactuator = 0; theradius = 96;
}


// CIRCLES
void SRTActiveSurfaceGUI::setcircle1()
{
    mySRTActiveSurfaceCore->setcircleORradius(1, 0);
    thecircle = 1; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle2()
{
    mySRTActiveSurfaceCore->setcircleORradius(2, 0);
    thecircle = 2; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle3()
{
    mySRTActiveSurfaceCore->setcircleORradius(3, 0);
    thecircle = 3; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle4()
{
    mySRTActiveSurfaceCore->setcircleORradius(4, 0);
    thecircle = 4; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle5()
{
    mySRTActiveSurfaceCore->setcircleORradius(5, 0);
    thecircle = 5; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle6()
{
    mySRTActiveSurfaceCore->setcircleORradius(6, 0);
    thecircle = 6; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle7()
{
    mySRTActiveSurfaceCore->setcircleORradius(7, 0);
    thecircle = 7; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle8()
{
    mySRTActiveSurfaceCore->setcircleORradius(8, 0);
    thecircle = 8; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle9()
{
    mySRTActiveSurfaceCore->setcircleORradius(9, 0);
    thecircle = 9; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle10()
{
    mySRTActiveSurfaceCore->setcircleORradius(10, 0);
    thecircle = 10; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle11()
{
    mySRTActiveSurfaceCore->setcircleORradius(11, 0);
    thecircle = 11; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle12()
{
    mySRTActiveSurfaceCore->setcircleORradius(12, 0);
    thecircle = 12; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle13()
{
    mySRTActiveSurfaceCore->setcircleORradius(13, 0);
    thecircle = 13; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle14()
{
    mySRTActiveSurfaceCore->setcircleORradius(14, 0);
    thecircle = 14; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle15()
{
    mySRTActiveSurfaceCore->setcircleORradius(15, 0);
    thecircle = 15; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle16()
{
    mySRTActiveSurfaceCore->setcircleORradius(16, 0);
    thecircle = 16; theactuator = 0; theradius = 0;
}
void SRTActiveSurfaceGUI::setcircle17()
{
    mySRTActiveSurfaceCore->setcircleORradius(17, 0);
    thecircle = 17; theactuator = 0; theradius = 0;
}
// 1 CIRCLE
void SRTActiveSurfaceGUI::setactuator1_1()
{
    mySRTActiveSurfaceCore->setactuator(1, 1);
    thecircle=1; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_2()
{
    mySRTActiveSurfaceCore->setactuator(1, 2);
    thecircle=1; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_3()
{
    mySRTActiveSurfaceCore->setactuator(1, 3);
    thecircle=1; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_4()
{
    mySRTActiveSurfaceCore->setactuator(1, 4);
    thecircle=1; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_5()
{
    mySRTActiveSurfaceCore->setactuator(1, 5);
    thecircle=1; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_6()
{
    mySRTActiveSurfaceCore->setactuator(1, 6);
    thecircle=1; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_7()
{
    mySRTActiveSurfaceCore->setactuator(1, 7);
    thecircle=1; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_8()
{
    mySRTActiveSurfaceCore->setactuator(1, 8);
    thecircle=1; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_9()
{
    mySRTActiveSurfaceCore->setactuator(1, 9);
    thecircle=1; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_10()
{
    mySRTActiveSurfaceCore->setactuator(1, 10);
    thecircle=1; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_11()
{
    mySRTActiveSurfaceCore->setactuator(1, 11);
    thecircle=1; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_12()
{
    mySRTActiveSurfaceCore->setactuator(1, 12);
    thecircle=1; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_13()
{
    mySRTActiveSurfaceCore->setactuator(1, 13);
    thecircle=1; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_14()
{
    mySRTActiveSurfaceCore->setactuator(1, 14);
    thecircle=1; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_15()
{
    mySRTActiveSurfaceCore->setactuator(1, 15);
    thecircle=1; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_16()
{
    mySRTActiveSurfaceCore->setactuator(1, 16);
    thecircle=1; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_17()
{
    mySRTActiveSurfaceCore->setactuator(1, 17);
    thecircle=1; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_18()
{
    mySRTActiveSurfaceCore->setactuator(1, 18);
    thecircle=1; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_19()
{
    mySRTActiveSurfaceCore->setactuator(1, 19);
    thecircle=1; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_20()
{
    mySRTActiveSurfaceCore->setactuator(1, 20);
    thecircle=1; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_21()
{
    mySRTActiveSurfaceCore->setactuator(1, 21);
    thecircle=1; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_22()
{
    mySRTActiveSurfaceCore->setactuator(1, 22);
    thecircle=1; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_23()
{
    mySRTActiveSurfaceCore->setactuator(1, 23);
    thecircle=1; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator1_24()
{
    mySRTActiveSurfaceCore->setactuator(1, 24);
    thecircle=1; theactuator=24; theradius=0;
}

// 2 CIRCLE
void SRTActiveSurfaceGUI::setactuator2_1()
{
    mySRTActiveSurfaceCore->setactuator(2, 1);
    thecircle=2; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_2()
{
    mySRTActiveSurfaceCore->setactuator(2, 2);
    thecircle=2; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_3()
{
    mySRTActiveSurfaceCore->setactuator(2, 3);
    thecircle=2; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_4()
{
    mySRTActiveSurfaceCore->setactuator(2, 4);
    thecircle=2; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_5()
{
    mySRTActiveSurfaceCore->setactuator(2, 5);
    thecircle=2; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_6()
{
    mySRTActiveSurfaceCore->setactuator(2, 6);
    thecircle=2; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_7()
{
    mySRTActiveSurfaceCore->setactuator(2, 7);
    thecircle=2; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_8()
{
    mySRTActiveSurfaceCore->setactuator(2, 8);
    thecircle=2; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_9()
{
    mySRTActiveSurfaceCore->setactuator(2, 9);
    thecircle=2; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_10()
{
    mySRTActiveSurfaceCore->setactuator(2, 10);
    thecircle=2; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_11()
{
    mySRTActiveSurfaceCore->setactuator(2, 11);
    thecircle=2; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_12()
{
    mySRTActiveSurfaceCore->setactuator(2, 12);
    thecircle=2; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_13()
{
    mySRTActiveSurfaceCore->setactuator(2, 13);
    thecircle=2; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_14()
{
    mySRTActiveSurfaceCore->setactuator(2, 14);
    thecircle=2; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_15()
{
    mySRTActiveSurfaceCore->setactuator(2, 15);
    thecircle=2; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_16()
{
    mySRTActiveSurfaceCore->setactuator(2, 16);
    thecircle=2; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_17()
{
    mySRTActiveSurfaceCore->setactuator(2, 17);
    thecircle=2; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_18()
{
    mySRTActiveSurfaceCore->setactuator(2, 18);
    thecircle=2; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_19()
{
    mySRTActiveSurfaceCore->setactuator(2, 19);
    thecircle=2; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_20()
{
    mySRTActiveSurfaceCore->setactuator(2, 20);
    thecircle=2; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_21()
{
    mySRTActiveSurfaceCore->setactuator(2, 21);
    thecircle=2; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_22()
{
    mySRTActiveSurfaceCore->setactuator(2, 22);
    thecircle=2; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_23()
{
    mySRTActiveSurfaceCore->setactuator(2, 23);
    thecircle=2; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator2_24()
{
    mySRTActiveSurfaceCore->setactuator(2, 24);
    thecircle=2; theactuator=24; theradius=0;
}

// 3 CIRCLE
void SRTActiveSurfaceGUI::setactuator3_1()
{
    mySRTActiveSurfaceCore->setactuator(3, 1);
    thecircle=3; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_2()
{
    mySRTActiveSurfaceCore->setactuator(3, 2);
    thecircle=3; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_3()
{
    mySRTActiveSurfaceCore->setactuator(3, 3);
    thecircle=3; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_4()
{
    mySRTActiveSurfaceCore->setactuator(3, 4);
    thecircle=3; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_5()
{
    mySRTActiveSurfaceCore->setactuator(3, 5);
    thecircle=3; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_6()
{
    mySRTActiveSurfaceCore->setactuator(3, 6);
    thecircle=3; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_7()
{
    mySRTActiveSurfaceCore->setactuator(3, 7);
    thecircle=3; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_8()
{
    mySRTActiveSurfaceCore->setactuator(3, 8);
    thecircle=3; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_9()
{
   mySRTActiveSurfaceCore->setactuator(3, 9);
   thecircle=3; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_10()
{
    mySRTActiveSurfaceCore->setactuator(3, 10);
    thecircle=3; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_11()
{
    mySRTActiveSurfaceCore->setactuator(3, 11);
    thecircle=3; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_12()
{
    mySRTActiveSurfaceCore->setactuator(3, 12);
    thecircle=3; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_13()
{
    mySRTActiveSurfaceCore->setactuator(3, 13);
    thecircle=3; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_14()
{
    mySRTActiveSurfaceCore->setactuator(3, 14);
    thecircle=3; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_15()
{
    mySRTActiveSurfaceCore->setactuator(3, 15);
    thecircle=3; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_16()
{
    mySRTActiveSurfaceCore->setactuator(3, 16);
    thecircle=3; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_17()
{
    mySRTActiveSurfaceCore->setactuator(3, 17);
    thecircle=3; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_18()
{
    mySRTActiveSurfaceCore->setactuator(3, 18);
    thecircle=3; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_19()
{
    mySRTActiveSurfaceCore->setactuator(3, 19);
    thecircle=3; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_20()
{
    mySRTActiveSurfaceCore->setactuator(3, 20);
    thecircle=3; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_21()
{
    mySRTActiveSurfaceCore->setactuator(3, 21);
    thecircle=3; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_22()
{
    mySRTActiveSurfaceCore->setactuator(3, 22);
    thecircle=3; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_23()
{
    mySRTActiveSurfaceCore->setactuator(3, 23);
    thecircle=3; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_24()
{
    mySRTActiveSurfaceCore->setactuator(3, 24);
    thecircle=3; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_25()
{
    mySRTActiveSurfaceCore->setactuator(3, 25);
    thecircle=3; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_26()
{
    mySRTActiveSurfaceCore->setactuator(3, 26);
    thecircle=3; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_27()
{
    mySRTActiveSurfaceCore->setactuator(3, 27);
    thecircle=3; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_28()
{
    mySRTActiveSurfaceCore->setactuator(3, 28);
    thecircle=3; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_29()
{
    mySRTActiveSurfaceCore->setactuator(3, 29);
    thecircle=3; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_30()
{
    mySRTActiveSurfaceCore->setactuator(3, 30);
    thecircle=3; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_31()
{
    mySRTActiveSurfaceCore->setactuator(3, 31);
    thecircle=3; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_32()
{
    mySRTActiveSurfaceCore->setactuator(3, 32);
    thecircle=3; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_33()
{
    mySRTActiveSurfaceCore->setactuator(3, 33);
    thecircle=3; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_34()
{
    mySRTActiveSurfaceCore->setactuator(3, 34);
    thecircle=3; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_35()
{
    mySRTActiveSurfaceCore->setactuator(3, 35);
    thecircle=3; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_36()
{
    mySRTActiveSurfaceCore->setactuator(3, 36);
    thecircle=3; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_37()
{
    mySRTActiveSurfaceCore->setactuator(3, 37);
    thecircle=3; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_38()
{
    mySRTActiveSurfaceCore->setactuator(3, 38);
    thecircle=3; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_39()
{
    mySRTActiveSurfaceCore->setactuator(3, 39);
    thecircle=3; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_40()
{
    mySRTActiveSurfaceCore->setactuator(3, 40);
    thecircle=3; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_41()
{
    mySRTActiveSurfaceCore->setactuator(3, 41);
    thecircle=3; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_42()
{
    mySRTActiveSurfaceCore->setactuator(3, 42);
    thecircle=3; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_43()
{
    mySRTActiveSurfaceCore->setactuator(3, 43);
    thecircle=3; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_44()
{
    mySRTActiveSurfaceCore->setactuator(3, 44);
    thecircle=3; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_45()
{
    mySRTActiveSurfaceCore->setactuator(3, 45);
    thecircle=3; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_46()
{
    mySRTActiveSurfaceCore->setactuator(3, 46);
    thecircle=3; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_47()
{
    mySRTActiveSurfaceCore->setactuator(3, 47);
    thecircle=3; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator3_48()
{
    mySRTActiveSurfaceCore->setactuator(3, 48);
    thecircle=3; theactuator=48; theradius=0;
}

// 4 CIRCLE
void SRTActiveSurfaceGUI::setactuator4_1()
{
    mySRTActiveSurfaceCore->setactuator(4, 1);
    thecircle=4; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_2()
{
    mySRTActiveSurfaceCore->setactuator(4, 2);
    thecircle=4; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_3()
{
    mySRTActiveSurfaceCore->setactuator(4, 3);
    thecircle=4; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_4()
{
    mySRTActiveSurfaceCore->setactuator(4, 4);
    thecircle=4; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_5()
{
    mySRTActiveSurfaceCore->setactuator(4, 5);
    thecircle=4; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_6()
{
    mySRTActiveSurfaceCore->setactuator(4, 6);
    thecircle=4; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_7()
{
    mySRTActiveSurfaceCore->setactuator(4, 7);
    thecircle=4; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_8()
{
    mySRTActiveSurfaceCore->setactuator(4, 8);
    thecircle=4; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_9()
{
   mySRTActiveSurfaceCore->setactuator(4, 9);
   thecircle=4; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_10()
{
    mySRTActiveSurfaceCore->setactuator(4, 10);
    thecircle=4; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_11()
{
    mySRTActiveSurfaceCore->setactuator(4, 11);
    thecircle=4; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_12()
{
    mySRTActiveSurfaceCore->setactuator(4, 12);
    thecircle=4; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_13()
{
    mySRTActiveSurfaceCore->setactuator(4, 13);
    thecircle=4; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_14()
{
    mySRTActiveSurfaceCore->setactuator(4, 14);
    thecircle=4; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_15()
{
    mySRTActiveSurfaceCore->setactuator(4, 15);
    thecircle=4; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_16()
{
    mySRTActiveSurfaceCore->setactuator(4, 16);
    thecircle=4; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_17()
{
    mySRTActiveSurfaceCore->setactuator(4, 17);
    thecircle=4; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_18()
{
    mySRTActiveSurfaceCore->setactuator(4, 18);
    thecircle=4; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_19()
{
    mySRTActiveSurfaceCore->setactuator(4, 19);
    thecircle=4; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_20()
{
    mySRTActiveSurfaceCore->setactuator(4, 20);
    thecircle=4; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_21()
{
    mySRTActiveSurfaceCore->setactuator(4, 21);
    thecircle=4; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_22()
{
    mySRTActiveSurfaceCore->setactuator(4, 22);
    thecircle=4; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_23()
{
    mySRTActiveSurfaceCore->setactuator(4, 23);
    thecircle=4; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_24()
{
    mySRTActiveSurfaceCore->setactuator(4, 24);
    thecircle=4; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_25()
{
    mySRTActiveSurfaceCore->setactuator(4, 25);
    thecircle=4; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_26()
{
    mySRTActiveSurfaceCore->setactuator(4, 26);
    thecircle=4; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_27()
{
    mySRTActiveSurfaceCore->setactuator(4, 27);
    thecircle=4; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_28()
{
    mySRTActiveSurfaceCore->setactuator(4, 28);
    thecircle=4; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_29()
{
    mySRTActiveSurfaceCore->setactuator(4, 29);
    thecircle=4; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_30()
{
    mySRTActiveSurfaceCore->setactuator(4, 30);
    thecircle=4; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_31()
{
    mySRTActiveSurfaceCore->setactuator(4, 31);
    thecircle=4; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_32()
{
    mySRTActiveSurfaceCore->setactuator(4, 32);
    thecircle=4; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_33()
{
    mySRTActiveSurfaceCore->setactuator(4, 33);
    thecircle=4; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_34()
{
    mySRTActiveSurfaceCore->setactuator(4, 34);
    thecircle=4; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_35()
{
    mySRTActiveSurfaceCore->setactuator(4, 35);
    thecircle=4; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_36()
{
    mySRTActiveSurfaceCore->setactuator(4, 36);
    thecircle=4; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_37()
{
    mySRTActiveSurfaceCore->setactuator(4, 37);
    thecircle=4; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_38()
{
    mySRTActiveSurfaceCore->setactuator(4, 38);
    thecircle=4; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_39()
{
    mySRTActiveSurfaceCore->setactuator(4, 39);
    thecircle=4; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_40()
{
    mySRTActiveSurfaceCore->setactuator(4, 40);
    thecircle=4; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_41()
{
    mySRTActiveSurfaceCore->setactuator(4, 41);
    thecircle=4; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_42()
{
    mySRTActiveSurfaceCore->setactuator(4, 42);
    thecircle=4; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_43()
{
    mySRTActiveSurfaceCore->setactuator(4, 43);
    thecircle=4; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_44()
{
    mySRTActiveSurfaceCore->setactuator(4, 44);
    thecircle=4; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_45()
{
    mySRTActiveSurfaceCore->setactuator(4, 45);
    thecircle=4; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_46()
{
    mySRTActiveSurfaceCore->setactuator(4, 46);
    thecircle=4; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_47()
{
    mySRTActiveSurfaceCore->setactuator(4, 47);
    thecircle=4; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator4_48()
{
    mySRTActiveSurfaceCore->setactuator(4, 48);
    thecircle=4; theactuator=48; theradius=0;
}
// 5 CIRCLE
void SRTActiveSurfaceGUI::setactuator5_1()
{
    mySRTActiveSurfaceCore->setactuator(5, 1);
    thecircle=5; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_2()
{
    mySRTActiveSurfaceCore->setactuator(5, 2);
    thecircle=5; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_3()
{
    mySRTActiveSurfaceCore->setactuator(5, 3);
    thecircle=5; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_4()
{
    mySRTActiveSurfaceCore->setactuator(5, 4);
    thecircle=5; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_5()
{
    mySRTActiveSurfaceCore->setactuator(5, 5);
    thecircle=5; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_6()
{
    mySRTActiveSurfaceCore->setactuator(5, 6);
    thecircle=5; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_7()
{
    mySRTActiveSurfaceCore->setactuator(5, 7);
    thecircle=5; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_8()
{
    mySRTActiveSurfaceCore->setactuator(5, 8);
    thecircle=5; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_9()
{
   mySRTActiveSurfaceCore->setactuator(5, 9);
   thecircle=5; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_10()
{
    mySRTActiveSurfaceCore->setactuator(5, 10);
    thecircle=5; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_11()
{
    mySRTActiveSurfaceCore->setactuator(5, 11);
    thecircle=5; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_12()
{
    mySRTActiveSurfaceCore->setactuator(5, 12);
    thecircle=5; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_13()
{
    mySRTActiveSurfaceCore->setactuator(5, 13);
    thecircle=5; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_14()
{
    mySRTActiveSurfaceCore->setactuator(5, 14);
    thecircle=5; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_15()
{
    mySRTActiveSurfaceCore->setactuator(5, 15);
    thecircle=5; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_16()
{
    mySRTActiveSurfaceCore->setactuator(5, 16);
    thecircle=5; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_17()
{
    mySRTActiveSurfaceCore->setactuator(5, 17);
    thecircle=5; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_18()
{
    mySRTActiveSurfaceCore->setactuator(5, 18);
    thecircle=5; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_19()
{
    mySRTActiveSurfaceCore->setactuator(5, 19);
    thecircle=5; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_20()
{
    mySRTActiveSurfaceCore->setactuator(5, 20);
    thecircle=5; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_21()
{
    mySRTActiveSurfaceCore->setactuator(5, 21);
    thecircle=5; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_22()
{
    mySRTActiveSurfaceCore->setactuator(5, 22);
    thecircle=5; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_23()
{
    mySRTActiveSurfaceCore->setactuator(5, 23);
    thecircle=5; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_24()
{
    mySRTActiveSurfaceCore->setactuator(5, 24);
    thecircle=5; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_25()
{
    mySRTActiveSurfaceCore->setactuator(5, 25);
    thecircle=5; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_26()
{
    mySRTActiveSurfaceCore->setactuator(5, 26);
    thecircle=5; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_27()
{
    mySRTActiveSurfaceCore->setactuator(5, 27);
    thecircle=5; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_28()
{
    mySRTActiveSurfaceCore->setactuator(5, 28);
    thecircle=5; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_29()
{
    mySRTActiveSurfaceCore->setactuator(5, 29);
    thecircle=5; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_30()
{
    mySRTActiveSurfaceCore->setactuator(5, 30);
    thecircle=5; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_31()
{
    mySRTActiveSurfaceCore->setactuator(5, 31);
    thecircle=5; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_32()
{
    mySRTActiveSurfaceCore->setactuator(5, 32);
    thecircle=5; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_33()
{
    mySRTActiveSurfaceCore->setactuator(5, 33);
    thecircle=5; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_34()
{
    mySRTActiveSurfaceCore->setactuator(5, 34);
    thecircle=5; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_35()
{
    mySRTActiveSurfaceCore->setactuator(5, 35);
    thecircle=5; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_36()
{
    mySRTActiveSurfaceCore->setactuator(5, 36);
    thecircle=5; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_37()
{
    mySRTActiveSurfaceCore->setactuator(5, 37);
    thecircle=5; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_38()
{
    mySRTActiveSurfaceCore->setactuator(5, 38);
    thecircle=5; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_39()
{
    mySRTActiveSurfaceCore->setactuator(5, 39);
    thecircle=5; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_40()
{
    mySRTActiveSurfaceCore->setactuator(5, 40);
    thecircle=5; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_41()
{
    mySRTActiveSurfaceCore->setactuator(5, 41);
    thecircle=5; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_42()
{
    mySRTActiveSurfaceCore->setactuator(5, 42);
    thecircle=5; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_43()
{
    mySRTActiveSurfaceCore->setactuator(5, 43);
    thecircle=5; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_44()
{
    mySRTActiveSurfaceCore->setactuator(5, 44);
    thecircle=5; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_45()
{
    mySRTActiveSurfaceCore->setactuator(5, 45);
    thecircle=5; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_46()
{
    mySRTActiveSurfaceCore->setactuator(5, 46);
    thecircle=5; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_47()
{
    mySRTActiveSurfaceCore->setactuator(5, 47);
    thecircle=5; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator5_48()
{
    mySRTActiveSurfaceCore->setactuator(5, 48);
    thecircle=5; theactuator=48; theradius=0;
}
// 6 CIRCLE
void SRTActiveSurfaceGUI::setactuator6_1()
{
    mySRTActiveSurfaceCore->setactuator(6, 1);
    thecircle=6; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_2()
{
    mySRTActiveSurfaceCore->setactuator(6, 2);
    thecircle=6; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_3()
{
    mySRTActiveSurfaceCore->setactuator(6, 3);
    thecircle=6; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_4()
{
    mySRTActiveSurfaceCore->setactuator(6, 4);
    thecircle=6; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_5()
{
    mySRTActiveSurfaceCore->setactuator(6, 5);
    thecircle=6; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_6()
{
    mySRTActiveSurfaceCore->setactuator(6, 6);
    thecircle=6; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_7()
{
    mySRTActiveSurfaceCore->setactuator(6, 7);
    thecircle=6; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_8()
{
    mySRTActiveSurfaceCore->setactuator(6, 8);
    thecircle=6; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_9()
{
   mySRTActiveSurfaceCore->setactuator(6, 9);
   thecircle=6; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_10()
{
    mySRTActiveSurfaceCore->setactuator(6, 10);
    thecircle=6; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_11()
{
    mySRTActiveSurfaceCore->setactuator(6, 11);
    thecircle=6; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_12()
{
    mySRTActiveSurfaceCore->setactuator(6, 12);
    thecircle=6; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_13()
{
    mySRTActiveSurfaceCore->setactuator(6, 13);
    thecircle=6; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_14()
{
    mySRTActiveSurfaceCore->setactuator(6, 14);
    thecircle=6; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_15()
{
    mySRTActiveSurfaceCore->setactuator(6, 15);
    thecircle=6; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_16()
{
    mySRTActiveSurfaceCore->setactuator(6, 16);
    thecircle=6; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_17()
{
    mySRTActiveSurfaceCore->setactuator(6, 17);
    thecircle=6; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_18()
{
    mySRTActiveSurfaceCore->setactuator(6, 18);
    thecircle=6; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_19()
{
    mySRTActiveSurfaceCore->setactuator(6, 19);
    thecircle=6; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_20()
{
    mySRTActiveSurfaceCore->setactuator(6, 20);
    thecircle=6; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_21()
{
    mySRTActiveSurfaceCore->setactuator(6, 21);
    thecircle=6; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_22()
{
    mySRTActiveSurfaceCore->setactuator(6, 22);
    thecircle=6; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_23()
{
    mySRTActiveSurfaceCore->setactuator(6, 23);
    thecircle=6; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_24()
{
    mySRTActiveSurfaceCore->setactuator(6, 24);
    thecircle=6; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_25()
{
    mySRTActiveSurfaceCore->setactuator(6, 25);
    thecircle=6; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_26()
{
    mySRTActiveSurfaceCore->setactuator(6, 26);
    thecircle=6; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_27()
{
    mySRTActiveSurfaceCore->setactuator(6, 27);
    thecircle=6; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_28()
{
    mySRTActiveSurfaceCore->setactuator(6, 28);
    thecircle=6; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_29()
{
    mySRTActiveSurfaceCore->setactuator(6, 29);
    thecircle=6; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_30()
{
    mySRTActiveSurfaceCore->setactuator(6, 30);
    thecircle=6; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_31()
{
    mySRTActiveSurfaceCore->setactuator(6, 31);
    thecircle=6; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_32()
{
    mySRTActiveSurfaceCore->setactuator(6, 32);
    thecircle=6; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_33()
{
    mySRTActiveSurfaceCore->setactuator(6, 33);
    thecircle=6; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_34()
{
    mySRTActiveSurfaceCore->setactuator(6, 34);
    thecircle=6; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_35()
{
    mySRTActiveSurfaceCore->setactuator(6, 35);
    thecircle=6; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_36()
{
    mySRTActiveSurfaceCore->setactuator(6, 36);
    thecircle=6; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_37()
{
    mySRTActiveSurfaceCore->setactuator(6, 37);
    thecircle=6; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_38()
{
    mySRTActiveSurfaceCore->setactuator(6, 38);
    thecircle=6; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_39()
{
    mySRTActiveSurfaceCore->setactuator(6, 39);
    thecircle=6; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_40()
{
    mySRTActiveSurfaceCore->setactuator(6, 40);
    thecircle=6; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_41()
{
    mySRTActiveSurfaceCore->setactuator(6, 41);
    thecircle=6; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_42()
{
    mySRTActiveSurfaceCore->setactuator(6, 42);
    thecircle=6; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_43()
{
    mySRTActiveSurfaceCore->setactuator(6, 43);
    thecircle=6; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_44()
{
    mySRTActiveSurfaceCore->setactuator(6, 44);
    thecircle=6; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_45()
{
    mySRTActiveSurfaceCore->setactuator(6, 45);
    thecircle=6; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_46()
{
    mySRTActiveSurfaceCore->setactuator(6, 46);
    thecircle=6; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_47()
{
    mySRTActiveSurfaceCore->setactuator(6, 47);
    thecircle=6; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator6_48()
{
    mySRTActiveSurfaceCore->setactuator(6, 48);
    thecircle=6; theactuator=48; theradius=0;
}
// 7 CIRCLE
void SRTActiveSurfaceGUI::setactuator7_1()
{
    mySRTActiveSurfaceCore->setactuator(7, 1);
    thecircle=7; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_2()
{
    mySRTActiveSurfaceCore->setactuator(7, 2);
    thecircle=7; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_3()
{
    mySRTActiveSurfaceCore->setactuator(7, 3);
    thecircle=7; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_4()
{
    mySRTActiveSurfaceCore->setactuator(7, 4);
    thecircle=7; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_5()
{
    mySRTActiveSurfaceCore->setactuator(7, 5);
    thecircle=7; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_6()
{
    mySRTActiveSurfaceCore->setactuator(7, 6);
    thecircle=7; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_7()
{
    mySRTActiveSurfaceCore->setactuator(7, 7);
    thecircle=7; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_8()
{
    mySRTActiveSurfaceCore->setactuator(7, 8);
    thecircle=7; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_9()
{
   mySRTActiveSurfaceCore->setactuator(7, 9);
   thecircle=7; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_10()
{
    mySRTActiveSurfaceCore->setactuator(7, 10);
    thecircle=7; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_11()
{
    mySRTActiveSurfaceCore->setactuator(7, 11);
    thecircle=7; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_12()
{
    mySRTActiveSurfaceCore->setactuator(7, 12);
    thecircle=7; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_13()
{
    mySRTActiveSurfaceCore->setactuator(7, 13);
    thecircle=7; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_14()
{
    mySRTActiveSurfaceCore->setactuator(7, 14);
    thecircle=7; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_15()
{
    mySRTActiveSurfaceCore->setactuator(7, 15);
    thecircle=7; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_16()
{
    mySRTActiveSurfaceCore->setactuator(7, 16);
    thecircle=7; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_17()
{
    mySRTActiveSurfaceCore->setactuator(7, 17);
    thecircle=7; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_18()
{
    mySRTActiveSurfaceCore->setactuator(7, 18);
    thecircle=7; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_19()
{
    mySRTActiveSurfaceCore->setactuator(7, 19);
    thecircle=7; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_20()
{
    mySRTActiveSurfaceCore->setactuator(7, 20);
    thecircle=7; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_21()
{
    mySRTActiveSurfaceCore->setactuator(7, 21);
    thecircle=7; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_22()
{
    mySRTActiveSurfaceCore->setactuator(7, 22);
    thecircle=7; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_23()
{
    mySRTActiveSurfaceCore->setactuator(7, 23);
    thecircle=7; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_24()
{
    mySRTActiveSurfaceCore->setactuator(7, 24);
    thecircle=7; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_25()
{
    mySRTActiveSurfaceCore->setactuator(7, 25);
    thecircle=7; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_26()
{
    mySRTActiveSurfaceCore->setactuator(7, 26);
    thecircle=7; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_27()
{
    mySRTActiveSurfaceCore->setactuator(7, 27);
    thecircle=7; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_28()
{
    mySRTActiveSurfaceCore->setactuator(7, 28);
    thecircle=7; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_29()
{
    mySRTActiveSurfaceCore->setactuator(7, 29);
    thecircle=7; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_30()
{
    mySRTActiveSurfaceCore->setactuator(7, 30);
    thecircle=7; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_31()
{
    mySRTActiveSurfaceCore->setactuator(7, 31);
    thecircle=7; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_32()
{
    mySRTActiveSurfaceCore->setactuator(7, 32);
    thecircle=7; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_33()
{
    mySRTActiveSurfaceCore->setactuator(7, 33);
    thecircle=7; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_34()
{
    mySRTActiveSurfaceCore->setactuator(7, 34);
    thecircle=7; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_35()
{
    mySRTActiveSurfaceCore->setactuator(7, 35);
    thecircle=7; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_36()
{
    mySRTActiveSurfaceCore->setactuator(7, 36);
    thecircle=7; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_37()
{
    mySRTActiveSurfaceCore->setactuator(7, 37);
    thecircle=7; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_38()
{
    mySRTActiveSurfaceCore->setactuator(7, 38);
    thecircle=7; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_39()
{
    mySRTActiveSurfaceCore->setactuator(7, 39);
    thecircle=7; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_40()
{
    mySRTActiveSurfaceCore->setactuator(7, 40);
    thecircle=7; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_41()
{
    mySRTActiveSurfaceCore->setactuator(7, 41);
    thecircle=7; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_42()
{
    mySRTActiveSurfaceCore->setactuator(7, 42);
    thecircle=7; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_43()
{
    mySRTActiveSurfaceCore->setactuator(7, 43);
    thecircle=7; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_44()
{
    mySRTActiveSurfaceCore->setactuator(7, 44);
    thecircle=7; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_45()
{
    mySRTActiveSurfaceCore->setactuator(7, 45);
    thecircle=7; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_46()
{
    mySRTActiveSurfaceCore->setactuator(7, 46);
    thecircle=7; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_47()
{
    mySRTActiveSurfaceCore->setactuator(7, 47);
    thecircle=7; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_48()
{
    mySRTActiveSurfaceCore->setactuator(7, 48);
    thecircle=7; theactuator=48; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_49()
{
    mySRTActiveSurfaceCore->setactuator(7, 49);
    thecircle=7; theactuator=49; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_50()
{
    mySRTActiveSurfaceCore->setactuator(7, 50);
    thecircle=7; theactuator=50; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_51()
{
    mySRTActiveSurfaceCore->setactuator(7, 51);
    thecircle=7; theactuator=51; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_52()
{
    mySRTActiveSurfaceCore->setactuator(7, 52);
    thecircle=7; theactuator=52; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_53()
{
    mySRTActiveSurfaceCore->setactuator(7, 53);
    thecircle=7; theactuator=53; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_54()
{
    mySRTActiveSurfaceCore->setactuator(7, 54);
    thecircle=7; theactuator=54; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_55()
{
    mySRTActiveSurfaceCore->setactuator(7, 55);
    thecircle=7; theactuator=55; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_56()
{
    mySRTActiveSurfaceCore->setactuator(7, 56);
    thecircle=7; theactuator=56; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_57()
{
   mySRTActiveSurfaceCore->setactuator(7, 57);
   thecircle=7; theactuator=57; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_58()
{
    mySRTActiveSurfaceCore->setactuator(7, 58);
    thecircle=7; theactuator=58; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_59()
{
    mySRTActiveSurfaceCore->setactuator(7, 59);
    thecircle=7; theactuator=59; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_60()
{
    mySRTActiveSurfaceCore->setactuator(7, 60);
    thecircle=7; theactuator=60; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_61()
{
    mySRTActiveSurfaceCore->setactuator(7, 61);
    thecircle=7; theactuator=61; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_62()
{
    mySRTActiveSurfaceCore->setactuator(7, 62);
    thecircle=7; theactuator=62; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_63()
{
    mySRTActiveSurfaceCore->setactuator(7, 63);
    thecircle=7; theactuator=63; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_64()
{
    mySRTActiveSurfaceCore->setactuator(7, 64);
    thecircle=7; theactuator=64; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_65()
{
    mySRTActiveSurfaceCore->setactuator(7, 65);
    thecircle=7; theactuator=65; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_66()
{
    mySRTActiveSurfaceCore->setactuator(7, 66);
    thecircle=7; theactuator=66; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_67()
{
    mySRTActiveSurfaceCore->setactuator(7, 67);
    thecircle=7; theactuator=67; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_68()
{
    mySRTActiveSurfaceCore->setactuator(7, 68);
    thecircle=7; theactuator=68; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_69()
{
    mySRTActiveSurfaceCore->setactuator(7, 69);
    thecircle=7; theactuator=69; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_70()
{
    mySRTActiveSurfaceCore->setactuator(7, 70);
    thecircle=7; theactuator=70; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_71()
{
    mySRTActiveSurfaceCore->setactuator(7, 71);
    thecircle=7; theactuator=71; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_72()
{
    mySRTActiveSurfaceCore->setactuator(7, 72);
    thecircle=7; theactuator=72; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_73()
{
    mySRTActiveSurfaceCore->setactuator(7, 73);
    thecircle=7; theactuator=73; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_74()
{
    mySRTActiveSurfaceCore->setactuator(7, 74);
    thecircle=7; theactuator=74; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_75()
{
    mySRTActiveSurfaceCore->setactuator(7, 75);
    thecircle=7; theactuator=75; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_76()
{
    mySRTActiveSurfaceCore->setactuator(7, 76);
    thecircle=7; theactuator=76; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_77()
{
    mySRTActiveSurfaceCore->setactuator(7, 77);
    thecircle=7; theactuator=77; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_78()
{
    mySRTActiveSurfaceCore->setactuator(7, 78);
    thecircle=7; theactuator=78; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_79()
{
    mySRTActiveSurfaceCore->setactuator(7, 79);
    thecircle=7; theactuator=79; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_80()
{
    mySRTActiveSurfaceCore->setactuator(7, 80);
    thecircle=7; theactuator=80; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_81()
{
    mySRTActiveSurfaceCore->setactuator(7, 81);
    thecircle=7; theactuator=81; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_82()
{
    mySRTActiveSurfaceCore->setactuator(7, 82);
    thecircle=7; theactuator=82; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_83()
{
    mySRTActiveSurfaceCore->setactuator(7, 83);
    thecircle=7; theactuator=83; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_84()
{
    mySRTActiveSurfaceCore->setactuator(7, 84);
    thecircle=7; theactuator=84; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_85()
{
    mySRTActiveSurfaceCore->setactuator(7, 85);
    thecircle=7; theactuator=85; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_86()
{
    mySRTActiveSurfaceCore->setactuator(7, 86);
    thecircle=7; theactuator=86; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_87()
{
    mySRTActiveSurfaceCore->setactuator(7, 87);
    thecircle=7; theactuator=87; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_88()
{
    mySRTActiveSurfaceCore->setactuator(7, 88);
    thecircle=7; theactuator=88; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_89()
{
    mySRTActiveSurfaceCore->setactuator(7, 89);
    thecircle=7; theactuator=89; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_90()
{
    mySRTActiveSurfaceCore->setactuator(7, 90);
    thecircle=7; theactuator=90; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_91()
{
    mySRTActiveSurfaceCore->setactuator(7, 91);
    thecircle=7; theactuator=91; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_92()
{
    mySRTActiveSurfaceCore->setactuator(7, 92);
    thecircle=7; theactuator=92; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_93()
{
    mySRTActiveSurfaceCore->setactuator(7, 93);
    thecircle=7; theactuator=93; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_94()
{
    mySRTActiveSurfaceCore->setactuator(7, 94);
    thecircle=7; theactuator=94; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_95()
{
    mySRTActiveSurfaceCore->setactuator(7, 95);
    thecircle=7; theactuator=95; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator7_96()
{
    mySRTActiveSurfaceCore->setactuator(7, 96);
    thecircle=7; theactuator=96; theradius=0;
}
// 8 CIRCLE
void SRTActiveSurfaceGUI::setactuator8_1()
{
    mySRTActiveSurfaceCore->setactuator(8, 1);
    thecircle=8; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_2()
{
    mySRTActiveSurfaceCore->setactuator(8, 2);
    thecircle=8; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_3()
{
    mySRTActiveSurfaceCore->setactuator(8, 3);
    thecircle=8; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_4()
{
    mySRTActiveSurfaceCore->setactuator(8, 4);
    thecircle=8; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_5()
{
    mySRTActiveSurfaceCore->setactuator(8, 5);
    thecircle=8; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_6()
{
    mySRTActiveSurfaceCore->setactuator(8, 6);
    thecircle=8; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_7()
{
    mySRTActiveSurfaceCore->setactuator(8, 7);
    thecircle=8; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_8()
{
    mySRTActiveSurfaceCore->setactuator(8, 8);
    thecircle=8; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_9()
{
   mySRTActiveSurfaceCore->setactuator(8, 9);
   thecircle=8; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_10()
{
    mySRTActiveSurfaceCore->setactuator(8, 10);
    thecircle=8; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_11()
{
    mySRTActiveSurfaceCore->setactuator(8, 11);
    thecircle=8; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_12()
{
    mySRTActiveSurfaceCore->setactuator(8, 12);
    thecircle=8; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_13()
{
    mySRTActiveSurfaceCore->setactuator(8, 13);
    thecircle=8; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_14()
{
    mySRTActiveSurfaceCore->setactuator(8, 14);
    thecircle=8; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_15()
{
    mySRTActiveSurfaceCore->setactuator(8, 15);
    thecircle=8; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_16()
{
    mySRTActiveSurfaceCore->setactuator(8, 16);
    thecircle=8; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_17()
{
    mySRTActiveSurfaceCore->setactuator(8, 17);
    thecircle=8; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_18()
{
    mySRTActiveSurfaceCore->setactuator(8, 18);
    thecircle=8; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_19()
{
    mySRTActiveSurfaceCore->setactuator(8, 19);
    thecircle=8; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_20()
{
    mySRTActiveSurfaceCore->setactuator(8, 20);
    thecircle=8; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_21()
{
    mySRTActiveSurfaceCore->setactuator(8, 21);
    thecircle=8; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_22()
{
    mySRTActiveSurfaceCore->setactuator(8, 22);
    thecircle=8; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_23()
{
    mySRTActiveSurfaceCore->setactuator(8, 23);
    thecircle=8; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_24()
{
    mySRTActiveSurfaceCore->setactuator(8, 24);
    thecircle=8; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_25()
{
    mySRTActiveSurfaceCore->setactuator(8, 25);
    thecircle=8; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_26()
{
    mySRTActiveSurfaceCore->setactuator(8, 26);
    thecircle=8; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_27()
{
    mySRTActiveSurfaceCore->setactuator(8, 27);
    thecircle=8; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_28()
{
    mySRTActiveSurfaceCore->setactuator(8, 28);
    thecircle=8; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_29()
{
    mySRTActiveSurfaceCore->setactuator(8, 29);
    thecircle=8; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_30()
{
    mySRTActiveSurfaceCore->setactuator(8, 30);
    thecircle=8; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_31()
{
    mySRTActiveSurfaceCore->setactuator(8, 31);
    thecircle=8; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_32()
{
    mySRTActiveSurfaceCore->setactuator(8, 32);
    thecircle=8; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_33()
{
    mySRTActiveSurfaceCore->setactuator(8, 33);
    thecircle=8; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_34()
{
    mySRTActiveSurfaceCore->setactuator(8, 34);
    thecircle=8; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_35()
{
    mySRTActiveSurfaceCore->setactuator(8, 35);
    thecircle=8; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_36()
{
    mySRTActiveSurfaceCore->setactuator(8, 36);
    thecircle=8; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_37()
{
    mySRTActiveSurfaceCore->setactuator(8, 37);
    thecircle=8; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_38()
{
    mySRTActiveSurfaceCore->setactuator(8, 38);
    thecircle=8; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_39()
{
    mySRTActiveSurfaceCore->setactuator(8, 39);
    thecircle=8; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_40()
{
    mySRTActiveSurfaceCore->setactuator(8, 40);
    thecircle=8; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_41()
{
    mySRTActiveSurfaceCore->setactuator(8, 41);
    thecircle=8; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_42()
{
    mySRTActiveSurfaceCore->setactuator(8, 42);
    thecircle=8; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_43()
{
    mySRTActiveSurfaceCore->setactuator(8, 43);
    thecircle=8; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_44()
{
    mySRTActiveSurfaceCore->setactuator(8, 44);
    thecircle=8; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_45()
{
    mySRTActiveSurfaceCore->setactuator(8, 45);
    thecircle=8; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_46()
{
    mySRTActiveSurfaceCore->setactuator(8, 46);
    thecircle=8; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_47()
{
    mySRTActiveSurfaceCore->setactuator(8, 47);
    thecircle=8; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_48()
{
    mySRTActiveSurfaceCore->setactuator(8, 48);
    thecircle=8; theactuator=48; theradius=0;
}
// 6 CIRCLE
void SRTActiveSurfaceGUI::setactuator8_49()
{
    mySRTActiveSurfaceCore->setactuator(8, 49);
    thecircle=8; theactuator=49; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_50()
{
    mySRTActiveSurfaceCore->setactuator(8, 50);
    thecircle=8; theactuator=50; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_51()
{
    mySRTActiveSurfaceCore->setactuator(8, 51);
    thecircle=8; theactuator=51; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_52()
{
    mySRTActiveSurfaceCore->setactuator(8, 52);
    thecircle=8; theactuator=52; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_53()
{
    mySRTActiveSurfaceCore->setactuator(8, 53);
    thecircle=8; theactuator=53; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_54()
{
    mySRTActiveSurfaceCore->setactuator(8, 54);
    thecircle=8; theactuator=54; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_55()
{
    mySRTActiveSurfaceCore->setactuator(8, 55);
    thecircle=8; theactuator=55; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_56()
{
    mySRTActiveSurfaceCore->setactuator(8, 56);
    thecircle=8; theactuator=56; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_57()
{
   mySRTActiveSurfaceCore->setactuator(8, 57);
   thecircle=8; theactuator=57; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_58()
{
    mySRTActiveSurfaceCore->setactuator(8, 58);
    thecircle=8; theactuator=58; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_59()
{
    mySRTActiveSurfaceCore->setactuator(8, 59);
    thecircle=8; theactuator=59; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_60()
{
    mySRTActiveSurfaceCore->setactuator(8, 60);
    thecircle=8; theactuator=60; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_61()
{
    mySRTActiveSurfaceCore->setactuator(8, 61);
    thecircle=8; theactuator=61; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_62()
{
    mySRTActiveSurfaceCore->setactuator(8, 62);
    thecircle=8; theactuator=62; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_63()
{
    mySRTActiveSurfaceCore->setactuator(8, 63);
    thecircle=8; theactuator=63; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_64()
{
    mySRTActiveSurfaceCore->setactuator(8, 64);
    thecircle=8; theactuator=64; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_65()
{
    mySRTActiveSurfaceCore->setactuator(8, 65);
    thecircle=8; theactuator=65; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_66()
{
    mySRTActiveSurfaceCore->setactuator(8, 66);
    thecircle=8; theactuator=66; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_67()
{
    mySRTActiveSurfaceCore->setactuator(8, 67);
    thecircle=8; theactuator=67; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_68()
{
    mySRTActiveSurfaceCore->setactuator(8, 68);
    thecircle=8; theactuator=68; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_69()
{
    mySRTActiveSurfaceCore->setactuator(8, 69);
    thecircle=8; theactuator=69; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_70()
{
    mySRTActiveSurfaceCore->setactuator(8, 70);
    thecircle=8; theactuator=70; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_71()
{
    mySRTActiveSurfaceCore->setactuator(8, 71);
    thecircle=8; theactuator=71; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_72()
{
    mySRTActiveSurfaceCore->setactuator(8, 72);
    thecircle=8; theactuator=72; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_73()
{
    mySRTActiveSurfaceCore->setactuator(8, 73);
    thecircle=8; theactuator=73; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_74()
{
    mySRTActiveSurfaceCore->setactuator(8, 74);
    thecircle=8; theactuator=74; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_75()
{
    mySRTActiveSurfaceCore->setactuator(8, 75);
    thecircle=8; theactuator=75; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_76()
{
    mySRTActiveSurfaceCore->setactuator(8, 76);
    thecircle=8; theactuator=76; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_77()
{
    mySRTActiveSurfaceCore->setactuator(8, 77);
    thecircle=8; theactuator=77; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_78()
{
    mySRTActiveSurfaceCore->setactuator(8, 78);
    thecircle=8; theactuator=78; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_79()
{
    mySRTActiveSurfaceCore->setactuator(8, 79);
    thecircle=8; theactuator=79; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_80()
{
    mySRTActiveSurfaceCore->setactuator(8, 80);
    thecircle=8; theactuator=80; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_81()
{
    mySRTActiveSurfaceCore->setactuator(8, 81);
    thecircle=8; theactuator=81; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_82()
{
    mySRTActiveSurfaceCore->setactuator(8, 82);
    thecircle=8; theactuator=82; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_83()
{
    mySRTActiveSurfaceCore->setactuator(8, 83);
    thecircle=8; theactuator=83; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_84()
{
    mySRTActiveSurfaceCore->setactuator(8, 84);
    thecircle=8; theactuator=84; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_85()
{
    mySRTActiveSurfaceCore->setactuator(8, 85);
    thecircle=8; theactuator=85; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_86()
{
    mySRTActiveSurfaceCore->setactuator(8, 86);
    thecircle=8; theactuator=86; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_87()
{
    mySRTActiveSurfaceCore->setactuator(8, 87);
    thecircle=8; theactuator=87; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_88()
{
    mySRTActiveSurfaceCore->setactuator(8, 88);
    thecircle=8; theactuator=88; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_89()
{
    mySRTActiveSurfaceCore->setactuator(8, 89);
    thecircle=8; theactuator=89; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_90()
{
    mySRTActiveSurfaceCore->setactuator(8, 90);
    thecircle=8; theactuator=90; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_91()
{
    mySRTActiveSurfaceCore->setactuator(8, 91);
    thecircle=8; theactuator=91; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_92()
{
    mySRTActiveSurfaceCore->setactuator(8, 92);
    thecircle=8; theactuator=92; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_93()
{
    mySRTActiveSurfaceCore->setactuator(8, 93);
    thecircle=8; theactuator=93; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_94()
{
    mySRTActiveSurfaceCore->setactuator(8, 94);
    thecircle=8; theactuator=94; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_95()
{
    mySRTActiveSurfaceCore->setactuator(8, 95);
    thecircle=8; theactuator=95; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator8_96()
{
    mySRTActiveSurfaceCore->setactuator(8, 96);
    thecircle=8; theactuator=96; theradius=0;
}
// 9 CIRCLE
void SRTActiveSurfaceGUI::setactuator9_1()
{
    mySRTActiveSurfaceCore->setactuator(9, 1);
    thecircle=9; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_2()
{
    mySRTActiveSurfaceCore->setactuator(9, 2);
    thecircle=9; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_3()
{
    mySRTActiveSurfaceCore->setactuator(9, 3);
    thecircle=9; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_4()
{
    mySRTActiveSurfaceCore->setactuator(9, 4);
    thecircle=9; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_5()
{
    mySRTActiveSurfaceCore->setactuator(9, 5);
    thecircle=9; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_6()
{
    mySRTActiveSurfaceCore->setactuator(9, 6);
    thecircle=9; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_7()
{
    mySRTActiveSurfaceCore->setactuator(9, 7);
    thecircle=9; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_8()
{
    mySRTActiveSurfaceCore->setactuator(9, 8);
    thecircle=9; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_9()
{
   mySRTActiveSurfaceCore->setactuator(9, 9);
   thecircle=9; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_10()
{
    mySRTActiveSurfaceCore->setactuator(9, 10);
    thecircle=9; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_11()
{
    mySRTActiveSurfaceCore->setactuator(9, 11);
    thecircle=9; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_12()
{
    mySRTActiveSurfaceCore->setactuator(9, 12);
    thecircle=9; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_13()
{
    mySRTActiveSurfaceCore->setactuator(9, 13);
    thecircle=9; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_14()
{
    mySRTActiveSurfaceCore->setactuator(9, 14);
    thecircle=9; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_15()
{
    mySRTActiveSurfaceCore->setactuator(9, 15);
    thecircle=9; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_16()
{
    mySRTActiveSurfaceCore->setactuator(9, 16);
    thecircle=9; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_17()
{
    mySRTActiveSurfaceCore->setactuator(9, 17);
    thecircle=9; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_18()
{
    mySRTActiveSurfaceCore->setactuator(9, 18);
    thecircle=9; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_19()
{
    mySRTActiveSurfaceCore->setactuator(9, 19);
    thecircle=9; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_20()
{
    mySRTActiveSurfaceCore->setactuator(9, 20);
    thecircle=9; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_21()
{
    mySRTActiveSurfaceCore->setactuator(9, 21);
    thecircle=9; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_22()
{
    mySRTActiveSurfaceCore->setactuator(9, 22);
    thecircle=9; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_23()
{
    mySRTActiveSurfaceCore->setactuator(9, 23);
    thecircle=9; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_24()
{
    mySRTActiveSurfaceCore->setactuator(9, 24);
    thecircle=9; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_25()
{
    mySRTActiveSurfaceCore->setactuator(9, 25);
    thecircle=9; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_26()
{
    mySRTActiveSurfaceCore->setactuator(9, 26);
    thecircle=9; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_27()
{
    mySRTActiveSurfaceCore->setactuator(9, 27);
    thecircle=9; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_28()
{
    mySRTActiveSurfaceCore->setactuator(9, 28);
    thecircle=9; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_29()
{
    mySRTActiveSurfaceCore->setactuator(9, 29);
    thecircle=9; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_30()
{
    mySRTActiveSurfaceCore->setactuator(9, 30);
    thecircle=9; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_31()
{
    mySRTActiveSurfaceCore->setactuator(9, 31);
    thecircle=9; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_32()
{
    mySRTActiveSurfaceCore->setactuator(9, 32);
    thecircle=9; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_33()
{
    mySRTActiveSurfaceCore->setactuator(9, 33);
    thecircle=9; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_34()
{
    mySRTActiveSurfaceCore->setactuator(9, 34);
    thecircle=9; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_35()
{
    mySRTActiveSurfaceCore->setactuator(9, 35);
    thecircle=9; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_36()
{
    mySRTActiveSurfaceCore->setactuator(9, 36);
    thecircle=9; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_37()
{
    mySRTActiveSurfaceCore->setactuator(9, 37);
    thecircle=9; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_38()
{
    mySRTActiveSurfaceCore->setactuator(9, 38);
    thecircle=9; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_39()
{
    mySRTActiveSurfaceCore->setactuator(9, 39);
    thecircle=9; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_40()
{
    mySRTActiveSurfaceCore->setactuator(9, 40);
    thecircle=9; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_41()
{
    mySRTActiveSurfaceCore->setactuator(9, 41);
    thecircle=9; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_42()
{
    mySRTActiveSurfaceCore->setactuator(9, 42);
    thecircle=9; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_43()
{
    mySRTActiveSurfaceCore->setactuator(9, 43);
    thecircle=9; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_44()
{
    mySRTActiveSurfaceCore->setactuator(9, 44);
    thecircle=9; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_45()
{
    mySRTActiveSurfaceCore->setactuator(9, 45);
    thecircle=9; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_46()
{
    mySRTActiveSurfaceCore->setactuator(9, 46);
    thecircle=9; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_47()
{
    mySRTActiveSurfaceCore->setactuator(9, 47);
    thecircle=9; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_48()
{
    mySRTActiveSurfaceCore->setactuator(9, 48);
    thecircle=9; theactuator=48; theradius=0;
}
// 6 CIRCLE
void SRTActiveSurfaceGUI::setactuator9_49()
{
    mySRTActiveSurfaceCore->setactuator(9, 49);
    thecircle=9; theactuator=49; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_50()
{
    mySRTActiveSurfaceCore->setactuator(9, 50);
    thecircle=9; theactuator=50; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_51()
{
    mySRTActiveSurfaceCore->setactuator(9, 51);
    thecircle=9; theactuator=51; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_52()
{
    mySRTActiveSurfaceCore->setactuator(9, 52);
    thecircle=9; theactuator=52; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_53()
{
    mySRTActiveSurfaceCore->setactuator(9, 53);
    thecircle=9; theactuator=53; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_54()
{
    mySRTActiveSurfaceCore->setactuator(9, 54);
    thecircle=9; theactuator=54; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_55()
{
    mySRTActiveSurfaceCore->setactuator(9, 55);
    thecircle=9; theactuator=55; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_56()
{
    mySRTActiveSurfaceCore->setactuator(9, 56);
    thecircle=9; theactuator=56; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_57()
{
   mySRTActiveSurfaceCore->setactuator(9, 57);
   thecircle=9; theactuator=57; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_58()
{
    mySRTActiveSurfaceCore->setactuator(9, 58);
    thecircle=9; theactuator=58; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_59()
{
    mySRTActiveSurfaceCore->setactuator(9, 59);
    thecircle=9; theactuator=59; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_60()
{
    mySRTActiveSurfaceCore->setactuator(9, 60);
    thecircle=9; theactuator=60; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_61()
{
    mySRTActiveSurfaceCore->setactuator(9, 61);
    thecircle=9; theactuator=61; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_62()
{
    mySRTActiveSurfaceCore->setactuator(9, 62);
    thecircle=9; theactuator=62; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_63()
{
    mySRTActiveSurfaceCore->setactuator(9, 63);
    thecircle=9; theactuator=63; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_64()
{
    mySRTActiveSurfaceCore->setactuator(9, 64);
    thecircle=9; theactuator=64; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_65()
{
    mySRTActiveSurfaceCore->setactuator(9, 65);
    thecircle=9; theactuator=65; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_66()
{
    mySRTActiveSurfaceCore->setactuator(9, 66);
    thecircle=9; theactuator=66; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_67()
{
    mySRTActiveSurfaceCore->setactuator(9, 67);
    thecircle=9; theactuator=67; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_68()
{
    mySRTActiveSurfaceCore->setactuator(9, 68);
    thecircle=9; theactuator=68; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_69()
{
    mySRTActiveSurfaceCore->setactuator(9, 69);
    thecircle=9; theactuator=69; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_70()
{
    mySRTActiveSurfaceCore->setactuator(9, 70);
    thecircle=9; theactuator=70; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_71()
{
    mySRTActiveSurfaceCore->setactuator(9, 71);
    thecircle=9; theactuator=71; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_72()
{
    mySRTActiveSurfaceCore->setactuator(9, 72);
    thecircle=9; theactuator=72; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_73()
{
    mySRTActiveSurfaceCore->setactuator(9, 73);
    thecircle=9; theactuator=73; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_74()
{
    mySRTActiveSurfaceCore->setactuator(9, 74);
    thecircle=9; theactuator=74; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_75()
{
    mySRTActiveSurfaceCore->setactuator(9, 75);
    thecircle=9; theactuator=75; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_76()
{
    mySRTActiveSurfaceCore->setactuator(9, 76);
    thecircle=9; theactuator=76; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_77()
{
    mySRTActiveSurfaceCore->setactuator(9, 77);
    thecircle=9; theactuator=77; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_78()
{
    mySRTActiveSurfaceCore->setactuator(9, 78);
    thecircle=9; theactuator=78; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_79()
{
    mySRTActiveSurfaceCore->setactuator(9, 79);
    thecircle=9; theactuator=79; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_80()
{
    mySRTActiveSurfaceCore->setactuator(9, 80);
    thecircle=9; theactuator=80; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_81()
{
    mySRTActiveSurfaceCore->setactuator(9, 81);
    thecircle=9; theactuator=81; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_82()
{
    mySRTActiveSurfaceCore->setactuator(9, 82);
    thecircle=9; theactuator=82; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_83()
{
    mySRTActiveSurfaceCore->setactuator(9, 83);
    thecircle=9; theactuator=83; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_84()
{
    mySRTActiveSurfaceCore->setactuator(9, 84);
    thecircle=9; theactuator=84; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_85()
{
    mySRTActiveSurfaceCore->setactuator(9, 85);
    thecircle=9; theactuator=85; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_86()
{
    mySRTActiveSurfaceCore->setactuator(9, 86);
    thecircle=9; theactuator=86; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_87()
{
    mySRTActiveSurfaceCore->setactuator(9, 87);
    thecircle=9; theactuator=87; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_88()
{
    mySRTActiveSurfaceCore->setactuator(9, 88);
    thecircle=9; theactuator=88; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_89()
{
    mySRTActiveSurfaceCore->setactuator(9, 89);
    thecircle=9; theactuator=89; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_90()
{
    mySRTActiveSurfaceCore->setactuator(9, 90);
    thecircle=9; theactuator=90; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_91()
{
    mySRTActiveSurfaceCore->setactuator(9, 91);
    thecircle=9; theactuator=91; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_92()
{
    mySRTActiveSurfaceCore->setactuator(9, 92);
    thecircle=9; theactuator=92; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_93()
{
    mySRTActiveSurfaceCore->setactuator(9, 93);
    thecircle=9; theactuator=93; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_94()
{
    mySRTActiveSurfaceCore->setactuator(9, 94);
    thecircle=9; theactuator=94; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_95()
{
    mySRTActiveSurfaceCore->setactuator(9, 95);
    thecircle=9; theactuator=95; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator9_96()
{
    mySRTActiveSurfaceCore->setactuator(9, 96);
    thecircle=9; theactuator=96; theradius=0;
}
// 10 CIRCLE
void SRTActiveSurfaceGUI::setactuator10_1()
{
    mySRTActiveSurfaceCore->setactuator(10, 1);
    thecircle=10; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_2()
{
    mySRTActiveSurfaceCore->setactuator(10, 2);
    thecircle=10; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_3()
{
    mySRTActiveSurfaceCore->setactuator(10, 3);
    thecircle=10; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_4()
{
    mySRTActiveSurfaceCore->setactuator(10, 4);
    thecircle=10; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_5()
{
    mySRTActiveSurfaceCore->setactuator(10, 5);
    thecircle=10; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_6()
{
    mySRTActiveSurfaceCore->setactuator(10, 6);
    thecircle=10; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_7()
{
    mySRTActiveSurfaceCore->setactuator(10, 7);
    thecircle=10; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_8()
{
    mySRTActiveSurfaceCore->setactuator(10, 8);
    thecircle=10; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_9()
{
   mySRTActiveSurfaceCore->setactuator(10, 9);
   thecircle=10; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_10()
{
    mySRTActiveSurfaceCore->setactuator(10, 10);
    thecircle=10; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_11()
{
    mySRTActiveSurfaceCore->setactuator(10, 11);
    thecircle=10; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_12()
{
    mySRTActiveSurfaceCore->setactuator(10, 12);
    thecircle=10; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_13()
{
    mySRTActiveSurfaceCore->setactuator(10, 13);
    thecircle=10; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_14()
{
    mySRTActiveSurfaceCore->setactuator(10, 14);
    thecircle=10; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_15()
{
    mySRTActiveSurfaceCore->setactuator(10, 15);
    thecircle=10; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_16()
{
    mySRTActiveSurfaceCore->setactuator(10, 16);
    thecircle=10; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_17()
{
    mySRTActiveSurfaceCore->setactuator(10, 17);
    thecircle=10; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_18()
{
    mySRTActiveSurfaceCore->setactuator(10, 18);
    thecircle=10; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_19()
{
    mySRTActiveSurfaceCore->setactuator(10, 19);
    thecircle=10; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_20()
{
    mySRTActiveSurfaceCore->setactuator(10, 20);
    thecircle=10; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_21()
{
    mySRTActiveSurfaceCore->setactuator(10, 21);
    thecircle=10; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_22()
{
    mySRTActiveSurfaceCore->setactuator(10, 22);
    thecircle=10; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_23()
{
    mySRTActiveSurfaceCore->setactuator(10, 23);
    thecircle=10; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_24()
{
    mySRTActiveSurfaceCore->setactuator(10, 24);
    thecircle=10; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_25()
{
    mySRTActiveSurfaceCore->setactuator(10, 25);
    thecircle=10; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_26()
{
    mySRTActiveSurfaceCore->setactuator(10, 26);
    thecircle=10; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_27()
{
    mySRTActiveSurfaceCore->setactuator(10, 27);
    thecircle=10; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_28()
{
    mySRTActiveSurfaceCore->setactuator(10, 28);
    thecircle=10; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_29()
{
    mySRTActiveSurfaceCore->setactuator(10, 29);
    thecircle=10; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_30()
{
    mySRTActiveSurfaceCore->setactuator(10, 30);
    thecircle=10; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_31()
{
    mySRTActiveSurfaceCore->setactuator(10, 31);
    thecircle=10; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_32()
{
    mySRTActiveSurfaceCore->setactuator(10, 32);
    thecircle=10; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_33()
{
    mySRTActiveSurfaceCore->setactuator(10, 33);
    thecircle=10; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_34()
{
    mySRTActiveSurfaceCore->setactuator(10, 34);
    thecircle=10; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_35()
{
    mySRTActiveSurfaceCore->setactuator(10, 35);
    thecircle=10; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_36()
{
    mySRTActiveSurfaceCore->setactuator(10, 36);
    thecircle=10; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_37()
{
    mySRTActiveSurfaceCore->setactuator(10, 37);
    thecircle=10; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_38()
{
    mySRTActiveSurfaceCore->setactuator(10, 38);
    thecircle=10; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_39()
{
    mySRTActiveSurfaceCore->setactuator(10, 39);
    thecircle=10; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_40()
{
    mySRTActiveSurfaceCore->setactuator(10, 40);
    thecircle=10; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_41()
{
    mySRTActiveSurfaceCore->setactuator(10, 41);
    thecircle=10; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_42()
{
    mySRTActiveSurfaceCore->setactuator(10, 42);
    thecircle=10; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_43()
{
    mySRTActiveSurfaceCore->setactuator(10, 43);
    thecircle=10; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_44()
{
    mySRTActiveSurfaceCore->setactuator(10, 44);
    thecircle=10; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_45()
{
    mySRTActiveSurfaceCore->setactuator(10, 45);
    thecircle=10; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_46()
{
    mySRTActiveSurfaceCore->setactuator(10, 46);
    thecircle=10; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_47()
{
    mySRTActiveSurfaceCore->setactuator(10, 47);
    thecircle=10; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_48()
{
    mySRTActiveSurfaceCore->setactuator(10, 48);
    thecircle=10; theactuator=48; theradius=0;
}
// 10 CIRCLE
void SRTActiveSurfaceGUI::setactuator10_49()
{
    mySRTActiveSurfaceCore->setactuator(10, 49);
    thecircle=10; theactuator=49; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_50()
{
    mySRTActiveSurfaceCore->setactuator(10, 50);
    thecircle=10; theactuator=50; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_51()
{
    mySRTActiveSurfaceCore->setactuator(10, 51);
    thecircle=10; theactuator=51; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_52()
{
    mySRTActiveSurfaceCore->setactuator(10, 52);
    thecircle=10; theactuator=52; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_53()
{
    mySRTActiveSurfaceCore->setactuator(10, 53);
    thecircle=10; theactuator=53; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_54()
{
    mySRTActiveSurfaceCore->setactuator(10, 54);
    thecircle=10; theactuator=54; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_55()
{
    mySRTActiveSurfaceCore->setactuator(10, 55);
    thecircle=10; theactuator=55; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_56()
{
    mySRTActiveSurfaceCore->setactuator(10, 56);
    thecircle=10; theactuator=56; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_57()
{
   mySRTActiveSurfaceCore->setactuator(10, 57);
   thecircle=10; theactuator=57; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_58()
{
    mySRTActiveSurfaceCore->setactuator(10, 58);
    thecircle=10; theactuator=58; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_59()
{
    mySRTActiveSurfaceCore->setactuator(10, 59);
    thecircle=10; theactuator=59; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_60()
{
    mySRTActiveSurfaceCore->setactuator(10, 60);
    thecircle=10; theactuator=60; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_61()
{
    mySRTActiveSurfaceCore->setactuator(10, 61);
    thecircle=10; theactuator=61; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_62()
{
    mySRTActiveSurfaceCore->setactuator(10, 62);
    thecircle=10; theactuator=62; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_63()
{
    mySRTActiveSurfaceCore->setactuator(10, 63);
    thecircle=10; theactuator=63; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_64()
{
    mySRTActiveSurfaceCore->setactuator(10, 64);
    thecircle=10; theactuator=64; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_65()
{
    mySRTActiveSurfaceCore->setactuator(10, 65);
    thecircle=10; theactuator=65; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_66()
{
    mySRTActiveSurfaceCore->setactuator(10, 66);
    thecircle=10; theactuator=66; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_67()
{
    mySRTActiveSurfaceCore->setactuator(10, 67);
    thecircle=10; theactuator=67; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_68()
{
    mySRTActiveSurfaceCore->setactuator(10, 68);
    thecircle=10; theactuator=68; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_69()
{
    mySRTActiveSurfaceCore->setactuator(10, 69);
    thecircle=10; theactuator=69; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_70()
{
    mySRTActiveSurfaceCore->setactuator(10, 70);
    thecircle=10; theactuator=70; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_71()
{
    mySRTActiveSurfaceCore->setactuator(10, 71);
    thecircle=10; theactuator=71; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_72()
{
    mySRTActiveSurfaceCore->setactuator(10, 72);
    thecircle=10; theactuator=72; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_73()
{
    mySRTActiveSurfaceCore->setactuator(10, 73);
    thecircle=10; theactuator=73; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_74()
{
    mySRTActiveSurfaceCore->setactuator(10, 74);
    thecircle=10; theactuator=74; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_75()
{
    mySRTActiveSurfaceCore->setactuator(10, 75);
    thecircle=10; theactuator=75; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_76()
{
    mySRTActiveSurfaceCore->setactuator(10, 76);
    thecircle=10; theactuator=76; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_77()
{
    mySRTActiveSurfaceCore->setactuator(10, 77);
    thecircle=10; theactuator=77; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_78()
{
    mySRTActiveSurfaceCore->setactuator(10, 78);
    thecircle=10; theactuator=78; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_79()
{
    mySRTActiveSurfaceCore->setactuator(10, 79);
    thecircle=10; theactuator=79; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_80()
{
    mySRTActiveSurfaceCore->setactuator(10, 80);
    thecircle=10; theactuator=80; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_81()
{
    mySRTActiveSurfaceCore->setactuator(10, 81);
    thecircle=10; theactuator=81; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_82()
{
    mySRTActiveSurfaceCore->setactuator(10, 82);
    thecircle=10; theactuator=82; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_83()
{
    mySRTActiveSurfaceCore->setactuator(10, 83);
    thecircle=10; theactuator=83; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_84()
{
    mySRTActiveSurfaceCore->setactuator(10, 84);
    thecircle=10; theactuator=84; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_85()
{
    mySRTActiveSurfaceCore->setactuator(10, 85);
    thecircle=10; theactuator=85; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_86()
{
    mySRTActiveSurfaceCore->setactuator(10, 86);
    thecircle=10; theactuator=86; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_87()
{
    mySRTActiveSurfaceCore->setactuator(10, 87);
    thecircle=10; theactuator=87; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_88()
{
    mySRTActiveSurfaceCore->setactuator(10, 88);
    thecircle=10; theactuator=88; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_89()
{
    mySRTActiveSurfaceCore->setactuator(10, 89);
    thecircle=10; theactuator=89; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_90()
{
    mySRTActiveSurfaceCore->setactuator(10, 90);
    thecircle=10; theactuator=90; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_91()
{
    mySRTActiveSurfaceCore->setactuator(10, 91);
    thecircle=10; theactuator=91; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_92()
{
    mySRTActiveSurfaceCore->setactuator(10, 92);
    thecircle=10; theactuator=92; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_93()
{
    mySRTActiveSurfaceCore->setactuator(10, 93);
    thecircle=10; theactuator=93; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_94()
{
    mySRTActiveSurfaceCore->setactuator(10, 94);
    thecircle=10; theactuator=94; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_95()
{
    mySRTActiveSurfaceCore->setactuator(10, 95);
    thecircle=10; theactuator=95; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator10_96()
{
    mySRTActiveSurfaceCore->setactuator(10, 96);
    thecircle=10; theactuator=96; theradius=0;
}
// 11 CIRCLE
void SRTActiveSurfaceGUI::setactuator11_1()
{
    mySRTActiveSurfaceCore->setactuator(11, 1);
    thecircle=11; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_2()
{
    mySRTActiveSurfaceCore->setactuator(11, 2);
    thecircle=11; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_3()
{
    mySRTActiveSurfaceCore->setactuator(11, 3);
    thecircle=11; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_4()
{
    mySRTActiveSurfaceCore->setactuator(11, 4);
    thecircle=11; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_5()
{
    mySRTActiveSurfaceCore->setactuator(11, 5);
    thecircle=11; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_6()
{
    mySRTActiveSurfaceCore->setactuator(11, 6);
    thecircle=11; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_7()
{
    mySRTActiveSurfaceCore->setactuator(11, 7);
    thecircle=11; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_8()
{
    mySRTActiveSurfaceCore->setactuator(11, 8);
    thecircle=11; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_9()
{
   mySRTActiveSurfaceCore->setactuator(11, 9);
   thecircle=11; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_10()
{
    mySRTActiveSurfaceCore->setactuator(11, 10);
    thecircle=11; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_11()
{
    mySRTActiveSurfaceCore->setactuator(11, 11);
    thecircle=11; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_12()
{
    mySRTActiveSurfaceCore->setactuator(11, 12);
    thecircle=11; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_13()
{
    mySRTActiveSurfaceCore->setactuator(11, 13);
    thecircle=11; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_14()
{
    mySRTActiveSurfaceCore->setactuator(11, 14);
    thecircle=11; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_15()
{
    mySRTActiveSurfaceCore->setactuator(11, 15);
    thecircle=11; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_16()
{
    mySRTActiveSurfaceCore->setactuator(11, 16);
    thecircle=11; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_17()
{
    mySRTActiveSurfaceCore->setactuator(11, 17);
    thecircle=11; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_18()
{
    mySRTActiveSurfaceCore->setactuator(11, 18);
    thecircle=11; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_19()
{
    mySRTActiveSurfaceCore->setactuator(11, 19);
    thecircle=11; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_20()
{
    mySRTActiveSurfaceCore->setactuator(11, 20);
    thecircle=11; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_21()
{
    mySRTActiveSurfaceCore->setactuator(11, 21);
    thecircle=11; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_22()
{
    mySRTActiveSurfaceCore->setactuator(11, 22);
    thecircle=11; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_23()
{
    mySRTActiveSurfaceCore->setactuator(11, 23);
    thecircle=11; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_24()
{
    mySRTActiveSurfaceCore->setactuator(11, 24);
    thecircle=11; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_25()
{
    mySRTActiveSurfaceCore->setactuator(11, 25);
    thecircle=11; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_26()
{
    mySRTActiveSurfaceCore->setactuator(11, 26);
    thecircle=11; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_27()
{
    mySRTActiveSurfaceCore->setactuator(11, 27);
    thecircle=11; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_28()
{
    mySRTActiveSurfaceCore->setactuator(11, 28);
    thecircle=11; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_29()
{
    mySRTActiveSurfaceCore->setactuator(11, 29);
    thecircle=11; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_30()
{
    mySRTActiveSurfaceCore->setactuator(11, 30);
    thecircle=11; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_31()
{
    mySRTActiveSurfaceCore->setactuator(11, 31);
    thecircle=11; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_32()
{
    mySRTActiveSurfaceCore->setactuator(11, 32);
    thecircle=11; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_33()
{
    mySRTActiveSurfaceCore->setactuator(11, 33);
    thecircle=11; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_34()
{
    mySRTActiveSurfaceCore->setactuator(11, 34);
    thecircle=11; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_35()
{
    mySRTActiveSurfaceCore->setactuator(11, 35);
    thecircle=11; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_36()
{
    mySRTActiveSurfaceCore->setactuator(11, 36);
    thecircle=11; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_37()
{
    mySRTActiveSurfaceCore->setactuator(11, 37);
    thecircle=11; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_38()
{
    mySRTActiveSurfaceCore->setactuator(11, 38);
    thecircle=11; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_39()
{
    mySRTActiveSurfaceCore->setactuator(11, 39);
    thecircle=11; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_40()
{
    mySRTActiveSurfaceCore->setactuator(11, 40);
    thecircle=11; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_41()
{
    mySRTActiveSurfaceCore->setactuator(11, 41);
    thecircle=11; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_42()
{
    mySRTActiveSurfaceCore->setactuator(11, 42);
    thecircle=11; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_43()
{
    mySRTActiveSurfaceCore->setactuator(11, 43);
    thecircle=11; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_44()
{
    mySRTActiveSurfaceCore->setactuator(11, 44);
    thecircle=11; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_45()
{
    mySRTActiveSurfaceCore->setactuator(11, 45);
    thecircle=11; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_46()
{
    mySRTActiveSurfaceCore->setactuator(11, 46);
    thecircle=11; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_47()
{
    mySRTActiveSurfaceCore->setactuator(11, 47);
    thecircle=11; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_48()
{
    mySRTActiveSurfaceCore->setactuator(11, 48);
    thecircle=11; theactuator=48; theradius=0;
}
// 6 CIRCLE
void SRTActiveSurfaceGUI::setactuator11_49()
{
    mySRTActiveSurfaceCore->setactuator(11, 49);
    thecircle=11; theactuator=49; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_50()
{
    mySRTActiveSurfaceCore->setactuator(11, 50);
    thecircle=11; theactuator=50; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_51()
{
    mySRTActiveSurfaceCore->setactuator(11, 51);
    thecircle=11; theactuator=51; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_52()
{
    mySRTActiveSurfaceCore->setactuator(11, 52);
    thecircle=11; theactuator=52; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_53()
{
    mySRTActiveSurfaceCore->setactuator(11, 53);
    thecircle=11; theactuator=53; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_54()
{
    mySRTActiveSurfaceCore->setactuator(11, 54);
    thecircle=11; theactuator=54; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_55()
{
    mySRTActiveSurfaceCore->setactuator(11, 55);
    thecircle=11; theactuator=55; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_56()
{
    mySRTActiveSurfaceCore->setactuator(11, 56);
    thecircle=11; theactuator=56; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_57()
{
   mySRTActiveSurfaceCore->setactuator(11, 57);
   thecircle=11; theactuator=57; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_58()
{
    mySRTActiveSurfaceCore->setactuator(11, 58);
    thecircle=11; theactuator=58; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_59()
{
    mySRTActiveSurfaceCore->setactuator(11, 59);
    thecircle=11; theactuator=59; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_60()
{
    mySRTActiveSurfaceCore->setactuator(11, 60);
    thecircle=11; theactuator=60; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_61()
{
    mySRTActiveSurfaceCore->setactuator(11, 61);
    thecircle=11; theactuator=61; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_62()
{
    mySRTActiveSurfaceCore->setactuator(11, 62);
    thecircle=11; theactuator=62; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_63()
{
    mySRTActiveSurfaceCore->setactuator(11, 63);
    thecircle=11; theactuator=63; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_64()
{
    mySRTActiveSurfaceCore->setactuator(11, 64);
    thecircle=11; theactuator=64; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_65()
{
    mySRTActiveSurfaceCore->setactuator(11, 65);
    thecircle=11; theactuator=65; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_66()
{
    mySRTActiveSurfaceCore->setactuator(11, 66);
    thecircle=11; theactuator=66; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_67()
{
    mySRTActiveSurfaceCore->setactuator(11, 67);
    thecircle=11; theactuator=67; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_68()
{
    mySRTActiveSurfaceCore->setactuator(11, 68);
    thecircle=11; theactuator=68; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_69()
{
    mySRTActiveSurfaceCore->setactuator(11, 69);
    thecircle=11; theactuator=69; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_70()
{
    mySRTActiveSurfaceCore->setactuator(11, 70);
    thecircle=11; theactuator=70; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_71()
{
    mySRTActiveSurfaceCore->setactuator(11, 71);
    thecircle=11; theactuator=71; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_72()
{
    mySRTActiveSurfaceCore->setactuator(11, 72);
    thecircle=11; theactuator=72; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_73()
{
    mySRTActiveSurfaceCore->setactuator(11, 73);
    thecircle=11; theactuator=73; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_74()
{
    mySRTActiveSurfaceCore->setactuator(11, 74);
    thecircle=11; theactuator=74; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_75()
{
    mySRTActiveSurfaceCore->setactuator(11, 75);
    thecircle=11; theactuator=75; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_76()
{
    mySRTActiveSurfaceCore->setactuator(11, 76);
    thecircle=11; theactuator=76; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_77()
{
    mySRTActiveSurfaceCore->setactuator(11, 77);
    thecircle=11; theactuator=77; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_78()
{
    mySRTActiveSurfaceCore->setactuator(11, 78);
    thecircle=11; theactuator=78; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_79()
{
    mySRTActiveSurfaceCore->setactuator(11, 79);
    thecircle=11; theactuator=79; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_80()
{
    mySRTActiveSurfaceCore->setactuator(11, 80);
    thecircle=11; theactuator=80; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_81()
{
    mySRTActiveSurfaceCore->setactuator(11, 81);
    thecircle=11; theactuator=81; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_82()
{
    mySRTActiveSurfaceCore->setactuator(11, 82);
    thecircle=11; theactuator=82; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_83()
{
    mySRTActiveSurfaceCore->setactuator(11, 83);
    thecircle=11; theactuator=83; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_84()
{
    mySRTActiveSurfaceCore->setactuator(11, 84);
    thecircle=11; theactuator=84; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_85()
{
    mySRTActiveSurfaceCore->setactuator(11, 85);
    thecircle=11; theactuator=85; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_86()
{
    mySRTActiveSurfaceCore->setactuator(11, 86);
    thecircle=11; theactuator=86; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_87()
{
    mySRTActiveSurfaceCore->setactuator(11, 87);
    thecircle=11; theactuator=87; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_88()
{
    mySRTActiveSurfaceCore->setactuator(11, 88);
    thecircle=11; theactuator=88; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_89()
{
    mySRTActiveSurfaceCore->setactuator(11, 89);
    thecircle=11; theactuator=89; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_90()
{
    mySRTActiveSurfaceCore->setactuator(11, 90);
    thecircle=11; theactuator=90; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_91()
{
    mySRTActiveSurfaceCore->setactuator(11, 91);
    thecircle=11; theactuator=91; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_92()
{
    mySRTActiveSurfaceCore->setactuator(11, 92);
    thecircle=11; theactuator=92; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_93()
{
    mySRTActiveSurfaceCore->setactuator(11, 93);
    thecircle=11; theactuator=93; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_94()
{
    mySRTActiveSurfaceCore->setactuator(11, 94);
    thecircle=11; theactuator=94; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_95()
{
    mySRTActiveSurfaceCore->setactuator(11, 95);
    thecircle=11; theactuator=95; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator11_96()
{
    mySRTActiveSurfaceCore->setactuator(11, 96);
    thecircle=11; theactuator=96; theradius=0;
}
// 12 CIRCLE
void SRTActiveSurfaceGUI::setactuator12_1()
{
    mySRTActiveSurfaceCore->setactuator(12, 1);
    thecircle=12; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_2()
{
    mySRTActiveSurfaceCore->setactuator(12, 2);
    thecircle=12; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_3()
{
    mySRTActiveSurfaceCore->setactuator(12, 3);
    thecircle=12; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_4()
{
    mySRTActiveSurfaceCore->setactuator(12, 4);
    thecircle=12; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_5()
{
    mySRTActiveSurfaceCore->setactuator(12, 5);
    thecircle=12; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_6()
{
    mySRTActiveSurfaceCore->setactuator(12, 6);
    thecircle=12; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_7()
{
    mySRTActiveSurfaceCore->setactuator(12, 7);
    thecircle=12; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_8()
{
    mySRTActiveSurfaceCore->setactuator(12, 8);
    thecircle=12; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_9()
{
   mySRTActiveSurfaceCore->setactuator(12, 9);
   thecircle=12; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_10()
{
    mySRTActiveSurfaceCore->setactuator(12, 10);
    thecircle=12; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_11()
{
    mySRTActiveSurfaceCore->setactuator(12, 11);
    thecircle=12; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_12()
{
    mySRTActiveSurfaceCore->setactuator(12, 12);
    thecircle=12; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_13()
{
    mySRTActiveSurfaceCore->setactuator(12, 13);
    thecircle=12; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_14()
{
    mySRTActiveSurfaceCore->setactuator(12, 14);
    thecircle=12; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_15()
{
    mySRTActiveSurfaceCore->setactuator(12, 15);
    thecircle=12; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_16()
{
    mySRTActiveSurfaceCore->setactuator(12, 16);
    thecircle=12; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_17()
{
    mySRTActiveSurfaceCore->setactuator(12, 17);
    thecircle=12; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_18()
{
    mySRTActiveSurfaceCore->setactuator(12, 18);
    thecircle=12; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_19()
{
    mySRTActiveSurfaceCore->setactuator(12, 19);
    thecircle=12; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_20()
{
    mySRTActiveSurfaceCore->setactuator(12, 20);
    thecircle=12; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_21()
{
    mySRTActiveSurfaceCore->setactuator(12, 21);
    thecircle=12; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_22()
{
    mySRTActiveSurfaceCore->setactuator(12, 22);
    thecircle=12; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_23()
{
    mySRTActiveSurfaceCore->setactuator(12, 23);
    thecircle=12; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_24()
{
    mySRTActiveSurfaceCore->setactuator(12, 24);
    thecircle=12; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_25()
{
    mySRTActiveSurfaceCore->setactuator(12, 25);
    thecircle=12; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_26()
{
    mySRTActiveSurfaceCore->setactuator(12, 26);
    thecircle=12; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_27()
{
    mySRTActiveSurfaceCore->setactuator(12, 27);
    thecircle=12; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_28()
{
    mySRTActiveSurfaceCore->setactuator(12, 28);
    thecircle=12; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_29()
{
    mySRTActiveSurfaceCore->setactuator(12, 29);
    thecircle=12; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_30()
{
    mySRTActiveSurfaceCore->setactuator(12, 30);
    thecircle=12; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_31()
{
    mySRTActiveSurfaceCore->setactuator(12, 31);
    thecircle=12; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_32()
{
    mySRTActiveSurfaceCore->setactuator(12, 32);
    thecircle=12; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_33()
{
    mySRTActiveSurfaceCore->setactuator(12, 33);
    thecircle=12; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_34()
{
    mySRTActiveSurfaceCore->setactuator(12, 34);
    thecircle=12; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_35()
{
    mySRTActiveSurfaceCore->setactuator(12, 35);
    thecircle=12; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_36()
{
    mySRTActiveSurfaceCore->setactuator(12, 36);
    thecircle=12; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_37()
{
    mySRTActiveSurfaceCore->setactuator(12, 37);
    thecircle=12; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_38()
{
    mySRTActiveSurfaceCore->setactuator(12, 38);
    thecircle=12; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_39()
{
    mySRTActiveSurfaceCore->setactuator(12, 39);
    thecircle=12; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_40()
{
    mySRTActiveSurfaceCore->setactuator(12, 40);
    thecircle=12; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_41()
{
    mySRTActiveSurfaceCore->setactuator(12, 41);
    thecircle=12; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_42()
{
    mySRTActiveSurfaceCore->setactuator(12, 42);
    thecircle=12; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_43()
{
    mySRTActiveSurfaceCore->setactuator(12, 43);
    thecircle=12; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_44()
{
    mySRTActiveSurfaceCore->setactuator(12, 44);
    thecircle=12; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_45()
{
    mySRTActiveSurfaceCore->setactuator(12, 45);
    thecircle=12; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_46()
{
    mySRTActiveSurfaceCore->setactuator(12, 46);
    thecircle=12; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_47()
{
    mySRTActiveSurfaceCore->setactuator(12, 47);
    thecircle=12; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_48()
{
    mySRTActiveSurfaceCore->setactuator(12, 48);
    thecircle=12; theactuator=48; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_49()
{
    mySRTActiveSurfaceCore->setactuator(12, 49);
    thecircle=12; theactuator=49; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_50()
{
    mySRTActiveSurfaceCore->setactuator(12, 50);
    thecircle=12; theactuator=50; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_51()
{
    mySRTActiveSurfaceCore->setactuator(12, 51);
    thecircle=12; theactuator=51; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_52()
{
    mySRTActiveSurfaceCore->setactuator(12, 52);
    thecircle=12; theactuator=52; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_53()
{
    mySRTActiveSurfaceCore->setactuator(12, 53);
    thecircle=12; theactuator=53; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_54()
{
    mySRTActiveSurfaceCore->setactuator(12, 54);
    thecircle=12; theactuator=54; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_55()
{
    mySRTActiveSurfaceCore->setactuator(12, 55);
    thecircle=12; theactuator=55; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_56()
{
    mySRTActiveSurfaceCore->setactuator(12, 56);
    thecircle=12; theactuator=56; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_57()
{
   mySRTActiveSurfaceCore->setactuator(12, 57);
   thecircle=12; theactuator=57; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_58()
{
    mySRTActiveSurfaceCore->setactuator(12, 58);
    thecircle=12; theactuator=58; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_59()
{
    mySRTActiveSurfaceCore->setactuator(12, 59);
    thecircle=12; theactuator=59; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_60()
{
    mySRTActiveSurfaceCore->setactuator(12, 60);
    thecircle=12; theactuator=60; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_61()
{
    mySRTActiveSurfaceCore->setactuator(12, 61);
    thecircle=12; theactuator=61; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_62()
{
    mySRTActiveSurfaceCore->setactuator(12, 62);
    thecircle=12; theactuator=62; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_63()
{
    mySRTActiveSurfaceCore->setactuator(12, 63);
    thecircle=12; theactuator=63; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_64()
{
    mySRTActiveSurfaceCore->setactuator(12, 64);
    thecircle=12; theactuator=64; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_65()
{
    mySRTActiveSurfaceCore->setactuator(12, 65);
    thecircle=12; theactuator=65; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_66()
{
    mySRTActiveSurfaceCore->setactuator(12, 66);
    thecircle=12; theactuator=66; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_67()
{
    mySRTActiveSurfaceCore->setactuator(12, 67);
    thecircle=12; theactuator=67; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_68()
{
    mySRTActiveSurfaceCore->setactuator(12, 68);
    thecircle=12; theactuator=68; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_69()
{
    mySRTActiveSurfaceCore->setactuator(12, 69);
    thecircle=12; theactuator=69; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_70()
{
    mySRTActiveSurfaceCore->setactuator(12, 70);
    thecircle=12; theactuator=70; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_71()
{
    mySRTActiveSurfaceCore->setactuator(12, 71);
    thecircle=12; theactuator=71; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_72()
{
    mySRTActiveSurfaceCore->setactuator(12, 72);
    thecircle=12; theactuator=72; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_73()
{
    mySRTActiveSurfaceCore->setactuator(12, 73);
    thecircle=12; theactuator=73; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_74()
{
    mySRTActiveSurfaceCore->setactuator(12, 74);
    thecircle=12; theactuator=74; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_75()
{
    mySRTActiveSurfaceCore->setactuator(12, 75);
    thecircle=12; theactuator=75; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_76()
{
    mySRTActiveSurfaceCore->setactuator(12, 76);
    thecircle=12; theactuator=76; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_77()
{
    mySRTActiveSurfaceCore->setactuator(12, 77);
    thecircle=12; theactuator=77; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_78()
{
    mySRTActiveSurfaceCore->setactuator(12, 78);
    thecircle=12; theactuator=78; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_79()
{
    mySRTActiveSurfaceCore->setactuator(12, 79);
    thecircle=12; theactuator=79; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_80()
{
    mySRTActiveSurfaceCore->setactuator(12, 80);
    thecircle=12; theactuator=80; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_81()
{
    mySRTActiveSurfaceCore->setactuator(12, 81);
    thecircle=12; theactuator=81; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_82()
{
    mySRTActiveSurfaceCore->setactuator(12, 82);
    thecircle=12; theactuator=82; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_83()
{
    mySRTActiveSurfaceCore->setactuator(12, 83);
    thecircle=12; theactuator=83; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_84()
{
    mySRTActiveSurfaceCore->setactuator(12, 84);
    thecircle=12; theactuator=84; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_85()
{
    mySRTActiveSurfaceCore->setactuator(12, 85);
    thecircle=12; theactuator=85; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_86()
{
    mySRTActiveSurfaceCore->setactuator(12, 86);
    thecircle=12; theactuator=86; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_87()
{
    mySRTActiveSurfaceCore->setactuator(12, 87);
    thecircle=12; theactuator=87; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_88()
{
    mySRTActiveSurfaceCore->setactuator(12, 88);
    thecircle=12; theactuator=88; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_89()
{
    mySRTActiveSurfaceCore->setactuator(12, 89);
    thecircle=12; theactuator=89; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_90()
{
    mySRTActiveSurfaceCore->setactuator(12, 90);
    thecircle=12; theactuator=90; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_91()
{
    mySRTActiveSurfaceCore->setactuator(12, 91);
    thecircle=12; theactuator=91; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_92()
{
    mySRTActiveSurfaceCore->setactuator(12, 92);
    thecircle=12; theactuator=92; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_93()
{
    mySRTActiveSurfaceCore->setactuator(12, 93);
    thecircle=12; theactuator=93; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_94()
{
    mySRTActiveSurfaceCore->setactuator(12, 94);
    thecircle=12; theactuator=94; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_95()
{
    mySRTActiveSurfaceCore->setactuator(12, 95);
    thecircle=12; theactuator=95; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator12_96()
{
    mySRTActiveSurfaceCore->setactuator(12, 96);
    thecircle=12; theactuator=96; theradius=0;
}
// 13 CIRCLE
void SRTActiveSurfaceGUI::setactuator13_1()
{
    mySRTActiveSurfaceCore->setactuator(13, 1);
    thecircle=13; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_2()
{
    mySRTActiveSurfaceCore->setactuator(13, 2);
    thecircle=13; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_3()
{
    mySRTActiveSurfaceCore->setactuator(13, 3);
    thecircle=13; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_4()
{
    mySRTActiveSurfaceCore->setactuator(13, 4);
    thecircle=13; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_5()
{
    mySRTActiveSurfaceCore->setactuator(13, 5);
    thecircle=13; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_6()
{
    mySRTActiveSurfaceCore->setactuator(13, 6);
    thecircle=13; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_7()
{
    mySRTActiveSurfaceCore->setactuator(13, 7);
    thecircle=13; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_8()
{
    mySRTActiveSurfaceCore->setactuator(13, 8);
    thecircle=13; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_9()
{
   mySRTActiveSurfaceCore->setactuator(13, 9);
   thecircle=13; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_10()
{
    mySRTActiveSurfaceCore->setactuator(13, 10);
    thecircle=13; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_11()
{
    mySRTActiveSurfaceCore->setactuator(13, 11);
    thecircle=13; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_12()
{
    mySRTActiveSurfaceCore->setactuator(13, 12);
    thecircle=13; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_13()
{
    mySRTActiveSurfaceCore->setactuator(13, 13);
    thecircle=13; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_14()
{
    mySRTActiveSurfaceCore->setactuator(13, 14);
    thecircle=13; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_15()
{
    mySRTActiveSurfaceCore->setactuator(13, 15);
    thecircle=13; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_16()
{
    mySRTActiveSurfaceCore->setactuator(13, 16);
    thecircle=13; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_17()
{
    mySRTActiveSurfaceCore->setactuator(13, 17);
    thecircle=13; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_18()
{
    mySRTActiveSurfaceCore->setactuator(13, 18);
    thecircle=13; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_19()
{
    mySRTActiveSurfaceCore->setactuator(13, 19);
    thecircle=13; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_20()
{
    mySRTActiveSurfaceCore->setactuator(13, 20);
    thecircle=13; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_21()
{
    mySRTActiveSurfaceCore->setactuator(13, 21);
    thecircle=13; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_22()
{
    mySRTActiveSurfaceCore->setactuator(13, 22);
    thecircle=13; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_23()
{
    mySRTActiveSurfaceCore->setactuator(13, 23);
    thecircle=13; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_24()
{
    mySRTActiveSurfaceCore->setactuator(13, 24);
    thecircle=13; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_25()
{
    mySRTActiveSurfaceCore->setactuator(13, 25);
    thecircle=13; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_26()
{
    mySRTActiveSurfaceCore->setactuator(13, 26);
    thecircle=13; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_27()
{
    mySRTActiveSurfaceCore->setactuator(13, 27);
    thecircle=13; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_28()
{
    mySRTActiveSurfaceCore->setactuator(13, 28);
    thecircle=13; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_29()
{
    mySRTActiveSurfaceCore->setactuator(13, 29);
    thecircle=13; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_30()
{
    mySRTActiveSurfaceCore->setactuator(13, 30);
    thecircle=13; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_31()
{
    mySRTActiveSurfaceCore->setactuator(13, 31);
    thecircle=13; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_32()
{
    mySRTActiveSurfaceCore->setactuator(13, 32);
    thecircle=13; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_33()
{
    mySRTActiveSurfaceCore->setactuator(13, 33);
    thecircle=13; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_34()
{
    mySRTActiveSurfaceCore->setactuator(13, 34);
    thecircle=13; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_35()
{
    mySRTActiveSurfaceCore->setactuator(13, 35);
    thecircle=13; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_36()
{
    mySRTActiveSurfaceCore->setactuator(13, 36);
    thecircle=13; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_37()
{
    mySRTActiveSurfaceCore->setactuator(13, 37);
    thecircle=13; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_38()
{
    mySRTActiveSurfaceCore->setactuator(13, 38);
    thecircle=13; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_39()
{
    mySRTActiveSurfaceCore->setactuator(13, 39);
    thecircle=13; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_40()
{
    mySRTActiveSurfaceCore->setactuator(13, 40);
    thecircle=13; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_41()
{
    mySRTActiveSurfaceCore->setactuator(13, 41);
    thecircle=13; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_42()
{
    mySRTActiveSurfaceCore->setactuator(13, 42);
    thecircle=13; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_43()
{
    mySRTActiveSurfaceCore->setactuator(13, 43);
    thecircle=13; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_44()
{
    mySRTActiveSurfaceCore->setactuator(13, 44);
    thecircle=13; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_45()
{
    mySRTActiveSurfaceCore->setactuator(13, 45);
    thecircle=13; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_46()
{
    mySRTActiveSurfaceCore->setactuator(13, 46);
    thecircle=13; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_47()
{
    mySRTActiveSurfaceCore->setactuator(13, 47);
    thecircle=13; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_48()
{
    mySRTActiveSurfaceCore->setactuator(13, 48);
    thecircle=13; theactuator=48; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_49()
{
    mySRTActiveSurfaceCore->setactuator(13, 49);
    thecircle=13; theactuator=49; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_50()
{
    mySRTActiveSurfaceCore->setactuator(13, 50);
    thecircle=13; theactuator=50; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_51()
{
    mySRTActiveSurfaceCore->setactuator(13, 51);
    thecircle=13; theactuator=51; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_52()
{
    mySRTActiveSurfaceCore->setactuator(13, 52);
    thecircle=13; theactuator=52; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_53()
{
    mySRTActiveSurfaceCore->setactuator(13, 53);
    thecircle=13; theactuator=53; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_54()
{
    mySRTActiveSurfaceCore->setactuator(13, 54);
    thecircle=13; theactuator=54; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_55()
{
    mySRTActiveSurfaceCore->setactuator(13, 55);
    thecircle=13; theactuator=55; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_56()
{
    mySRTActiveSurfaceCore->setactuator(13, 56);
    thecircle=13; theactuator=56; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_57()
{
   mySRTActiveSurfaceCore->setactuator(13, 57);
   thecircle=13; theactuator=57; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_58()
{
    mySRTActiveSurfaceCore->setactuator(13, 58);
    thecircle=13; theactuator=58; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_59()
{
    mySRTActiveSurfaceCore->setactuator(13, 59);
    thecircle=13; theactuator=59; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_60()
{
    mySRTActiveSurfaceCore->setactuator(13, 60);
    thecircle=13; theactuator=60; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_61()
{
    mySRTActiveSurfaceCore->setactuator(13, 61);
    thecircle=13; theactuator=61; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_62()
{
    mySRTActiveSurfaceCore->setactuator(13, 62);
    thecircle=13; theactuator=62; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_63()
{
    mySRTActiveSurfaceCore->setactuator(13, 63);
    thecircle=13; theactuator=63; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_64()
{
    mySRTActiveSurfaceCore->setactuator(13, 64);
    thecircle=13; theactuator=64; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_65()
{
    mySRTActiveSurfaceCore->setactuator(13, 65);
    thecircle=13; theactuator=65; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_66()
{
    mySRTActiveSurfaceCore->setactuator(13, 66);
    thecircle=13; theactuator=66; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_67()
{
    mySRTActiveSurfaceCore->setactuator(13, 67);
    thecircle=13; theactuator=67; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_68()
{
    mySRTActiveSurfaceCore->setactuator(13, 68);
    thecircle=13; theactuator=68; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_69()
{
    mySRTActiveSurfaceCore->setactuator(13, 69);
    thecircle=13; theactuator=69; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_70()
{
    mySRTActiveSurfaceCore->setactuator(13, 70);
    thecircle=13; theactuator=70; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_71()
{
    mySRTActiveSurfaceCore->setactuator(13, 71);
    thecircle=13; theactuator=71; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_72()
{
    mySRTActiveSurfaceCore->setactuator(13, 72);
    thecircle=13; theactuator=72; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_73()
{
    mySRTActiveSurfaceCore->setactuator(13, 73);
    thecircle=13; theactuator=73; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_74()
{
    mySRTActiveSurfaceCore->setactuator(13, 74);
    thecircle=13; theactuator=74; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_75()
{
    mySRTActiveSurfaceCore->setactuator(13, 75);
    thecircle=13; theactuator=75; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_76()
{
    mySRTActiveSurfaceCore->setactuator(13, 76);
    thecircle=13; theactuator=76; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_77()
{
    mySRTActiveSurfaceCore->setactuator(13, 77);
    thecircle=13; theactuator=77; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_78()
{
    mySRTActiveSurfaceCore->setactuator(13, 78);
    thecircle=13; theactuator=78; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_79()
{
    mySRTActiveSurfaceCore->setactuator(13, 79);
    thecircle=13; theactuator=79; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_80()
{
    mySRTActiveSurfaceCore->setactuator(13, 80);
    thecircle=13; theactuator=80; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_81()
{
    mySRTActiveSurfaceCore->setactuator(13, 81);
    thecircle=13; theactuator=81; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_82()
{
    mySRTActiveSurfaceCore->setactuator(13, 82);
    thecircle=13; theactuator=82; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_83()
{
    mySRTActiveSurfaceCore->setactuator(13, 83);
    thecircle=13; theactuator=83; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_84()
{
    mySRTActiveSurfaceCore->setactuator(13, 84);
    thecircle=13; theactuator=84; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_85()
{
    mySRTActiveSurfaceCore->setactuator(13, 85);
    thecircle=13; theactuator=85; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_86()
{
    mySRTActiveSurfaceCore->setactuator(13, 86);
    thecircle=13; theactuator=86; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_87()
{
    mySRTActiveSurfaceCore->setactuator(13, 87);
    thecircle=13; theactuator=87; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_88()
{
    mySRTActiveSurfaceCore->setactuator(13, 88);
    thecircle=13; theactuator=88; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_89()
{
    mySRTActiveSurfaceCore->setactuator(13, 89);
    thecircle=13; theactuator=89; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_90()
{
    mySRTActiveSurfaceCore->setactuator(13, 90);
    thecircle=13; theactuator=90; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_91()
{
    mySRTActiveSurfaceCore->setactuator(13, 91);
    thecircle=13; theactuator=91; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_92()
{
    mySRTActiveSurfaceCore->setactuator(13, 92);
    thecircle=13; theactuator=92; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_93()
{
    mySRTActiveSurfaceCore->setactuator(13, 93);
    thecircle=13; theactuator=93; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_94()
{
    mySRTActiveSurfaceCore->setactuator(13, 94);
    thecircle=13; theactuator=94; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_95()
{
    mySRTActiveSurfaceCore->setactuator(13, 95);
    thecircle=13; theactuator=95; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator13_96()
{
    mySRTActiveSurfaceCore->setactuator(13, 96);
    thecircle=13; theactuator=96; theradius=0;
}
// 14 CIRCLE
void SRTActiveSurfaceGUI::setactuator14_1()
{
    mySRTActiveSurfaceCore->setactuator(14, 1);
    thecircle=14; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_2()
{
    mySRTActiveSurfaceCore->setactuator(14, 2);
    thecircle=14; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_3()
{
    mySRTActiveSurfaceCore->setactuator(14, 3);
    thecircle=14; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_4()
{
    mySRTActiveSurfaceCore->setactuator(14, 4);
    thecircle=14; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_5()
{
    mySRTActiveSurfaceCore->setactuator(14, 5);
    thecircle=14; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_6()
{
    mySRTActiveSurfaceCore->setactuator(14, 6);
    thecircle=14; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_7()
{
    mySRTActiveSurfaceCore->setactuator(14, 7);
    thecircle=14; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_8()
{
    mySRTActiveSurfaceCore->setactuator(14, 8);
    thecircle=14; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_9()
{
   mySRTActiveSurfaceCore->setactuator(14, 9);
   thecircle=14; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_10()
{
    mySRTActiveSurfaceCore->setactuator(14, 10);
    thecircle=14; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_11()
{
    mySRTActiveSurfaceCore->setactuator(14, 11);
    thecircle=14; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_12()
{
    mySRTActiveSurfaceCore->setactuator(14, 12);
    thecircle=14; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_13()
{
    mySRTActiveSurfaceCore->setactuator(14, 13);
    thecircle=14; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_14()
{
    mySRTActiveSurfaceCore->setactuator(14, 14);
    thecircle=14; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_15()
{
    mySRTActiveSurfaceCore->setactuator(14, 15);
    thecircle=14; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_16()
{
    mySRTActiveSurfaceCore->setactuator(14, 16);
    thecircle=14; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_17()
{
    mySRTActiveSurfaceCore->setactuator(14, 17);
    thecircle=14; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_18()
{
    mySRTActiveSurfaceCore->setactuator(14, 18);
    thecircle=14; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_19()
{
    mySRTActiveSurfaceCore->setactuator(14, 19);
    thecircle=14; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_20()
{
    mySRTActiveSurfaceCore->setactuator(14, 20);
    thecircle=14; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_21()
{
    mySRTActiveSurfaceCore->setactuator(14, 21);
    thecircle=14; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_22()
{
    mySRTActiveSurfaceCore->setactuator(14, 22);
    thecircle=14; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_23()
{
    mySRTActiveSurfaceCore->setactuator(14, 23);
    thecircle=14; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_24()
{
    mySRTActiveSurfaceCore->setactuator(14, 24);
    thecircle=14; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_25()
{
    mySRTActiveSurfaceCore->setactuator(14, 25);
    thecircle=14; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_26()
{
    mySRTActiveSurfaceCore->setactuator(14, 26);
    thecircle=14; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_27()
{
    mySRTActiveSurfaceCore->setactuator(14, 27);
    thecircle=14; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_28()
{
    mySRTActiveSurfaceCore->setactuator(14, 28);
    thecircle=14; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_29()
{
    mySRTActiveSurfaceCore->setactuator(14, 29);
    thecircle=14; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_30()
{
    mySRTActiveSurfaceCore->setactuator(14, 30);
    thecircle=14; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_31()
{
    mySRTActiveSurfaceCore->setactuator(14, 31);
    thecircle=14; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_32()
{
    mySRTActiveSurfaceCore->setactuator(14, 32);
    thecircle=14; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_33()
{
    mySRTActiveSurfaceCore->setactuator(14, 33);
    thecircle=14; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_34()
{
    mySRTActiveSurfaceCore->setactuator(14, 34);
    thecircle=14; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_35()
{
    mySRTActiveSurfaceCore->setactuator(14, 35);
    thecircle=14; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_36()
{
    mySRTActiveSurfaceCore->setactuator(14, 36);
    thecircle=14; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_37()
{
    mySRTActiveSurfaceCore->setactuator(14, 37);
    thecircle=14; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_38()
{
    mySRTActiveSurfaceCore->setactuator(14, 38);
    thecircle=14; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_39()
{
    mySRTActiveSurfaceCore->setactuator(14, 39);
    thecircle=14; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_40()
{
    mySRTActiveSurfaceCore->setactuator(14, 40);
    thecircle=14; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_41()
{
    mySRTActiveSurfaceCore->setactuator(14, 41);
    thecircle=14; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_42()
{
    mySRTActiveSurfaceCore->setactuator(14, 42);
    thecircle=14; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_43()
{
    mySRTActiveSurfaceCore->setactuator(14, 43);
    thecircle=14; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_44()
{
    mySRTActiveSurfaceCore->setactuator(14, 44);
    thecircle=14; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_45()
{
    mySRTActiveSurfaceCore->setactuator(14, 45);
    thecircle=14; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_46()
{
    mySRTActiveSurfaceCore->setactuator(14, 46);
    thecircle=14; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_47()
{
    mySRTActiveSurfaceCore->setactuator(14, 47);
    thecircle=14; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_48()
{
    mySRTActiveSurfaceCore->setactuator(14, 48);
    thecircle=14; theactuator=48; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_49()
{
    mySRTActiveSurfaceCore->setactuator(14, 49);
    thecircle=14; theactuator=49; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_50()
{
    mySRTActiveSurfaceCore->setactuator(14, 50);
    thecircle=14; theactuator=50; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_51()
{
    mySRTActiveSurfaceCore->setactuator(14, 51);
    thecircle=14; theactuator=51; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_52()
{
    mySRTActiveSurfaceCore->setactuator(14, 52);
    thecircle=14; theactuator=52; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_53()
{
    mySRTActiveSurfaceCore->setactuator(14, 53);
    thecircle=14; theactuator=53; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_54()
{
    mySRTActiveSurfaceCore->setactuator(14, 54);
    thecircle=14; theactuator=54; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_55()
{
    mySRTActiveSurfaceCore->setactuator(14, 55);
    thecircle=14; theactuator=55; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_56()
{
    mySRTActiveSurfaceCore->setactuator(14, 56);
    thecircle=14; theactuator=56; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_57()
{
   mySRTActiveSurfaceCore->setactuator(14, 57);
   thecircle=14; theactuator=57; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_58()
{
    mySRTActiveSurfaceCore->setactuator(14, 58);
    thecircle=14; theactuator=58; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_59()
{
    mySRTActiveSurfaceCore->setactuator(14, 59);
    thecircle=14; theactuator=59; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_60()
{
    mySRTActiveSurfaceCore->setactuator(14, 60);
    thecircle=14; theactuator=60; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_61()
{
    mySRTActiveSurfaceCore->setactuator(14, 61);
    thecircle=14; theactuator=61; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_62()
{
    mySRTActiveSurfaceCore->setactuator(14, 62);
    thecircle=14; theactuator=62; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_63()
{
    mySRTActiveSurfaceCore->setactuator(14, 63);
    thecircle=14; theactuator=63; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_64()
{
    mySRTActiveSurfaceCore->setactuator(14, 64);
    thecircle=14; theactuator=64; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_65()
{
    mySRTActiveSurfaceCore->setactuator(14, 65);
    thecircle=14; theactuator=65; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_66()
{
    mySRTActiveSurfaceCore->setactuator(14, 66);
    thecircle=14; theactuator=66; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_67()
{
    mySRTActiveSurfaceCore->setactuator(14, 67);
    thecircle=14; theactuator=67; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_68()
{
    mySRTActiveSurfaceCore->setactuator(14, 68);
    thecircle=14; theactuator=68; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_69()
{
    mySRTActiveSurfaceCore->setactuator(14, 69);
    thecircle=14; theactuator=69; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_70()
{
    mySRTActiveSurfaceCore->setactuator(14, 70);
    thecircle=14; theactuator=70; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_71()
{
    mySRTActiveSurfaceCore->setactuator(14, 71);
    thecircle=14; theactuator=71; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_72()
{
    mySRTActiveSurfaceCore->setactuator(14, 72);
    thecircle=14; theactuator=72; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_73()
{
    mySRTActiveSurfaceCore->setactuator(14, 73);
    thecircle=14; theactuator=73; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_74()
{
    mySRTActiveSurfaceCore->setactuator(14, 74);
    thecircle=14; theactuator=74; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_75()
{
    mySRTActiveSurfaceCore->setactuator(14, 75);
    thecircle=14; theactuator=75; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_76()
{
    mySRTActiveSurfaceCore->setactuator(14, 76);
    thecircle=14; theactuator=76; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_77()
{
    mySRTActiveSurfaceCore->setactuator(14, 77);
    thecircle=14; theactuator=77; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_78()
{
    mySRTActiveSurfaceCore->setactuator(14, 78);
    thecircle=14; theactuator=78; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_79()
{
    mySRTActiveSurfaceCore->setactuator(14, 79);
    thecircle=14; theactuator=79; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_80()
{
    mySRTActiveSurfaceCore->setactuator(14, 80);
    thecircle=14; theactuator=80; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_81()
{
    mySRTActiveSurfaceCore->setactuator(14, 81);
    thecircle=14; theactuator=81; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_82()
{
    mySRTActiveSurfaceCore->setactuator(14, 82);
    thecircle=14; theactuator=82; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_83()
{
    mySRTActiveSurfaceCore->setactuator(14, 83);
    thecircle=14; theactuator=83; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_84()
{
    mySRTActiveSurfaceCore->setactuator(14, 84);
    thecircle=14; theactuator=84; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_85()
{
    mySRTActiveSurfaceCore->setactuator(14, 85);
    thecircle=14; theactuator=85; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_86()
{
    mySRTActiveSurfaceCore->setactuator(14, 86);
    thecircle=14; theactuator=86; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_87()
{
    mySRTActiveSurfaceCore->setactuator(14, 87);
    thecircle=14; theactuator=87; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_88()
{
    mySRTActiveSurfaceCore->setactuator(14, 88);
    thecircle=14; theactuator=88; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_89()
{
    mySRTActiveSurfaceCore->setactuator(14, 89);
    thecircle=14; theactuator=89; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_90()
{
    mySRTActiveSurfaceCore->setactuator(14, 90);
    thecircle=14; theactuator=90; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_91()
{
    mySRTActiveSurfaceCore->setactuator(14, 91);
    thecircle=14; theactuator=91; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_92()
{
    mySRTActiveSurfaceCore->setactuator(14, 92);
    thecircle=14; theactuator=92; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_93()
{
    mySRTActiveSurfaceCore->setactuator(14, 93);
    thecircle=14; theactuator=93; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_94()
{
    mySRTActiveSurfaceCore->setactuator(14, 94);
    thecircle=14; theactuator=94; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_95()
{
    mySRTActiveSurfaceCore->setactuator(14, 95);
    thecircle=14; theactuator=95; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator14_96()
{
    mySRTActiveSurfaceCore->setactuator(14, 96);
    thecircle=14; theactuator=96; theradius=0;
}
// 15 CIRCLE
void SRTActiveSurfaceGUI::setactuator15_1()
{
    mySRTActiveSurfaceCore->setactuator(15, 1);
    thecircle=15; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_2()
{
    mySRTActiveSurfaceCore->setactuator(15, 2);
    thecircle=15; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_3()
{
    mySRTActiveSurfaceCore->setactuator(15, 3);
    thecircle=15; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_4()
{
    mySRTActiveSurfaceCore->setactuator(15, 4);
    thecircle=15; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_5()
{
    mySRTActiveSurfaceCore->setactuator(15, 5);
    thecircle=15; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_6()
{
    mySRTActiveSurfaceCore->setactuator(15, 6);
    thecircle=15; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_7()
{
    mySRTActiveSurfaceCore->setactuator(15, 7);
    thecircle=15; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_8()
{
    mySRTActiveSurfaceCore->setactuator(15, 8);
    thecircle=15; theactuator=8; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_9()
{
   mySRTActiveSurfaceCore->setactuator(15, 9);
   thecircle=15; theactuator=9; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_10()
{
    mySRTActiveSurfaceCore->setactuator(15, 10);
    thecircle=15; theactuator=10; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_11()
{
    mySRTActiveSurfaceCore->setactuator(15, 11);
    thecircle=15; theactuator=11; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_12()
{
    mySRTActiveSurfaceCore->setactuator(15, 12);
    thecircle=15; theactuator=12; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_13()
{
    mySRTActiveSurfaceCore->setactuator(15, 13);
    thecircle=15; theactuator=13; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_14()
{
    mySRTActiveSurfaceCore->setactuator(15, 14);
    thecircle=15; theactuator=14; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_15()
{
    mySRTActiveSurfaceCore->setactuator(15, 15);
    thecircle=15; theactuator=15; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_16()
{
    mySRTActiveSurfaceCore->setactuator(15, 16);
    thecircle=15; theactuator=16; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_17()
{
    mySRTActiveSurfaceCore->setactuator(15, 17);
    thecircle=15; theactuator=17; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_18()
{
    mySRTActiveSurfaceCore->setactuator(15, 18);
    thecircle=15; theactuator=18; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_19()
{
    mySRTActiveSurfaceCore->setactuator(15, 19);
    thecircle=15; theactuator=19; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_20()
{
    mySRTActiveSurfaceCore->setactuator(15, 20);
    thecircle=15; theactuator=20; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_21()
{
    mySRTActiveSurfaceCore->setactuator(15, 21);
    thecircle=15; theactuator=21; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_22()
{
    mySRTActiveSurfaceCore->setactuator(15, 22);
    thecircle=15; theactuator=22; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_23()
{
    mySRTActiveSurfaceCore->setactuator(15, 23);
    thecircle=15; theactuator=23; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_24()
{
    mySRTActiveSurfaceCore->setactuator(15, 24);
    thecircle=15; theactuator=24; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_25()
{
    mySRTActiveSurfaceCore->setactuator(15, 25);
    thecircle=15; theactuator=25; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_26()
{
    mySRTActiveSurfaceCore->setactuator(15, 26);
    thecircle=15; theactuator=26; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_27()
{
    mySRTActiveSurfaceCore->setactuator(15, 27);
    thecircle=15; theactuator=27; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_28()
{
    mySRTActiveSurfaceCore->setactuator(15, 28);
    thecircle=15; theactuator=28; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_29()
{
    mySRTActiveSurfaceCore->setactuator(15, 29);
    thecircle=15; theactuator=29; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_30()
{
    mySRTActiveSurfaceCore->setactuator(15, 30);
    thecircle=15; theactuator=30; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_31()
{
    mySRTActiveSurfaceCore->setactuator(15, 31);
    thecircle=15; theactuator=31; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_32()
{
    mySRTActiveSurfaceCore->setactuator(15, 32);
    thecircle=15; theactuator=32; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_33()
{
    mySRTActiveSurfaceCore->setactuator(15, 33);
    thecircle=15; theactuator=33; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_34()
{
    mySRTActiveSurfaceCore->setactuator(15, 34);
    thecircle=15; theactuator=34; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_35()
{
    mySRTActiveSurfaceCore->setactuator(15, 35);
    thecircle=15; theactuator=35; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_36()
{
    mySRTActiveSurfaceCore->setactuator(15, 36);
    thecircle=15; theactuator=36; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_37()
{
    mySRTActiveSurfaceCore->setactuator(15, 37);
    thecircle=15; theactuator=37; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_38()
{
    mySRTActiveSurfaceCore->setactuator(15, 38);
    thecircle=15; theactuator=38; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_39()
{
    mySRTActiveSurfaceCore->setactuator(15, 39);
    thecircle=15; theactuator=39; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_40()
{
    mySRTActiveSurfaceCore->setactuator(15, 40);
    thecircle=15; theactuator=40; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_41()
{
    mySRTActiveSurfaceCore->setactuator(15, 41);
    thecircle=15; theactuator=41; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_42()
{
    mySRTActiveSurfaceCore->setactuator(15, 42);
    thecircle=15; theactuator=42; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_43()
{
    mySRTActiveSurfaceCore->setactuator(15, 43);
    thecircle=15; theactuator=43; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_44()
{
    mySRTActiveSurfaceCore->setactuator(15, 44);
    thecircle=15; theactuator=44; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_45()
{
    mySRTActiveSurfaceCore->setactuator(15, 45);
    thecircle=15; theactuator=45; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_46()
{
    mySRTActiveSurfaceCore->setactuator(15, 46);
    thecircle=15; theactuator=46; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_47()
{
    mySRTActiveSurfaceCore->setactuator(15, 47);
    thecircle=15; theactuator=47; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_48()
{
    mySRTActiveSurfaceCore->setactuator(15, 48);
    thecircle=15; theactuator=48; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_49()
{
    mySRTActiveSurfaceCore->setactuator(15, 49);
    thecircle=15; theactuator=49; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_50()
{
    mySRTActiveSurfaceCore->setactuator(15, 50);
    thecircle=15; theactuator=50; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_51()
{
    mySRTActiveSurfaceCore->setactuator(15, 51);
    thecircle=15; theactuator=51; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_52()
{
    mySRTActiveSurfaceCore->setactuator(15, 52);
    thecircle=15; theactuator=52; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_53()
{
    mySRTActiveSurfaceCore->setactuator(15, 53);
    thecircle=15; theactuator=53; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_54()
{
    mySRTActiveSurfaceCore->setactuator(15, 54);
    thecircle=15; theactuator=54; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_55()
{
    mySRTActiveSurfaceCore->setactuator(15, 55);
    thecircle=15; theactuator=55; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_56()
{
    mySRTActiveSurfaceCore->setactuator(15, 56);
    thecircle=15; theactuator=56; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_57()
{
   mySRTActiveSurfaceCore->setactuator(15, 57);
   thecircle=15; theactuator=57; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_58()
{
    mySRTActiveSurfaceCore->setactuator(15, 58);
    thecircle=15; theactuator=58; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_59()
{
    mySRTActiveSurfaceCore->setactuator(15, 59);
    thecircle=15; theactuator=59; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_60()
{
    mySRTActiveSurfaceCore->setactuator(15, 60);
    thecircle=15; theactuator=60; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_61()
{
    mySRTActiveSurfaceCore->setactuator(15, 61);
    thecircle=15; theactuator=61; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_62()
{
    mySRTActiveSurfaceCore->setactuator(15, 62);
    thecircle=15; theactuator=62; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_63()
{
    mySRTActiveSurfaceCore->setactuator(15, 63);
    thecircle=15; theactuator=63; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_64()
{
    mySRTActiveSurfaceCore->setactuator(15, 64);
    thecircle=15; theactuator=64; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_65()
{
    mySRTActiveSurfaceCore->setactuator(15, 65);
    thecircle=15; theactuator=65; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_66()
{
    mySRTActiveSurfaceCore->setactuator(15, 66);
    thecircle=15; theactuator=66; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_67()
{
    mySRTActiveSurfaceCore->setactuator(15, 67);
    thecircle=15; theactuator=67; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_68()
{
    mySRTActiveSurfaceCore->setactuator(15, 68);
    thecircle=15; theactuator=68; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_69()
{
    mySRTActiveSurfaceCore->setactuator(15, 69);
    thecircle=15; theactuator=69; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_70()
{
    mySRTActiveSurfaceCore->setactuator(15, 70);
    thecircle=15; theactuator=70; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_71()
{
    mySRTActiveSurfaceCore->setactuator(15, 71);
    thecircle=15; theactuator=71; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_72()
{
    mySRTActiveSurfaceCore->setactuator(15, 72);
    thecircle=15; theactuator=72; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_73()
{
    mySRTActiveSurfaceCore->setactuator(15, 73);
    thecircle=15; theactuator=73; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_74()
{
    mySRTActiveSurfaceCore->setactuator(15, 74);
    thecircle=15; theactuator=74; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_75()
{
    mySRTActiveSurfaceCore->setactuator(15, 75);
    thecircle=15; theactuator=75; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_76()
{
    mySRTActiveSurfaceCore->setactuator(15, 76);
    thecircle=15; theactuator=76; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_77()
{
    mySRTActiveSurfaceCore->setactuator(15, 77);
    thecircle=15; theactuator=77; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_78()
{
    mySRTActiveSurfaceCore->setactuator(15, 78);
    thecircle=15; theactuator=78; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_79()
{
    mySRTActiveSurfaceCore->setactuator(15, 79);
    thecircle=15; theactuator=79; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_80()
{
    mySRTActiveSurfaceCore->setactuator(15, 80);
    thecircle=15; theactuator=80; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_81()
{
    mySRTActiveSurfaceCore->setactuator(15, 81);
    thecircle=15; theactuator=81; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_82()
{
    mySRTActiveSurfaceCore->setactuator(15, 82);
    thecircle=15; theactuator=82; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_83()
{
    mySRTActiveSurfaceCore->setactuator(15, 83);
    thecircle=15; theactuator=83; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_84()
{
    mySRTActiveSurfaceCore->setactuator(15, 84);
    thecircle=15; theactuator=84; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_85()
{
    mySRTActiveSurfaceCore->setactuator(15, 85);
    thecircle=15; theactuator=85; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_86()
{
    mySRTActiveSurfaceCore->setactuator(15, 86);
    thecircle=15; theactuator=86; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_87()
{
    mySRTActiveSurfaceCore->setactuator(15, 87);
    thecircle=15; theactuator=87; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_88()
{
    mySRTActiveSurfaceCore->setactuator(15, 88);
    thecircle=15; theactuator=88; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_89()
{
    mySRTActiveSurfaceCore->setactuator(15, 89);
    thecircle=15; theactuator=89; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_90()
{
    mySRTActiveSurfaceCore->setactuator(15, 90);
    thecircle=15; theactuator=90; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_91()
{
    mySRTActiveSurfaceCore->setactuator(15, 91);
    thecircle=15; theactuator=91; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_92()
{
    mySRTActiveSurfaceCore->setactuator(15, 92);
    thecircle=15; theactuator=92; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_93()
{
    mySRTActiveSurfaceCore->setactuator(15, 93);
    thecircle=15; theactuator=93; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_94()
{
    mySRTActiveSurfaceCore->setactuator(15, 94);
    thecircle=15; theactuator=94; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_95()
{
    mySRTActiveSurfaceCore->setactuator(15, 95);
    thecircle=15; theactuator=95; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator15_96()
{
    mySRTActiveSurfaceCore->setactuator(15, 96);
    thecircle=15; theactuator=96; theradius=0;
}
// 16 CIRCLE
void SRTActiveSurfaceGUI::setactuator16_1()
{
    mySRTActiveSurfaceCore->setactuator(16, 1);
    thecircle=16; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator16_2()
{
    mySRTActiveSurfaceCore->setactuator(16, 2);
    thecircle=16; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator16_3()
{
    mySRTActiveSurfaceCore->setactuator(16, 3);
    thecircle=16; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator16_4()
{
    mySRTActiveSurfaceCore->setactuator(16, 4);
    thecircle=16; theactuator=4; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator16_5()
{
    mySRTActiveSurfaceCore->setactuator(16, 5);
    thecircle=16; theactuator=5; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator16_6()
{
    mySRTActiveSurfaceCore->setactuator(16, 6);
    thecircle=16; theactuator=6; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator16_7()
{
    mySRTActiveSurfaceCore->setactuator(16, 7);
    thecircle=16; theactuator=7; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator16_8()
{
    mySRTActiveSurfaceCore->setactuator(16, 8);
    thecircle=16; theactuator=8; theradius=0;
}
// 17 CIRCLE
void SRTActiveSurfaceGUI::setactuator17_1()
{
    mySRTActiveSurfaceCore->setactuator(17, 1);
    thecircle=17; theactuator=1; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator17_2()
{
    mySRTActiveSurfaceCore->setactuator(17, 2);
    thecircle=17; theactuator=2; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator17_3()
{
    mySRTActiveSurfaceCore->setactuator(17, 3);
    thecircle=17; theactuator=3; theradius=0;
}
void SRTActiveSurfaceGUI::setactuator17_4()
{
    mySRTActiveSurfaceCore->setactuator(17, 4);
    thecircle=17; theactuator=4; theradius=0;
}

void SRTActiveSurfaceGUI::manager()
{
    ManagerPasswordlineEdit->setEnabled(true);
    if (passwordcheck == 0)
    {
 QString str;
 str = "enter password & press Enter";
 ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
 ManagerLabel->setText(str);
 str = ManagerPasswordlineEdit->text();
 if (str=="sbasilio") 
              {
                        buttonGroup1->setEnabled(true);
                        passwordcheck=1;
                        ManagerLabel->clear();
                        ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
                        str = "password ok";
                        ManagerLabel->setText(str);
                        ManagerPasswordlineEdit->clear();
              } 
              else 
              {
                        passwordcheck = 0;
              }
     }
}

void SRTActiveSurfaceGUI::move()
{
    QString str, str1;
    long incr;
    
    str1 = ActuatorNumberlineEdit->text();
    if (str1.length()==0)
    {
 ManagerLabel->clear();
              ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
 ManagerLabel->setText("choose actuator");
    }
    else
    {
 str = ActuatorMovelineEdit->text();
              if (str.length()==0)
    incr = 0;
 else
    incr = str.toLong();
              ManagerLabel->clear();
 ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
 if (thecircle != 0 && theactuator == 0 && theradius == 0)
     str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("move");
 if (thecircle == 0 && theactuator == 0 && theradius != 0)
     str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("move");
 if (thecircle != 0 && theactuator != 0 && theradius == 0)
     str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("move");
 if (thecircle == 0 && theactuator == 0 && theradius == 0)
     str = QString("%1 %2").arg("ALL").arg("move");
 ManagerLabel->setText(str);
 mySRTActiveSurfaceCore->move(thecircle,theactuator,theradius,incr);
   }  
}

void SRTActiveSurfaceGUI::setprofile()
{
    QString str, str1;
    long profile;
    
    str1 = ActuatorNumberlineEdit->text();
    if (str1.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	profile = (long)SetProfilecomboBox->currentItem();
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	thecircle = theactuator = theradius = 0;
	str = QString("%1 %2").arg("ALL").arg("setprofile");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->setProfile(profile);
    }
}

void SRTActiveSurfaceGUI::correction()
{
    QString str, str1;
    double correction;
    
    str1 = ActuatorNumberlineEdit->text();
    if (str1.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	str = ActuatorCorrectionlineEdit->text();
	if (str.length()==0)
	    correction = 0;
	else
	    correction = str.toDouble();
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("correction");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("correction");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("correction");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("correction");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->correction(thecircle,theactuator,theradius,correction);
    }  
}

void SRTActiveSurfaceGUI::update()
{
    QString str, str1;
    long int elev;
    
    str = ActuatorUpdatelineEdit->text();
    if (str.length()==0)
	elev = 0;
    else
	elev = str.toLong();
    if (elev <0 || elev >90) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,0,0));
	str1 = QString("%1").arg("Elevation out of range");
	ManagerLabel->setText(str1);
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	thecircle = theactuator = theradius = 0;
	str = QString("%1 %2").arg("ALL").arg("update");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->update(elev);
    }
}

void SRTActiveSurfaceGUI::reset()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("reset");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("reset");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("reset");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("reset");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->reset(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::stop()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("stop");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("stop");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("stop");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("stop");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->stop(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::up()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("up");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("up");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("up");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("up");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->up(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::down()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("down");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("down");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("down");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("down");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->down(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::top()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("top");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("top");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("top");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("top");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->top(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::bottom()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("bottom");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("bottom");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("bottom");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("bottom");	
	 ManagerLabel->setText(str);
	 mySRTActiveSurfaceCore->bottom(thecircle,theactuator,theradius);
     }
}

void SRTActiveSurfaceGUI::calibrate()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("calibrate");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("calibrate");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("calibrate");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("calibrate");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->calibrate(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::calVer()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("verify calibration");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("verify calibration");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("verify calibration");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("verify calibration");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->calVer(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::stow()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("stow");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("stow");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("stow");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("stow");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->stow(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::setup()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("setup");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("setup");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("setup");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("setup");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->setup(thecircle,theactuator,theradius);
    }
}

void SRTActiveSurfaceGUI::refPos()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	if (thecircle != 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg("CIRCLE").arg(thecircle).arg("reference position");
	if (thecircle == 0 && theactuator == 0 && theradius != 0)
	    str = QString("%1_%2 %3").arg("RADIUS").arg(theradius).arg("reference position");
	if (thecircle != 0 && theactuator != 0 && theradius == 0)
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("reference position");
	if (thecircle == 0 && theactuator == 0 && theradius == 0)
	    str = QString("%1 %2").arg("ALL").arg("reference position");
	ManagerLabel->setText(str);
	mySRTActiveSurfaceCore->refPos(thecircle,theactuator,theradius);
    }
}
void SRTActiveSurfaceGUI::recoverUSD()
{
    QString str;
    str = ActuatorNumberlineEdit->text();
    if (str.length()==0) {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(255,255,0));
	ManagerLabel->setText("choose actuator");
    }
    else {
	ManagerLabel->clear();
	ManagerLabel->setPaletteBackgroundColor(QColor(85,255,0));
	
	if (thecircle != 0 && theactuator != 0 && theradius == 0) {
	    str = QString("%1_%2 %3").arg(thecircle).arg(theactuator).arg("USD recover");
	    ManagerLabel->setText(str);
	    mySRTActiveSurfaceCore->recoverUSD(thecircle,theactuator);
	}
	else {
	    str = QString("%1").arg("only one actuator per time");
	    ManagerLabel->setText(str);
	}
    }
}


void SRTActiveSurfaceGUI::enable()
{
    mySRTActiveSurfaceCore->enable();
    EnableButton->setPaletteBackgroundColor(QColor(0,170,0));
    DisableButton->setPaletteBackgroundColor(QColor(0,85,255));
}

void SRTActiveSurfaceGUI::disable()
{
    mySRTActiveSurfaceCore->disable();
    EnableButton->setPaletteBackgroundColor(QColor(0,85,255));
    DisableButton->setPaletteBackgroundColor(QColor(0,170,0));
}

void SRTActiveSurfaceGUI::init()
{

}

void SRTActiveSurfaceGUI::destroy()
{

}
