#include "SP_types.h"
#include "SP_function1.h"
#include "SP_function0.h"
#include "SP_function3.h"
#include <ComponentErrors.h>
#include <IRATools.h>

using namespace SimpleParser; 

class CProva {
public:
int stampa(const int& i) {
	printf("'Stampa': %d\n",i);
	return i*i;
}
void stampaStringa(const char *a) {
	printf("'Stampa Stringa':  %s\n",a);
}
void change(char *&a) {
	if (a!=NULL) delete []a;
	a=new char [128];
	strcpy(a,"changed now !!!!!");
}
int changeDouble(double &d) {
	int i=(int) d;
	d/=2.0;
	return i;
}
void test3Unary(const int& i,const int& j,const int& k) {
	int y=i+j+k;
	printf("'Test3Unary': %d\n",y);
}
void Error() throw (ComponentErrors::MemoryAllocationExImpl) {
	_THROW_EXCPT(ComponentErrors::MemoryAllocationExImpl,"CProva::Error()");
}
void testRadHourAngle(double& ra) {
	printf("'testRadHourAngle': %lf\n",ra);
	ra=DPI;
}
void testDegAngle(double& ang) {
	printf("'testDegAngle': %lf\n",ang);
	ang=265.2689;
}
void testDegDeclination(double& dec) {
	printf("'testDegDeclination': %lf\n",dec);
	dec=44.456;
}
};

int main(int argc, char *argv[]) {
	CProva cl;
	WORD pos;
	IRA::CString params[3] = { "4", "ciao!!", "12.44" };
	IRA::CString outParams[3];
	int_type intRet;
	int_type intVal("4");
	string_type strVal("ciao!!");
	double_type dblVal("12.44");

	//
	function1<CProva,non_constant,int_type,I<int_type> > funA(&cl,&CProva::stampa);
	function1<CProva,non_constant,void_type,I<string_type> > funB(&cl,&CProva::stampaStringa);
	function1<CProva,non_constant,void_type,O<string_type> > funC(&cl,&CProva::change);
	function1<CProva,non_constant,int_type,O<double_type> > funD(&cl,&CProva::changeDouble);
	function0<CProva,non_constant,void_type > funE(&cl,&CProva::Error);
	function1<CProva,non_constant,void_type,O<rightAscension_type<rad,true> > > funF(&cl,&CProva::testRadHourAngle);
	function1<CProva,non_constant,void_type,O<angle_type<deg> > > funG(&cl,&CProva::testDegAngle);
	function1<CProva,non_constant,void_type,O<declination_type<deg,true> > > funH(&cl,&CProva::testDegDeclination);
	function3<CProva,non_constant,void_type,I<int_type>,I<int_type>,I<int_type> > funI(&cl,&CProva::test3Unary);
	
	/*base *p1,*p2;
	struct prova<int>data;
	data.number=1;
	data.type=&intVal;
	
	p1=&funA;
	p2=&funE;
	
	functor<1,CProva,int_type > *f1;
	
	f1 = dynamic_cast<functor<1,CProva,int_type > *>(p1);
	f1->call(&params[0]);
	
	//( (functor<1,CProva,R<int_type> >*)  p1)->call(&params[0]);*/
	
	
	// Test the vector of strings  method
	printf("Testing string vector  method......\n");
	funA.call(&params[0],1);
	pos=funA.get(outParams); 
	printf("Return from 'Stampa': %s\n",(const char *)outParams[pos]);
	funB.call(&params[1],1);
	funC.call(&params[1],1);
	funC.get(&params[1]);
	printf("Parameter from 'Change': %s\n",(const char *)params[1]);
	funD.call(&params[2],1);
	pos=funD.get(outParams);
	printf("Parameter from 'ChangeDouble': %s\n",(const char *)outParams[0]);
	printf("Return from 'ChangeDouble': %s\n",(const char *)outParams[pos]);
	params[0]="10"; params[1]="12"; params[2]="22";
	funI.call(params,3);

	//Test the single parameters method
	printf("\nTesting parameters method......\n");
	intRet=funA(intVal);
	printf("Return from 'Stampa': %d\n",(int)intRet);
	funB(strVal);
	funC(strVal);
	printf("Parameter from 'Change': %s\n",(const char *)strVal);
	intRet=funD(dblVal);
	printf("Parameter from 'ChangeDouble': %lf\n",(double)dblVal);
	printf("Return from 'ChangeDouble': %d\n",(int)intRet);
	
	// Test special Type
	printf("\nTesting special types......\n");
	printf("\nTesting right ascension......\n");
	rightAscension_type<rad,true> ra("14:24:21.5");
	printf("Previuos value is: %s\n",(const char *)ra);
	funF(ra);
	printf("current value is: %s\n",(const char *)ra);
	printf("\nTesting angle......\n");
	angle_type<deg> ang("-214.5");
	printf("Previuos value is: %s\n",(const char *)ang);
	funG(ang);
	printf("current value is: %s\n",(const char *)ang);
	printf("\nTesting declination......\n");
	declination_type<deg,true> dec("-32:11:24.5");
	printf("Previuos value is: %s\n",(const char *)dec);
	funH(dec);
	printf("current value is: %s\n",(const char *)dec);
	printf("\nTesting time.....1\n");	
	time_type tm1("201-18:04:32.1");
	printf("time is %s\n",(const char *)tm1);
	printf("100 nanoseconds are %llu\n",(ACS::Time)tm1);
	printf("\nTesting time.....2\n");
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	time_type tm2(now.value().value);
	printf("time is %s\n",(const char *)tm2);
	printf("100 nanoseconds are %llu\n",(ACS::Time)tm2);

	
	// Test error handling
	printf("\nTesting error handling......\n");
	printf("\nbad double.....\n");
	try {
		double_type err_double("zd4l");
		printf("%lf\n",(double)err_double);
	}
	catch (ParserErrors::BadTypeFormatExImpl& ex) {
		IRA::CString out;
		_EXCPT_TO_CSTRING(out,ex);
		printf("%s\n",(const char *)out);
	}
	printf("\nbad right ascension.....\n");
	try {
		rightAscension_type<rad,true> badRa("23:70:23.33");
	}
	catch (ParserErrors::BadTypeFormatExImpl& ex) {
		IRA::CString out;
		_EXCPT_TO_CSTRING(out,ex);
		printf("%s\n",(const char *)out);
	}
	printf("\nbad angle......\n");
	try {
		angle_type<rad> badAng("-366:70:23.33");
	}
	catch (ParserErrors::BadTypeFormatExImpl& ex) {
		IRA::CString out;
		_EXCPT_TO_CSTRING(out,ex);
		printf("%s\n",(const char *)out);
	}
	printf("\nbad declination......\n");
	try {
		declination_type<rad,true> badDec("194:25:10.21");
	}
	catch (ParserErrors::BadTypeFormatExImpl& ex) {
		IRA::CString out;
		_EXCPT_TO_CSTRING(out,ex);
		printf("%s\n",(const char *)out);
	}	
	
	printf("\nbad time......\n");
	try {
		time_type badTime("26:12:14.5");
	}
	catch (ParserErrors::BadTypeFormatExImpl& ex) {
		IRA::CString out;
		_EXCPT_TO_CSTRING(out,ex);
		printf("%s\n",(const char *)out);
	}		
	
	printf("\nbad integer.....\n");	
	try {
		int_type err_int;
		err_int="-f4";
		printf("%d\n",(int)err_int);
	}
	catch (ParserErrors::BadTypeFormatExImpl& ex) {
		IRA::CString out;
		_EXCPT_TO_CSTRING(out,ex);
		printf("%s\n",(const char *)out);
	}
	printf("\nbad integer using string vector method.....\n");
	IRA::CString badIntParam("ghw1");
	try { 
		funA.call(&badIntParam,1);
	}
	catch (ParserErrors::ConversionErrorExImpl& ex) {
		IRA::CString out;
		_EXCPT_TO_CSTRING(out,ex);
		printf("%s\n",(const char *)out);	
	}
	printf("\nerror in calling a function with parameters method.....\n");
	try {
		funE();
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		IRA::CString out;
		_EXCPT_TO_CSTRING(out,ex);
		printf("%s\n",(const char *)out);		
	}
	printf("\nerror in calling a function with string vector.....\n");
	try {
		funE.call(NULL,0);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		IRA::CString out;
		_EXCPT_TO_CSTRING(out,ex);
		printf("%s\n",(const char *)out);		
	}	
	return 0;
}

