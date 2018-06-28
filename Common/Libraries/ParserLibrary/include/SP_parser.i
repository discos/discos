#define _SP_MAXLENGTH 20
#define TIMERIDENTIFIER '!'
#define PROCEDUREPARAMETER '$'

template <class OBJ>
void  CParser<OBJ>::run(const IRA::CString& command,IRA::CString& out) throw (ParserErrors::ParserErrorsExImpl,ACSErr::ACSbaseExImpl)
{
	IRA::CString instr; 
	if ((command=="") || (command=="\n")) {
		out="";
		return;
	}
	try {
		out=executeCommand(command,instr);
	}
	catch (ParserErrors::RemoteCommandErrorExImpl& ex) { // note: this is the case an error is found when a remote call is done...the exception is used to signal the error and to transport the corresponding error message
		out=ex.getRemoteCommandMessage();
		throw ex;
	}
	catch (ParserErrors::ParserErrorsExImpl& ex) {
		IRA::CString msg;
		SimpleParser::CFormatter<ParserErrors::ParserErrorsExImpl>::exceptionToUser(ex,msg);
		//_EXCPT_TO_CSTRING(msg,ex);
		out=instr+m_errorDelimiter+msg;
		throw ex;
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		IRA::CString msg;
		//_EXCPT_TO_CSTRING(msg,ex);
		SimpleParser::CFormatter<ACSErr::ACSbaseExImpl>::exceptionToUser(ex,msg);
		out=instr+m_errorDelimiter+msg;
		throw ex;
	}
}

template <class OBJ>
void CParser<OBJ>::runAsync(const IRA::CString& command,_SP_CALLBACK(callBack),const void * callBackParam)  throw (ParserErrors::ParserErrorsExImpl)
{
	TRule *elem;
	bool timeTagged;
	WORD parNum;
	IRA::CString timeCommand;
	ACS::Time execTime;
	ACS::TimeInterval execInterval;
	IRA::CString inParams[_SP_MAXLENGTH];
	IRA::CString instr;
	elem=checkCommand(command,instr,inParams,parNum,timeTagged,timeCommand,execTime,execInterval); // throws exceptions
	if (elem->m_type==PROCEDURE){
		pushProcedure(instr,elem->m_procedure,inParams,parNum,callBack,callBackParam); // throws ProcedureErrorExImpl
	}
	else {
		pushCommand(command,elem->m_package,callBack,callBackParam);
	}
}

template<class OBJ>
IRA::CString CParser<OBJ>::executeCommand(const IRA::CString& command,IRA::CString& instr) throw (ParserErrors::SyntaxErrorExImpl,ParserErrors::CommandNotFoundExImpl,
		ParserErrors::NotEnoughParametersExImpl,ParserErrors::SystemCommandErrorExImpl,ParserErrors::ProcedureErrorExImpl,ParserErrors::TimeFormatErrorExImpl,ParserErrors::ParserTimerErrorExImpl,
		ParserErrors::NotSupportedErrorExImpl,ParserErrors::TooManyParametersExImpl,ParserErrors::ConversionErrorExImpl,ParserErrors::PackageErrorExImpl,ParserErrors::RemoteCommandErrorExImpl,ACSErr::ACSbaseExImpl)
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
	IRA::CString logCommand;
	elem=checkCommand(command,instr,inParams,parNum,timeTagged,timeCommand,execTime,execInterval); // throws exceptions
	logCommand=command;
	logCommand.RTrim();
	if (!timeTagged) {   //sync operations
		if (elem->m_type==PROCEDURE){
			pushProcedure(instr,elem->m_procedure,inParams,parNum); // throws ProcedureErrorExImpl
			CUSTOM_LOG(LM_FULL_INFO,"CParser::executeCommand()",(LM_NOTICE,"Procedure issued {%s}",(const char *)logCommand));
			return instr+m_answerDelimiter;  //it informs that the procedure seems to be correct and it will be executed 
		}	
		else if (elem->m_type==COMMAND) {
			CUSTOM_LOG(LM_FULL_INFO,"CParser::executeCommand()",(LM_NOTICE,"Command issued {%s}",(const char *)logCommand));
			// polimorphic cast...allowed because the CFunctor derives from CBaseFunction
			CFunctor<OBJ> * function = dynamic_cast<CFunctor<OBJ> *>(elem->m_func);
			//try {
			function->call(inParams,parNum);  //throw ConversionErrorExImpl, ACSbaseExImpl
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
		else if (elem->m_type==SYSTEMCALL) {
			CUSTOM_LOG(LM_FULL_INFO,"CParser::executeCommand()",(LM_NOTICE,"Command issued {%s}",(const char *)command));
			//work around in order to avoid message proliferation on stderr stdout
			IRA::CString composeCall("export ACS_LOG_STDOUT=11; "); 
			IRA::CString answer("");
			composeCall+=elem->m_syscall;
			for (WORD k=0;k<parNum;k++) {
				composeCall+=" ";
				composeCall+=inParams[k];
			}
			/*if (system((const char *)composeCall)<0) {
				_EXCPT(ParserErrors::SystemCommandErrorExImpl,err,"CParser::executeCommand()");
				err.setSystemCommand((const char *)composeCall);
				throw err;
			}*/
			redi::ipstream proc((const char *)composeCall,redi::pstreams::pstdout|redi::pstreams::pstderr);
  			std::string line;
  			while (std::getline(proc.err(),line)) answer+=line.c_str();
  			if (answer.Left(6)=="error ") {
  				int len=answer.GetLength();
  				answer=answer.Right(len-6);
  				return instr+m_errorDelimiter+answer;
  			}
  			else {
  				return instr+m_answerDelimiter+answer;
  			}
		}
		else if (elem->m_type==KEYCOMMAND) {
			if (instr==_SP_TIMETAGGEDQUEUE) {
				return instr+m_answerDelimiter+showTimeBasedCommands();
			}
			if (instr==_SP_TIMETAGGEDQUEUECLEAR) {
				int pos=inParams[0].ToInt();
				if (pos<0) {
					_EXCPT(ParserErrors::ConversionErrorExImpl,impl,"CParser::executeCommand()");
					throw impl;
				}
				if ((pos==0) && (inParams[0]!="0")) {
					_EXCPT(ParserErrors::ConversionErrorExImpl,impl,"CParser::executeCommand()");
					throw impl;
				}
				flushTimeBasedCommands(pos);
				return instr+m_answerDelimiter;
			}
			if (instr==_SP_TIMETAGGEDQUEUECLEARALL) {
				flushTimeBasedCommands(-1);
				return instr+m_answerDelimiter;
			}
			else {
				_EXCPT(ParserErrors::CommandNotFoundExImpl,impl,"CParser::executeCommand()");
				throw impl;
			}
		}
		else { //REMOTECOMMAND
			_SP_REMOTECALL(pointer)=elem->m_remoteFunc;
			IRA::CString answer;
			bool res=(*m_object.*pointer)(command,elem->m_package,elem->m_param,answer); //throw ParserErrors::PackageErrorExImpl
			if (res) {
				return answer;
			}
			else {
				//this is a cheat to pass to the run function the message coming from the remote command execution in case of error and inform the error occurred
				_EXCPT(ParserErrors::RemoteCommandErrorExImpl,impl,"CParser::executeCommand()");
				impl.setRemoteCommandMessage((const char *)answer);
				throw impl;
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
				else {
					m_timerInitialized=true;
				}
			}
			TTimerJob *job=new TTimerJob(this,timeCommand);  //freed by the timer
			if (!m_timer.schedule(&timerHandler,execTime,0,job,&timerCleanup)) {
				_EXCPT(ParserErrors::ParserTimerErrorExImpl,err,"CParser::executeCommand()");
				throw err;
			}
		}
		else if (execInterval!=0) {  //continuous operation
			if (!m_timerInitialized) {
				if (!m_timer.init()) {
					_EXCPT(ParserErrors::ParserTimerErrorExImpl,err,"CParser::executeCommand()");
					throw err;
				}
				else {
					m_timerInitialized=true;
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
		ACS::TimeInterval& execInterval)  throw (ParserErrors::SyntaxErrorExImpl,ParserErrors::CommandNotFoundExImpl,ParserErrors::NotEnoughParametersExImpl,ParserErrors::TimeFormatErrorExImpl,
		ParserErrors::NotSupportedErrorExImpl,ParserErrors::TooManyParametersExImpl)
{
	IRA::CString timeMark;
	TRule *elem;
	int result=parseCommand(line,instr,inParams,timeTagged,timeCommand,timeMark,m_maxArgs);
	if (result<0) {
		_EXCPT(ParserErrors::SyntaxErrorExImpl,err,"CParser::checkCommand()");
		throw err;
	}
	if ((timeTagged) && (!m_levelTwoSupport)) {
		_EXCPT(ParserErrors::NotSupportedErrorExImpl,err,"CParser::checkCommand()");
		throw err;
	}
	elem=locate(instr);
	parNum=result;
	if (elem==NULL) {
		_EXCPT(ParserErrors::CommandNotFoundExImpl,err,"CParser::checkCommand()");
		throw err;
	}
	if ((elem->m_type==KEYCOMMAND) && (!m_levelTwoSupport)) {
		_EXCPT(ParserErrors::NotSupportedErrorExImpl,err,"CParser::checkCommand()");
		throw err;
	}
	if (timeTagged) {
		if (!parseTime(timeMark,execTime,execInterval)) {
			_EXCPT(ParserErrors::TimeFormatErrorExImpl,err,"CParser::checkCommand()");
			throw err;
		}
	}
	if ((elem->m_type==COMMAND) || (elem->m_type==PROCEDURE) || (elem->m_type==SYSTEMCALL) || (elem->m_type==KEYCOMMAND)) {
		if (result<elem->m_inputParametersNumber) {
			_EXCPT(ParserErrors::NotEnoughParametersExImpl,err,"CParser::checkCommand()");
			throw err;
		}
		if (result>elem->m_inputParametersNumber) {
			_EXCPT(ParserErrors::TooManyParametersExImpl,err,"CParser::checkCommand()");
			throw err;			
		}
	}
	return elem;
}

template <class OBJ>
typename CParser<OBJ>::TRule * CParser<OBJ>::locate(const IRA::CString& instr)
{
	typename TRuleSet::const_iterator it;
	for(it=m_ruleSet.begin(); it!=m_ruleSet.end();it++) {
		if ((*it)->m_name==instr) {
			return *it;
		}
	}
	baci::ThreadSyncGuard guard(&m_extraRuleSetMutex);
	for(it=m_extraRuleSet.begin(); it!=m_extraRuleSet.end();it++) {
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
			while (run && data->popCommand(elem)) {  // if run is false the popCommand is not executed so the next command in the list is not taken out
				_SP_CALLBACK(cb)=elem->m_callBack;
				try {	
					data->executeCommand(elem->m_command,instr);
				}
				catch (ParserErrors::ParserErrorsExImpl& ex) {
					if (cb!=NULL) (*cb)(elem->m_parameter,elem->m_name,false);
					// the error related to the parser are not logged, they just appears as error messages to the user operator input. In that case the command is asynchronous, so nothing is returned to the user
					//ex.log(LM_ERROR);
				}
				catch (ACSErr::ACSbaseExImpl& ex) {
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
		//ex.log(LM_ERROR); // no logging.....
	}	
	catch (ACSErr::ACSbaseExImpl& ex) {
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
void CParser<OBJ>::pushProcedure(const IRA::CString& name,const ACS::stringSeq& procedure,IRA::CString *procPrm,WORD& parNumber,
		_SP_CALLBACK(callBack),const void* parameter) throw (ParserErrors::ProcedureErrorExImpl)
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
	for(unsigned i=0;i<procedure.length();i++) {  //check procedure correctness
		//replace the place holders with the procedure arguments
		IRA::CString line((const char *)procedure[i]);
		for (WORD j=0;j<parNumber;j++) {
			IRA::CString pp;
			//pp.Format("%c%d",PROCEDUREPARAMETER,j);
			pp.Format("%c%d",PROCEDUREPARAMETER,j+1);
			line.ReplaceAll((const char *)pp,procPrm[j]);
		}
		try {
			elem=checkCommand((const char *)line,instr,inParams,parNum,timeTagged,timeCommand,execTime,execInterval);
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
			pushProcedure(instr,elem->m_procedure,inParams,parNum,callBack,parameter); // throws an exception
		}
		else {
			if (i==procedure.length()-1) {  /// the call back is propagated only for the last operation
				pushCommand((const char *)line,name,callBack,parameter);
			}
			else {
				pushCommand((const char *)line,name);
			}
		}
	}
}

template <class OBJ>
void CParser<OBJ>::pushCommand(const IRA::CString& line,const IRA::CString& package,_SP_CALLBACK(callBack),const void* parameter)
{
	baci::ThreadSyncGuard guard(&m_exeListMutex);
	m_executionList.push_back(new TExecutionUnit(line,package,callBack,parameter));
}

template <class OBJ>
bool CParser<OBJ>::popCommand(TExecutionUnit *& cmd)
{
	baci::ThreadSyncGuard guard(&m_exeListMutex);
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
bool CParser<OBJ>::checkStation(const IRA::CString& st)
{
	if (st=="ALL") {
		return true;
	}
	IRA::CString env(std::getenv("STATION"));
	if (env==st) return true;
	else return false;
}

template <class OBJ>
IRA::CString CParser<OBJ>::showTimeBasedCommands()
{
	unsigned iterator=0;
	ACS::Time time;
	ACS::TimeInterval interval;
	const void *param;
	IRA::CString out(""),position;
	IRA::CString timeStr,intervalStr;
	TTimerJob* job;
	while (m_timer.getNextEvent(iterator,time,interval,param)) {
		job=(TTimerJob *)param;
		if (time!=0) {
			IRA::CIRATools::timeToStr(time,timeStr);
		}
		else timeStr="0000-000-00:00:00.00";
		IRA::CIRATools::intervalToStr(interval,intervalStr);
		timeStr=IRA::CString(m_timeDelimiter)+timeStr;
		intervalStr=IRA::CString(TIMERIDENTIFIER)+intervalStr;
		position.Format("%u",iterator-1);
		out+="\n"+position+" "+timeStr+" "+intervalStr+" "+job->m_command;
	}
	return out;
}

template <class OBJ>
void CParser<OBJ>::flushTimeBasedCommands(const int& pos)
{
	if (pos<0) {
		m_timer.cancelAll();
	}
	else {
		m_timer.cancel((unsigned)pos);
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
		if (getNextToken(line,i,m_timeDelimiter,token)) { //check if the time delimiter is present
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
	else {
		return 0;  //if no command delimiter is present, the command is considered to be the whole line
	}
	while (getNextToken(newLine,i,m_delimiter,token)) {
		ok=true;
		/*if (token==m_jollyChar) {
			pars[n]=m_jollyCharReplacement;
		}
		else {
			pars[n]=token;
		}*/
		pars[n]=token;
		n++;
		if (n>=maxPars) return maxPars;
	}
	if (ok) return n;
	else return -1;
}
