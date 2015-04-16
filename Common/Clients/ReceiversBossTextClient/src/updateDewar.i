

#ifndef _UPDATEDEWAR_I_
#define _UPDATEDEWAR_I_

void updateDewar(Receivers::DewarPositioner_ptr dewar);

void updateDewar(Receivers::DewarPositioner_ptr dewar)
{
	char *corbaStr;
	IRA::CString configuration;
	bool isRewRequired,isRewinding;
	double position,iStaticPos,iParPos,dParPos;
	bool isReady;
	Receivers::TDewarPositionerScanInfo_var info;
	IRA::CString outString,format;
	if (CORBA::is_nil(dewar)) {
		outString="                 ----- Not Available ----- ";
		extraLabel3->setValue(outString);
		extraLabel3->setStyle(TW::CStyle(TW::CColorPair::GREEN_BLACK,0));
		extraLabel3->Refresh();
		return;
	}
	try {
		if (dewar->isConfigured()) {
			corbaStr=dewar->getConfiguration();
			configuration=corbaStr;
			CORBA::string_free(corbaStr);
			isRewRequired=dewar->isRewindingRequired();
			isRewinding=dewar->isRewinding();
			position=dewar->getPosition();
			isReady=dewar->isReady();
			info=dewar->getScanInfo();
			iStaticPos=info->iStaticPos;
			iParPos=info->iParallacticPos;
			dParPos=info->dParallacticPos;
		}
		else {
			configuration="none";
			isRewRequired=false;
			isRewinding=false;
			position=0.0;
			isReady=true;
			iStaticPos=iParPos=dParPos=0;
		}
	}
	catch (...) {
		/*_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()");
		_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);*/
		return;
	}
	outString="Configuration: "+configuration;
	extraLabel1->setValue(outString);
	extraLabel1->setStyle(TW::CStyle(TW::CColorPair::WHITE_BLACK,0));
	extraLabel1->Refresh();
	if (isReady) {
		outString="Dewar is ready";
		extraLabel2->setValue(outString);
		extraLabel2->setStyle(TW::CStyle(TW::CColorPair::GREEN_BLACK,0));
		extraLabel2->Refresh();
	}
	else {
		outString="Dewar not ready";
		extraLabel2->setValue(outString);
		extraLabel2->setStyle(TW::CStyle(TW::CColorPair::RED_BLACK,0));
		extraLabel2->Refresh();
	}
	format.Format("%05.1lf",position);
	outString="Derotator Position: "+format;
	extraLabel3->setValue(outString);
	extraLabel3->setStyle(TW::CStyle(TW::CColorPair::WHITE_BLACK,0));
	extraLabel3->Refresh();
	format.Format("%05.1lf / %05.1lf / %05.1lf",iStaticPos,iParPos,dParPos);
	outString="Dewar Position: "+format;
	extraLabel4->setValue(outString);
	extraLabel4->setStyle(TW::CStyle(TW::CColorPair::WHITE_BLACK,0));
	extraLabel4->Refresh();
	if (isRewRequired) {
		if (isRewinding) {
			outString="Rewinding required (rewinding...)";
			extraLabel5->setValue(outString);
			extraLabel5->setStyle(TW::CStyle(TW::CColorPair::YELLOW_BLACK,0));
			extraLabel5->Refresh();
		}
		else { 
			outString="Rewinding required";
			extraLabel5->setValue(outString);
			extraLabel5->setStyle(TW::CStyle(TW::CColorPair::RED_BLACK,0));
			extraLabel5->Refresh();
		}
	}
	else {
		outString="Rewinding not required";
		extraLabel5->setValue(outString);
		extraLabel5->setStyle(TW::CStyle(TW::CColorPair::GREEN_BLACK,0));
		extraLabel5->Refresh();
	}
}

#endif

