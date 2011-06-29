/* $Id: SP_parser.i,v 1.15 2011-03-28 10:06:40 a.orlati Exp $											*/

#define _SP_MAXLENGTH 20
#define TIMERIDENTIFIER '!'

template <class OBJ>
void  CParser<OBJ>::run(const IRA::CString& command,IRA::CString& out) throw (ParserErrors::ParserErrorsExImpl)
{
	IRA::CString instr; 
	if ((command=="") || (command=="\n")) {
		out="";
		return;
	}
	try {
		out=executeCommand(command,instr);
	}
	catch (ParserErrors::ParserErrorsExImpl& ex) {
		IRA::CString msg;
		_EXCPT_TO_CSTRING(msg,ex);
		out=instr+m_errorDelimiter+msg;
		throw ex;
	}
}
template <class OBJ>
void CParser<OBJ>::inject(const IRA::CString& name,const ACS::stringSeq& procedure,_SP_CALLBACK(callBack),const void * callBackParam) throw (ParserErrors::ProcedureErrorExImpl)
{
	pushProcedure(name,procedure,callBack,callBackParam);
}

template<class OBJ>
IRA::CString CParser<OBJ>::executeCommand(const IRA::CString& command,IRA::CString& instr) throw (ParserErrors::SyntaxErrorExImpl,ParserErrors::CommandNotFoundExImpl,
		ParserErrors::NotEnoughParametersExImpl,ParserErrors::ExecutionErrorExImpl,ParserErrors::ProcedureErrorExImpl,ParserErrors::TimeFormatErrorExImpl,ParserErrors::ParserTimerErrorExImpl,
		ParserErrors::NotSupportedErrorExImpl,ParserErrors::TooManyParametersExImpl)
{
	TRule *elem;
	WORD outNumber;
	bool timeTagged;
	WORD parNum;
	IRA::CString timeCommand;
	ACS::Time execTime;
	ACS::TimeInterval execInterval;
	IRA::CString inParams[_SP_MAXLENGTH];
	IRA::CString outParams[_SP_MAXLENGTH];
	elem=checkCommand(command,instr,inParams,parNum,timeTagged,timeCommand,execTime,execInterval); // throws exceptions
	if (!timeTagged) {   //sync operations
		if (elem->m_type==PROCEDURE){
			pushProcedure(instr,elem->m_procedure); // throws ProcedureErrorExImpl
			return instr+m_answerDelimiter;  //it informs that the procedure seems to be correct and it will be executed 
		}	
		else if (elem->m_type==COMMAND) {
			// polimorphic cast...allowed because the CFunctor derives from CBaseFunction
			CFunctor<OBJ> * function = dynamic_cast<CFunctor<OBJ> *>(elem->m_func);
			try {
				function->call(inParams,parNum);
			}
			catch (ACSErr::ACSbaseExImpl& ex) {
				_ADD_BACKTRACE(ParserErrors::ExecutionErrorExImpl,err,ex,"CParser::executeCommand()");
				err.setCommand((const char *)command);
				throw err;
			}
			outNumber=function->get(outParams);
			IRA::CString answer("");
			if (outNumber>0) {
				for(int j=0;j<outNumber;++j) {
					answer+=outParams[j];
					answer+=IRA::CString(m_answerDelimiter);
				}
			}
			return instr+m_answerDelimiter+answer;
		}
		else { //REMOTECOMMAND
			try {
				_SP_REMOTECALL(pointer)=elem->m_remoteFunc;
				return (*m_object.*pointer)(command,elem->m_package,elem->m_param);
			}
			catch (ParserErrors::ExecutionErrorExImpl& ex) {
				throw ex;
			}
			catch (ParserErrors::PackageErrorExImpl& ex) {
				throw ex;
			}
		}
	}
	else { //async operations
		if ((execTime==0) && (execInterval==0)) { //execution is demanded to the internal thread and it will be done as soon as possible
			pushCommand(timeCommand,elem->m_package);
		}
		else if (execTime!=0) {  //time tagged operation
			if (!m_timerInitialized) {
				if (!m_timer.init()) {
					_EXCPT(ParserErrors::ParserTimerErrorExImpl,err,"CParser::executeCommand()");
					throw err;
				}
			}
			TTimerJob *job=new TTimerJob(this,timeCommand);  //freed by the timer
			if (!m_timer.schedule(&timerHandler,execTime,0,job,&timerCleanup)) {
				_EXCPT(ParserErrors::ParserTimerErrorExImpl,err,"CParser::executeCommand()");
				throw err;
			}
		}
		else if (execInterval!=0) {  //continuos operation
			if (!m_timerInitialized) {
				if (!m_timer.init()) {
					_EXCPT(ParserErrors::ParserTimerErrorExImpl,err,"CParser::executeCommand()");
					throw err;
				}
			}
			TIMEVALUE now;
			IRA::CIRATools::getTime(now);
			TTimerJob *job=new TTimerJob(this,timeCommand); //freed by the timer
			if (!m_timer.schedule(&timerHandler,now.value().value,execInterval,job,&timerCleanup)) {
				_EXCPT(ParserErrors::ParserTimerErrorExImpl,err,"CParser::executeCommand()");
				throw err;
			}
		}
		return instr+m_answerDelimiter;
	}
}

template<class OBJ>
typename CParser<OBJ>::TRule *CParser<OBJ>::checkCommand(const IRA::CString& line,IRA::CString& instr,IRA::CString* inParams,WORD& parNum,bool& timeTagged,IRA::CString& timeCommand,ACS::Time& execTime,
		ACS::TimeInterval& execInterval) const throw (ParserErrors::SyntaxErrorExImpl,ParserErrors::CommandNotFoundExImpl,ParserErrors::NotEnoughParametersExImpl,ParserErrors::TimeFormatErrorExImpl,
		ParserErrors::NotSupportedErrorExImpl,ParserErrors::TooManyParametersExImpl)
{
	IRA::CString timeMark;
	TRule *elem;
	int result=parseCommand(line,instr,inParams,timeTagged,timeCommand,timeMark,m_maxArgs);
	if (result<0) {
		_EXCPT(ParserErrors::SyntaxErrorExImpl,err,"CParser::checkCommand()");
		throw err;
	}
	if ((timeTagged) && (!m_asyncSupport)) {
		_EXCPT(ParserErrors::NotSupportedErrorExImpl,err,"CParser::checkCommand()");
		throw err;
	}
	elem=locate(instr);
	parNum=result;
	if (elem==NULL) {
		_EXCPT(ParserErrors::CommandNotFoundExImpl,err,"CParser::checkCommand()");
		throw err;
	}
	if (timeTagged) {
		if (!parseTime(timeMark,execTime,execInterval)) {
			_EXCPT(ParserErrors::TimeFormatErrorExImpl,err,"CParser::checkCommand()");
			throw err;
		}
	}
	if (elem->m_type==COMMAND) {
		if (result<elem->m_arity) {
			_EXCPT(ParserErrors::NotEnoughParametersExImpl,err,"CParser::checkCommand()");
			throw err;
		}
		if (result>elem->m_arity) {
			_EXCPT(ParserErrors::TooManyParametersExImpl,err,"CParser::checkCommand()");
			throw err;			
		}
	}
	return elem;
}

template <class OBJ>
typename CParser<OBJ>::TRule * CParser<OBJ>::locate(const IRA::CString& instr) const
{
	typename TRuleSet::const_iterator it;
	for(it=m_ruleSet.begin(); it!=m_ruleSet.end();it++) {
		if ((*it)->m_name==instr) {
			return *it;
		}
	}
	return NULL;
}

template <class OBJ>
void CParser<OBJ>::worker(void *threadParam)
{
	CParser<OBJ>* data;
	bool run;
	TExecutionUnit *elem;
	IRA::CString instr;
	TIMEVALUE start,stop;
	//get access to the thread this function is being executed from
	ACS::ThreadBaseParameter *baciParameter=static_cast<ACS::ThreadBaseParameter *>(threadParam);
	ACS::ThreadBase *myself=baciParameter->getThreadBase();
	// Init the thread
	if (ACS::ThreadBase::InitThread!=0)  {
		ACS::ThreadBase::InitThread(THREAD_NAME);
	}
	data=(CParser<OBJ> *)baciParameter->getParameter();
	ACS::TimeInterval res=myself->getResponseTime();
	DDWORD response=res/10; //response time in microseconds
	while(myself->check()==true) {
		if (myself->isSuspended()==false) {
			run=true;
			IRA::CIRATools::getTime(start);
			while (run && data->popCommand(elem)) {  // if run if false the popCommand is not executed so the next command in the list is not taken out
				_SP_CALLBACK(cb)=elem->m_callBack;
				try {	
					data->executeCommand(elem->m_command,instr);
				}
				catch (ParserErrors::ParserErrorsExImpl& ex) {
					if (cb!=NULL) (*cb)(elem->m_parameter,elem->m_name,false);
					ex.log(LM_ERROR);
				}
				if (cb!=NULL) (*cb)(elem->m_parameter,elem->m_name,true);
				IRA::CIRATools::getTime(stop);
				if (IRA::CIRATools::timeDifference(start,stop)>=response) {
					run=false;
				}
			}
		}
		myself->sleep();
	}
	if (ACS::ThreadBase::DoneThread!=0) {
		ACS::ThreadBase::DoneThread();
	}
	myself->setStopped();
	delete baciParameter;
}

template <class OBJ>
void CParser<OBJ>::timerHandler(const ACS::Time& time,const void * par)
{
	IRA::CString instr;
	TTimerJob* exec=(TTimerJob *)par;
	try {	
		exec->m_parser->executeCommand(exec->m_command,instr);
	}
	catch (ParserErrors::ParserErrorsExImpl& ex) {
		ex.log(LM_ERROR);
	}	
}

template <class OBJ>
void CParser<OBJ>::timerCleanup(const void * par)
{
	if (par!=NULL) {
		TTimerJob* exec=(TTimerJob *)par;
		delete exec;
	}
}

template <class OBJ>
void CParser<OBJ>::pushProcedure(const IRA::CString& name,const ACS::stringSeq& procedure,_SP_CALLBACK(callBack),const void* parameter) throw (ParserErrors::ParserErrorsExImpl)
{
	TRule *elem;
	IRA::CString instr;
	bool timeTagged;
	WORD parNum;
	IRA::CString timeCommand;
	ACS::Time execTime;
	ACS::TimeInterval execInterval;
	IRA::CString inParams[_SP_MAXLENGTH];
	if (procedure.length()==0) { // if the procedure contains no commands...it is skipped and the callback is called with success status
		if (callBack!=NULL) (*callBack)(parameter,name,true);
		return;
	}
	for(unsigned i=0;i<procedure.length();i++) {  //check procedure correcteness
		try {
			elem=checkCommand((const char *)procedure[i],instr,inParams,parNum,timeTagged,timeCommand,execTime,execInterval);
		}
		catch (ParserErrors::ParserErrorsExImpl& ex) {
			_ADD_BACKTRACE(ParserErrors::ProcedureErrorExImpl,impl,ex,"CParser::pushProcedure()");
			impl.setCommand((const char *)procedure[i]);
			impl.setProcedureName((const char *)name); //instr contains the name of the procedure 
			// in case of error the callback is called immediately because the procedure execution is not continued any more
			if (callBack!=NULL) (*callBack)(parameter,name,false); 
			throw impl;
		}
		if (elem->m_type==PROCEDURE) {
			pushProcedure(instr,elem->m_procedure,callBack,parameter); // throws an exception
		}
		else {
			if (i==procedure.length()-1) {  /// the call back is propageted only for the last operation
				pushCommand((const char *)procedure[i],name,callBack,parameter);
			}
			else {
				pushCommand((const char *)procedure[i],name);
			}
		}
	}
}

template <class OBJ>
void CParser<OBJ>::pushCommand(const IRA::CString& line,const IRA::CString& package,_SP_CALLBACK(callBack),const void* parameter)
{
	baci::ThreadSyncGuard guard(&m_localMutex);
	m_executionList.push_back(new TExecutionUnit(line,package,callBack,parameter));
}

template <class OBJ>
bool CParser<OBJ>::popCommand(TExecutionUnit *& cmd)
{
	baci::ThreadSyncGuard guard(&m_localMutex);
	if (m_executionList.empty()) {
		return false;
	}
	else {
		cmd=m_executionList.front();
		m_executionList.pop_front();
		return true;
	}	
}

template <class OBJ>
bool CParser<OBJ>::getNextToken(const IRA::CString& str,int &start,char delimiter,IRA::CString &ret) const
{
		int i;
		bool ok=false;
		char *token;
		i=str.GetLength();
		token=new char[i+1];
		i=0;
		while ((str[start]!=0) && (str[start]!='\n')) {
			ok=true;
			if ((str[start]==delimiter)) {
				start++;
				break;
			}
			else {
				token[i]=str[start];
				start++;
				i++;
			}
		}
		token[i]=0;
		ret=IRA::CString(token);
		delete []token;
		if (!ok) return false;
		else return true;
}

template<class OBJ>
bool CParser<OBJ>::parseTime(const IRA::CString& timeMark,ACS::Time& executionTime,ACS::TimeInterval& interval) const
{
	if (timeMark=="") {  // 
		executionTime=0;
		interval=0;
		return true;
	}
	else {  
		if (timeMark[0]==TIMERIDENTIFIER) {
			IRA::CString tt;
			tt=timeMark.Mid(1,timeMark.GetLength()-1); //skip the first character
			if (!IRA::CIRATools::strToInterval(tt,interval,false)) {
				return false;
			}
			executionTime=0;
			return true;
		}
		else {
			if (!IRA::CIRATools::strToTime(timeMark,executionTime,false)) {
				return false;
			}
			interval=0;
			return true;
		}
	}
}

template<class OBJ>
int CParser<OBJ>::parseCommand(const IRA::CString& line,IRA::CString& instr,IRA::CString* pars,bool& timeTagged,IRA::CString& timeCommand,IRA::CString& timeMark,const int& maxPars) const
{
	IRA::CString token;
	IRA::CString newLine;
	bool ok=false;
	int i=0,n=0;
	if (line.Find(m_timeDelimiter,0)>0) {
		if (getNextToken(line,i,m_timeDelimiter,token)) { //chek if the time delimiter is present
			timeTagged=true;
			if (i!=line.GetLength()) {  // the timeCommand has no time mark, so the command is executed async, as soon as possible
				timeMark=line.Right(line.GetLength()-i);
				if (timeMark=="\n") timeMark="";
			}
			else {
				timeMark="";
			}
			timeCommand=token;
			newLine=token;
		}
	}
	else {
		timeTagged=false;
		timeMark="";
		timeCommand="";
		newLine=line;
	}
	i=0;
	instr="";
	if (getNextToken(newLine,i,m_commandDelimiter,token)) {
		instr=token;
		ok=true;
	}
	else return -1;
	while (getNextToken(newLine,i,m_delimiter,token)) {
		ok=true;
		if (token==m_jollyChar) {
			pars[n]=m_jollyCharReplacement;
		}
		else {
			pars[n]=token;
		}
		n++;
		if (n>=maxPars) return maxPars;
	}
	if (ok) return n;
	else return -1;
}
