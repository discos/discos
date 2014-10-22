// $Id: TW_Callbacks.i,v 1.5 2009-05-06 14:06:53 a.orlati Exp $

template <class CT,class POA,bool ISSEQ>
CStandardCallback<CT,POA,ISSEQ>::CStandardCallback() :  
  CCommonCallback(ISSEQ), m_guardInterval(0), m_workingGuard(m_guardInterval)
{
}

template <class CT,class POA,bool ISSEQ>
CStandardCallback<CT,POA,ISSEQ>::~CStandardCallback()
{
}

template <class CT,class POA,bool ISSEQ>
void CStandardCallback<CT,POA,ISSEQ>::setGuardInterval(const DDWORD& interval) 
{ 
	if (isStopped()) {
		m_guardInterval=interval;
		m_workingGuard.changeInterval(interval);
	}
}

template <class CT,class POA,bool ISSEQ>
void CStandardCallback<CT,POA,ISSEQ>::working(RCT value,const ACSErr::Completion &c,const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
	CompletionImpl comp(c);
	if (comp.isErrorFree()) {
		if (!isStopped()) {
			Action(value);
		}
	}
	else {
		IRA::CString outMsg;
		_COMPL_TO_CSTRING(outMsg,comp);
		CFrameComponent *outComp=getFrameComponent()->getOutputComponent();
		if (outComp!=NULL) {
			outComp->setValue(outMsg);
			outComp->Refresh();
		}
		_IRA_LOGGUARD_LOG_COMPLETION(m_workingGuard,comp,LM_ERROR);
		//comp.log();
	}
}

template <class CT,class POA,bool ISSEQ>
void CStandardCallback<CT,POA,ISSEQ>::Action(RCT value)
{
	IRA::CString tmp;
	if (!m_isSequence) {
		baci::BACIValue val( _tw_getTypeValue(value,0));
		tmp=m_formatFunction(val,m_formatArgument);
	}
	else {
		IRA::CString format;
		tmp="";
		long len=_tw_getTypeLength(value);
		for (long i=0;i<len;i++) {
			baci::BACIValue val( _tw_getTypeValue(value,i));
			format=m_formatFunction(val,m_formatArgument);
			if (i==0) {
				tmp+=format;
			}
			else {
				tmp+=" "+format;
			}
		}
	}
	getFrameComponent()->setValue(tmp);
	getFrameComponent()->Refresh();
}

template <class CT,class POA,bool ISSEQ>
void CStandardCallback<CT,POA,ISSEQ>::done(RCT value,const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
}
