template<class EX>
void CFormatter<EX>::exceptionToUser(EX& exObj,IRA::CString& output)
{
	IRA::CString tmp;
	exObj.top(); 
	tmp.Format("(type:%ld code:%ld): %s",exObj.getErrorType(),exObj.getErrorCode(),
		(const char *)getMessage(exObj));
	output=tmp;
	while (exObj.getNext()!=NULL) {
		tmp.Format("(type:%ld code:%ld): %s",exObj.getErrorType(),exObj.getErrorCode(),
			(const char *)getMessage(exObj));
		output+="\n";
		output+=tmp;
	}
	exObj.top();
}

template<class EX>
IRA::CString CFormatter<EX>::getMessage(EX& exObj)
{
	char *descr;
	ACE_CString msg=exObj.getData(USER_MESSAGE_FIELDNAME);
	if (msg.length()==0) {
		descr=exObj.getDescription();
		IRA::CString ret(descr);
		CORBA::string_free(descr);
		return ret;
	}
	else {
		IRA::CString ret(msg);
		return ret;
	}
}