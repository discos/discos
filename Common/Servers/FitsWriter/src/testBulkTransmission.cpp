// $Id: testBulkTransmission.cpp,v 1.9 2011-04-15 09:08:40 a.orlati Exp $

#include <GenericBackendC.h>
#include <DataReceiverC.h>
#include <FitsWriterC.h>
#include <IRA>
#include <maciSimpleClient.h>
#include <ClientErrors.h>
#include <getopt.h>
#include <ACSBulkDataError.h>

using namespace IRA;

bool done;

#define GAP 3
#define MIN_DURATION 2

void printHelp() {
	printf("Test fits writing throught the bulk data channel\n");
	printf("\n");
	printf("testBulk [-h] -b backend [-w writer] [-f filename] [-p path] [-o observer] \n");
	printf("\n");
	printf("[-h]                 Shows this help\n");
	printf("-b backend  It gives the name of the backend that provides tha data\n");
	printf("[-w writer]   It gives the name of the writer or data receiver, if not givet the default fits writer is considered\n");
	printf("[-f filename]   Prefix of the file name that will be output. The complete file name is obtained by appending the time of file creation\n");
	printf("[-p path]         Directory in which the file will be stored. Default is ~\n");
	printf("[-o observer] The value that will be put in the ObserverName field of the file. Default is dummy\n");
}

int main(int argc, char *argv[]) 
{
	char input[256];
	IRA::CString fileName;
	ACE_Time_Value tv(MIN_DURATION);
	Backends::GenericBackend_var sender;
	Management::DataReceiver_var recv;
	maci::SimpleClient client;
	TIMEVALUE now;
	acstime::Duration gap;
	int out;
	CString nomeFile;
	CString backend="";
	CString writer="";
	CString path="~";
	CString observer="dummy";
		
	while ((out=getopt(argc,argv,"hb:w:f:p:o:"))!=-1) {
		switch (out) {
			case 'b' : {
				backend=CString(optarg);
				break;
			}
			case 'f': {
				nomeFile=CString(optarg);
				break;
			}
			case 'p': {
				path=CString(optarg);
				break;
			}
			case 'w': {
				writer=CString(optarg);
				break;
			}
			case 'o': {
				observer=CString(optarg);
				break;
			}
			case 'h': {
				printHelp();
				exit(0);
			}
		}
	}
	if (backend=="") {
		printf("The name of the backend must be given\n\n");
		printHelp();
		exit(-1);
	}
	if (writer=="") {
		writer="MANAGEMENT/FitsZilla";
	}
	try {
		if (client.init(argc,argv)==0) {
			_EXCPT(ClientErrors::CouldntInitExImpl,impl,":main()");
			impl.log();
			exit(-1);
		}
		else {
			if (client.login()==0) {
				_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"::main()");
				impl.log();
				exit(-1);
			}
		}	
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::main()");
		impl.log();
		exit(-1);
	}
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"LOGGED_IN"));	
	try {
		sender=client.getComponent<Backends::GenericBackend>((const char *)backend,0,true);
		if (CORBA::is_nil(sender.in())==true) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"::main()");
			impl.setComponentName((const char *)backend);
			impl.log();
			exit(-1);
		}
	}
	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"::main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		exit(-1);
	}
	catch (maciErrType::CannotGetComponentExImpl& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"::main()");
		impl.setComponentName((const char *)backend);
		impl.log();
		exit(-1);
	}	
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::main()");
		impl.log();
		exit(-1);
	}
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"GOT_COMPONENENT: %s",(const char *)backend));
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_DEBUG,"Reference is: %d",sender.ptr()));
	
	try {
		recv=client.getComponent<Management::DataReceiver>((const char *)writer,0,true);
		if (CORBA::is_nil(recv.in())) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"::main()");
			impl.setComponentName((const char *)writer);
			impl.log();
			exit(-1);
		}
	}
	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"::main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		exit(-1);
	}
	catch (maciErrType::CannotGetComponentExImpl& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"::main()");
		impl.setComponentName((const char *)writer);
		impl.log();
		exit(-1);
	}	
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::main()");
		impl.log();
		exit(-1);
	}	
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"GOT_COMPONENENT: %s",(const char *)writer));
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"Reference is: %d",recv.ptr()));
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"ALL_COMPONENTS_RETRIEVED"));

	ACE_OS::sleep(1);	
	done=false;
	//signal(SIGINT,handler);
	printf("type quit to terminate\n");
	printf("type start to begin data transfer\n");
	printf("type stop to close data transfer\n");
	gap.value=GAP*10000000;
	/*tv.set(RUNNING_SEC,0);*/
	try {
		sender->connect(recv.in());
		sender->sendHeader();
	}
	catch (ACSBulkDataError::AVConnectErrorEx& ex) {
		ACSBulkDataError::AVConnectErrorExImpl impl(ex);
		impl.log();
	}
	catch (...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::main()");
		impl.log();
	}
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"WAITING_FOR_BACKENDS_TO_SETTLE"));
	IRA::CIRATools::Wait(GAP,0);
	while (!done) {
		printf ("> ");
		scanf("%s",input);
		if (strcmp(input,"start")==0) {
			IRA::CIRATools::getTime(now);
			now+=gap; // start gap seconds from now
			fileName.Format("%s/%s_%02d_%02d_%02d.fits",(const char *)path,(const char *)nomeFile,now.hour(),now.minute(),now.second());
			ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"NEW_FILE %s",(const char *)fileName));
			tv.set(MIN_DURATION,0);
			try {	
				recv->setFileName((const char*)fileName);
				recv->setProjectName("PrototypeProject");
				recv->setObserverName((const char *)observer);
				sender->sendData(now.value().value);
			}
			catch (...) {
				_EXCPT(ClientErrors::UnknownExImpl,impl,"::main()");
				impl.log();
			}
			ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"RUNNING FOR AT LEAST %d SECS",MIN_DURATION));
			client.run(tv);
		}
		else if (strcmp(input,"stop")==0) {
			try {
				sender->sendStop();
			}
			catch (...) {
				_EXCPT(ClientErrors::UnknownExImpl,impl,"::main()");
				impl.log();
			}
			ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"STOP"));
		}
		else if (strcmp(input,"quit")==0) {
			done=true;
		}
		else {
			ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"UNKNOW_COMMAND"));
		}
	}
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"TERMINATING"));
	ACE_OS::sleep(1);	
	try {
		sender->terminate();
		sender->disconnect();
		recv->closeReceiver();
	}
	catch (...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::main()");
		impl.log();
	}
	ACE_OS::sleep(1);	
	try {
		client.releaseComponent((const char *)backend);
		client.releaseComponent((const char *)writer);
	}
	catch (maciErrType::CannotReleaseComponentExImpl& E) {
		E.log();
	}
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"COMPONENTS_RELEASED"));
	CIRATools::Wait(1,0);
	client.logout();
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"LOGGED_OUT"));
	CIRATools::Wait(1,0);
	signal(SIGINT,SIG_DFL);
	return 0;
}
