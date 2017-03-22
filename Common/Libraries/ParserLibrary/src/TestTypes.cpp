#include "SP_types.h"

#include <ComponentErrors.h>
#include <IRATools.h>


using namespace SimpleParser; 

typedef enum {
	PIPPO,
	PLUTO
} Disney;

class Disney_converter
{
public:
	Disney strToVal(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		if (strcmp(str,"PIPPO")==0) {
			return PIPPO;
		}
		else if (strcmp(str,"PLUTO")==0) {
			return PLUTO;
		}
		else {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"Disney_converter::strToVal()");
			ex.setExpectedType("enumeration");
			throw ex;
		}
	}
	char *valToStr(const Disney& val) {
		char tmp[128];
		if (val==PLUTO) {
			strcpy(tmp,"PLUTO");
		}
		else {
			strcpy(tmp,"PIPPO");
		}
		char *c=new char [strlen(tmp)+1];
		strcpy(c,tmp);
		return c;
	}
};

_SP_WILDCARD_CLASS(Disney_wildcard,"PIPPO");

int main(int argc, char *argv[]) {

	int_type p;
	int_type k;
	p=10;
	k="20";
	int_type j(k);
	int_type a;
	a="*";
	int_type b(15);
	printf("valore di p: %s, %d\n",(const char *)p,(int)p);
	printf("valore di k: %s, %d\n",(const char *)k,(int)k);
	printf("valore di j: %s, %d\n",(const char *)j,(int)j);
	printf("valore di a: %s, %d\n",(const char *)a,(int)a);
	printf("valore di b: %s, %d\n",(const char *)b,(int)b);

	string_type sp;
	sp="ciao";
	string_type sk(sp);
	string_type sj("saluti");
	printf("valore di sp: %s\n",(const char *)sp);
	printf("valore di sk: %s\n",(const char *)sk);
	printf("valore di sj: %s\n",(const char *)sj);

	long_type lt(13232232);
	printf("valore di lt: %ld\n",(long)lt);

	double_type dt("23.4343222");
	printf("valore di dt: %lf\n",(double)lt);

	enum_type<Disney_converter,Disney,Disney_wildcard> gino;
	gino=PIPPO;
	enum_type<Disney_converter,Disney> pino("PIPPO");
	enum_type<Disney_converter,Disney> rino(PLUTO);
	printf("valore di gino: %s, %d\n",(const char *)gino,(int)gino);
	printf("valore di pino: %s, %d\n",(const char *)pino,(int)pino);
	printf("valore di rino: %s, %d\n",(const char *)rino,(int)rino);
	gino="PLUTO";
	printf("valore di gino: %s, %d\n",(const char *)gino,(Disney)gino);
	gino="*";
	printf("valore di gino: %s, %d\n",(const char *)gino,(Disney)gino);



	angle_type<rad> angle1;
	angle1="359.9d";
	angle_type<rad> angle2("180:10:10.0");
	angle_type<rad> angle3;
	angle3=3.12;
	angle_type<rad> angle4;
	angle4="*";
	printf("valore di angle1: %s, %lf\n",(const char *)angle1,(double)angle1);
	printf("valore di angle2: %s, %lf\n",(const char *)angle2,(double)angle2);
	printf("valore di angle3: %s, %lf\n",(const char *)angle3,(double)angle3);
	printf("valore di angle4: %s, %lf\n",(const char *)angle4,(double)angle4);

	declination_type<rad,true> dec;
	dec="72:15:23.56";
	printf("valore di dec: %s, %lf\n",(const char *)dec,(double)dec);

	rightAscension_type<rad,true> ra;
	ra="12:15:13.56h";
	printf("valore di ra: %s, %lf\n",(const char *)ra,(double)ra);

	azimuth_type<deg,true> az;
	az=166.232;
	printf("valore di az: %s, %lf\n",(const char *)az,(double)az);

	elevation_type<deg,true> el;
	el=0.15;
	printf("valore di el: %s, %lf\n",(const char *)el,(double)el);

	galacticLatitude_type<rad,true> glat;
	glat="44.897d";
	printf("valore di glat: %s, %lf\n",(const char *)glat,(double)glat);

	galacticLongitude_type<rad,true> glon;
	glon=3.14;
	printf("valore di glon: %s, %lf\n",(const char *)glon,(double)glon);

	time_type tt;
	tt="2014-110-14:20:23.56";
	printf("valore di tt: %s, %lld\n",(const char *)tt,(ACS::Time)tt);

	time_type tt1;
	tt1=12233434334522300ll;
	printf("valore di tt1: %s, %lld\n",(const char *)tt1,(ACS::Time)tt1);

	interval_type itt(864010000000ll);
	printf("valore di itt: %s, %lld\n",(const char *)itt,(ACS::Time)itt);

	longSeq_type lseq;
	lseq="10;12;14;13;22";
    printf("valore di lseq: %s\n",(const char *)lseq);
    ACS::longSeq testSeq;
    testSeq=lseq;
    printf("lunghezza di lseq: %d\n",testSeq.length());
    for (unsigned jj=0;jj<testSeq.length();jj++) printf("%d\n",testSeq[jj]);

    doubleSeq_type dseq;
    dseq="10.1212;12.0;14.12322;13.99;22.3433";
     printf("valore di dseq: %s\n",(const char *)dseq);
     ACS::doubleSeq testSeq1;
     testSeq1=dseq;
     printf("lunghezza di dseq: %d\n",testSeq1.length());
     for (unsigned jj=0;jj<testSeq1.length();jj++) printf("%lf\n",testSeq1[jj]);


	return 0;
}

