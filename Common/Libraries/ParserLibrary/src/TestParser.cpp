#include <IRA>
#include <ComponentErrors.h>
#include "SP_parser.h"

using namespace SimpleParser;

enum TBools {
	VERO,
	FALSO
};

namespace SimpleParser {
class BoolsString {
public:
	static char *valToStr(TBools& val) {
		char *c=new char[16];
		if (val==VERO) {
			strcpy(c,"VERO");
		}
		else {
			strcpy(c,"FALSO");
		}
		return c;
	}
	static TBools strToVal(const char* str) {
		IRA::CString strVal(str);
		strVal.MakeUpper();
		if (strVal=="VERO") {
			return VERO;
		}
		else {
			return FALSO;
		}
	}
};

};

class CTest {
public:	
void positivo(const long& var,TBools& res) {
	if (var>0) res=VERO;
	else res=FALSO;
}
	
void stampa(const char *str) {
	printf("%s\n",str);
}
int somma(const int&i,const int&j) {
	return i+j;
}
double modulo(const double& r,const double& i) {
	return sqrt(r*r+i*i);
}
void random(int& r) {
	r=rand();
}
void errore(const int& i)
{
	if (i==0) {
		_THROW_EXCPT(ComponentErrors::MemoryAllocationExImpl,"Errore()");
	}
}
ACS::Time ora() const
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	return now.value().value;
}
int doppio(double& o,const double& i)
{
	double b;
	o=i;
	b=rand()/i;
	if (b>o) return 1;
	else return 0;
}
long mult(const long& p1,const long& p2,const long& p3,const long& p4) const
{
	return p1*p2*p3*p4;
}
long sum (const long& p1,const long& p2,const long& p3,const long& p4,const long& p5)
{
	return p1+p2+p3+p4+p5;
}
long sub(const long& p1,const long& p2,const long& p3,const long& p4,const long& p5,const long& p6) const
{
	return -p1-p2-p3-p4-p5-p6;
}
void exp(const long & i,const long& d1,const long& d2,const long& d3,const long& d4,const long& d5,const double& d6) const
{
	long res=d1+d2+d3+d4+d5+(long)d6;
	res*=i;
	printf("%ld\n",res);
	//return res;
}
double sumSeq(const ACS::doubleSeq& seq)
{
	double adder=0;
	for (unsigned long i=0;i<seq.length();i++) {
		adder+=seq[i];
	}
	return adder;
}

void square(double& e)
{
	e=e*e;
}

void sequence(ACS::longSeq& seq,const long& len) const
{
	seq.length(len);
	for (long i=0;i<len;i++) {
		seq[i]=i;
	}
}

void wait(const long& sec) const
{
	IRA::CIRATools::Wait(sec,0);
}

IRA::CString remoteCall(const IRA::CString& command,const IRA::CString& package,const long& par) throw (ParserErrors::ExecutionErrorExImpl,ParserErrors::PackageErrorExImpl)
{
	if (par==0) {
		IRA::CString out;
		out.Format("Eseguito: %s of package %s \n",(const char *)command,(const char *)package);
		return out;
	}
	else {
		ParserErrors::ExecutionErrorExImpl err(__FILE__,__LINE__,"remoteCall()");
		throw err;
	}
}

static void callBack(const void *param,const IRA::CString& name,const bool& result) 
{
	CTest *tst=(CTest *)param;
	IRA::CString msg;
	msg.Format("%s injected done! Result is %d\n",(const char *)name,result);
	tst->stampa((const char *)msg);
}

};

int main(int argc, char *argv[])
{
	CTest test;
	CParser<CTest> parser(&test,8,true);
	char input[128];
	IRA::CString out;
	ACS::stringSeq proc1,proc2,proc3,proc4,proc5,proc6,proc7;
	IRA::CString inStr,outCommand,outCommandLine;
	
	parser.add<1>("positivo",new function2<CTest,non_constant,void_type,I<long_type>,O<enum_type<BoolsString,TBools> > >(&test,&CTest::positivo));
	parser.add<1>("stampa",new function1<CTest,non_constant,void_type,I<string_type> >(&test,&CTest::stampa));
	parser.add<2>("somma",new function2<CTest,non_constant,int_type,I<int_type>,I<int_type> >(&test,&CTest::somma));
	parser.add<2>("modulo",new function2<CTest,non_constant,double_type,I<double_type>,I<double_type> >(&test,&CTest::modulo));
	parser.add<1>("errore",new function1<CTest,non_constant,void_type,I<int_type> >(&test,&CTest::errore));
	parser.add<0>("random",new function1<CTest,non_constant,void_type,O<int_type> >(&test,&CTest::random));
	parser.add<0>("ora",new function0<CTest,constant,time_type >(&test,&CTest::ora));
	parser.add<1>("doppio", new function2<CTest,non_constant,int_type,O<double_type>,I<double_type> >(&test,&CTest::doppio));
	parser.add<1>("square", new function1<CTest,non_constant,void_type,IO<double_type> >(&test,&CTest::square));
	parser.add<4>("mult",new function4<CTest,constant,long_type,I<long_type>,I<long_type>,I<long_type>,I<long_type> > (&test,&CTest::mult));
	parser.add<5>("sum",new function5<CTest,non_constant,long_type,I<long_type>,I<long_type>,I<long_type>,I<long_type>,I<long_type> > (&test,&CTest::sum));	
	parser.add<6>("sub",new function6<CTest,constant,long_type,I<long_type>,I<long_type>,I<long_type>,I<long_type>,I<long_type>,I<long_type> > (&test,&CTest::sub));
	function7<CTest,constant,void_type,I<long_type>, I<long_type>, I<long_type>, I<long_type>, I<long_type>, I<long_type>, I<double_type> > *p;
	p=new function7<CTest,constant,void_type,I<long_type>,I<long_type>,I<long_type>,I<long_type>,I<long_type>,I<long_type>,I<double_type> > (&test,&CTest::exp);
	parser.add<7>("exp",p);
	parser.add<1>("sumSeq",new function1<CTest,non_constant,double_type,I<doubleSeq_type> >(&test,&CTest::sumSeq));
	parser.add<1>("sequence",new function2<CTest,constant,void_type,O<longSeq_type>,I<long_type> >(&test,&CTest::sequence));
	parser.add<1>("wait",new function1<CTest,constant,void_type,I<long_type> >(&test,&CTest::wait));
	parser.add("remoteok","extern",0,&CTest::remoteCall);
	parser.add("remotefail","extern",1,&CTest::remoteCall);
	
	proc1.length(3);
	proc1[0]="wait=8";
	//proc1[1]="stampa=hello";
	proc1[1]="somma=10,5";
	proc1[2]="stampa=ciao everybody";
	parser.add("procHello","procedureFile",proc1);  //add a procedure that takes long to complete
	
	proc2.length(2);
	proc2[0]="stampa=eccezione in arrivo!";
	proc2[1]="errore=0";
	parser.add("procError","procedureFile",proc2);
	
	proc3.length(1);
	proc3[0]="somma=100";   
	parser.add("procSyntax","procedureFile",proc3);  //add a procedure that contains a syntax error
	
	proc4.length(2);
	proc4[0]="stampa=chiamo procedura remota";
	proc4[1]="remotefail";
	parser.add("procRemote","procedureFile",proc4);
	
	//parser.inject("salutoIniziale",proc1);
	
	proc5.length(3);
	proc5[0]="wait=8@";
	proc5[1]="stampa=stampa circa dopo 8 secondi@";
	proc5[2]="stampa=prima stampa....!!!!!";
	parser.add("procAsync","procedureFile",proc5);
	
	proc6.length(3);
	proc6[0]="stampa=nested procedure";
	proc6[1]="procHello";
	proc6[2]="stampa=cosa succede";
	parser.add("nestedProc","procedureFile",proc6);
	
	proc7.length(4);
	proc7[0]="stampa=injected procedure";
	proc7[1]="stampa=second statement";
	proc7[2]="wait=8";
	proc7[3]="stampa=eseguito alle 22 e un quarto@22:15:30";
	
	try {
		parser.inject("injectedProc",proc7,&CTest::callBack);
	}
	catch (ParserErrors::ProcedureErrorExImpl& ex) {
		printf("errore nell'inject\n");
	}
	
	printf(">");
	for(;;) {
		fgets(input,128,stdin);
		inStr=IRA::CString(input);
		if (inStr=="exit\n") break;
		try {
			parser.run(inStr,out);
		}
		catch (...) {
			//in a command interpreter component the error should  be logged (DEBUG) and thrown to the upper level
		}
		printf("%s\n",(const char *)out);
		printf(">");
	}
	return 0;
}
