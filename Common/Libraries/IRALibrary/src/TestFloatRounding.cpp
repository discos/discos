#include "IRA"

using namespace IRA;


int main(int argc, char *argv[])
{
	double a=10.232359;
	double b=30.777799;
	long digits=2;
	printf("value: %lf\n",a);
	printf("\tnearest (digits %ld): %lf\n",digits,CIRATools::roundNearest(a,digits));
	printf("\tup (digits %ld): %lf\n",digits,CIRATools::roundUp(a,digits));
	printf("\tdown (digits %ld): %lf\n",digits,CIRATools::roundDown(a,digits));
	printf("value: %lf\n",b);
	printf("\tnearest (digits %ld): %lf\n",digits,CIRATools::roundNearest(b,digits));
	printf("\tup (digits %ld): %lf\n",digits,CIRATools::roundUp(b,digits));
	printf("\tdown (digits %ld): %lf\n",digits,CIRATools::roundDown(b,digits));
	digits=5;
	printf("value: %lf\n",a);
	printf("\tnearest (digits %ld): %lf\n",digits,CIRATools::roundNearest(a,digits));
	printf("\tup (digits %ld): %lf\n",digits,CIRATools::roundUp(a,digits));
	printf("\tdown (digits %ld): %lf\n",digits,CIRATools::roundDown(a,digits));
	printf("value: %lf\n",b);
	printf("\tnearest (digits %ld): %lf\n",digits,CIRATools::roundNearest(b,digits));
	printf("\tup (digits %ld): %lf\n",digits,CIRATools::roundUp(b,digits));
	printf("\tdown (digits %ld): %lf\n",digits,CIRATools::roundDown(b,digits));
	digits=1;
	printf("value: %lf\n",a);
	printf("\tnearest (digits %ld): %lf\n",digits,CIRATools::roundNearest(a,digits));
	printf("\tup (digits %ld): %lf\n",digits,CIRATools::roundUp(a,digits));
	printf("\tdown (digits %ld): %lf\n",digits,CIRATools::roundDown(a,digits));
	printf("value: %lf\n",b);
	printf("\tnearest (digits %ld): %lf\n",digits,CIRATools::roundNearest(b,digits));
	printf("\tup (digits %ld): %lf\n",digits,CIRATools::roundUp(b,digits));
	printf("\tdown (digits %ld): %lf\n",digits,CIRATools::roundDown(b,digits));
	digits=0;
	printf("value: %lf\n",a);
	printf("\tnearest (digits %ld): %lf\n",digits,CIRATools::roundNearest(a,digits));
	printf("\tup (digits %ld): %lf\n",digits,CIRATools::roundUp(a,digits));
	printf("\tdown (digits %ld): %lf\n",digits,CIRATools::roundDown(a,digits));
	printf("value: %lf\n",b);
	printf("\tnearest (digits %ld): %lf\n",digits,CIRATools::roundNearest(b,digits));
	printf("\tup (digits %ld): %lf\n",digits,CIRATools::roundUp(b,digits));
	printf("\tdown (digits %ld): %lf\n",digits,CIRATools::roundDown(b,digits));
}
