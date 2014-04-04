#include "Led.h"

Led::Led(QWidget * parent,QString str ,color attrt, mode modt)
{

	attr=attrt;
	mod=modt;
    lab=new QLabel(parent);
    labT=new QLabel(parent);
	if(attr==Green) {
		if(mod==On) icon=new QIcon("../icon/greenOn.png");
		else icon=new QIcon("../icon/greenOff.png");
	}
	if(attr==Red) {
		if(mod==On)icon=new QIcon("../icon/redOn.png");
		else icon=new QIcon("../icon/redOff.png");
	}	
	if(attr==Yellow) {
		if(mod==On)icon=new QIcon("../icon/yellowOn.png");
		else icon=new QIcon("../icon/yellowOff.png");
	}
	QPixmap pixmap = icon->pixmap(QSize(18, 18),QIcon::Normal, QIcon::On);
    lab->setPixmap(pixmap);
    text=str;
    text.insert(0,"      ");
    labT->setText(text);
}

Led::~Led()
{
}

void Led::setMode(mode modt){
	
	mod=modt;
	if(attr==Green){ 
		if(mod==On)icon=new QIcon("../icon/greenOn.png");
		else icon=new QIcon("../icon/greenOff.png");
	}
	if(attr==Red) {
		if(mod==On)icon=new QIcon("../icon/redOn.png");
		else icon=new QIcon("../icon/redOff.png");
	}	
	if(attr==Yellow) {
		if(mod==On)icon=new QIcon("../icon/yellowOn.png");
		else icon=new QIcon("../icon/yellowOff.png");
	}
	QPixmap pixmap = icon->pixmap(QSize(18, 18),QIcon::Normal, QIcon::On);
	lab->setPixmap(pixmap);
}

void Led::setColor(color attrt){
	
	attr=attrt;
	if(attr==Green) {
		if(mod==On)icon=new QIcon("../icon/greenOn.png");
		else icon=new QIcon("../icon/greenOff.png");
	}
	if(attr==Red){ 
		if(mod==On)icon=new QIcon("../icon/redOn.png");
		else icon=new QIcon("../icon/redOff.png");
	}	
	if(attr==Yellow) {
		if(mod==On)icon=new QIcon("../icon/yellowOn.png");
		else icon=new QIcon("../icon/yellowOff.png");
	}
	QPixmap pixmap = icon->pixmap(QSize(18, 18),QIcon::Normal, QIcon::On);
	lab->setPixmap(pixmap);
}