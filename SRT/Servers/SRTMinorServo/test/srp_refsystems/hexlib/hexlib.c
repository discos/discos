/*
  INAF - Osservatorio Astronomico di Cagliari
  data: 20 settembre 2011
  rel: 0.8
  autore: Franco Buffa
  e-mail: fbuffa@oa-cagliari.inaf.it
  
  ATTENZIONE BISOGNA INSTALLARE LE LIB GSL 
  http://www.gnu.org/software/gsl/
  
  per compilare il codice:
  gcc foobar.c hexlib.c -lgsl -lgslcblas -lm -o foobar
  
  bugs & info: 

   20 09 2011 - Piccole manutenzioni sui test0x.c
   e introdotte le 2 rot. RZRYRX e RXRYRZ 

   10 08 2010 - introdotte nuove coord. attuatori nel file hexdata,
   la funzione init_p non è più da utilizzarsi 

   26 10 2005 - create nuove func. per implementare il goto

   19 10 2005 - corr. bug che trasformava erroneamente gli angoli in rad

   12 10 2005 - corretti bugs individuati da F. Fiocchi (thanks!)

   21 12 2004 - corretto bug nella func "dir"
  
   22 Giugno 2004 - create le func "inv" e "print_p"
   + altre modifiche minori
   
   10 Maggio 2004 - corretto un baco in test2 che
   impediva la compilazione con vecchie versioni 
   di gcc + alcune modifiche minori
*/

#include "hexlib.h"

static int rotation = RZRYRX;

void rot(double *w,double *x, double *y)
{
 int i,j;
 double r[3],x0[3],sum,cp,sp,ct,st,cf,sf,R[3][3];
 for (i=0;i<3;i++)
 {
  x0[i]=x[i];
  r[i]=w[i+3];
 } 
 cp=cos(r[0]);
 sp=sin(r[0]);
 ct=cos(r[1]);
 st=sin(r[1]);
 cf=cos(r[2]);
 sf=sin(r[2]);

 if(rotation==RXRYRZ)
 {
// matrice di rotazione RX(RY(RZ(P)))
  R[0][0]=ct*cf;
  R[0][1]=-ct*sf;
  R[0][2]=st;
  R[1][0]=cp*sf+sp*st*cf;
  R[1][1]=cp*cf-sp*st*sf;
  R[1][2]=-sp*ct;
  R[2][0]=sp*sf-cp*st*cf;
  R[2][1]=sp*cf+cp*st*sf;
  R[2][2]=cp*ct;
 }
 else
 {
// ** DEFAULT **
// matrice di rotazione RZ(RY(RX(P)))
  R[0][0]=ct*cf;
  R[0][1]=sp*st*cf-cp*sf;
  R[0][2]=cp*st*cf+sp*sf;
  R[1][0]=ct*sf;
  R[1][1]=sp*st*sf+cp*cf;
  R[1][2]=cp*st*sf-sp*cf;
  R[2][0]=-st;
  R[2][1]=sp*ct;
  R[2][2]=cp*ct;
 }

 for(i=0;i<3;i++)
 {
  sum=0;
  for(j=0;j<3;j++) sum+=R[i][j]*x0[j];
  y[i]=sum+w[i];
 }
}


void print_state (struct rparams *p)
{
  int i;
  printf ("status = %s(%d)\n", gsl_strerror (p->status),p->status);
  printf ("iter = %04u\n",p->iter);
  printf ("x = ");
  for(i=0;i<_n;i++) printf ("%12.8f ", p->x[i]);
  printf ("\nf =");
  for(i=0;i<_n;i++) printf ("%12.8f ", p->y[i]);
  printf ("\n");
}

void dir(struct rparams *p)
{
  int i;
  double xx[_n];
  double aa1[3],aa2[3],aa3[3],aa4[3],aa5[3],aa6[3];
  double pp1[3],pp2[3],pp3[3],pp4[3],pp5[3],pp6[3];
  double xx1[3],xx2[3],xx3[3],xx4[3],xx5[3],xx6[3];
        
  for(i=0;i<_n;i++) xx[i]=p->x[i];
  
  pp1[0]=p->p1[0];pp1[1]=p->p1[1];pp1[2]=p->p1[2];
  pp2[0]=p->p2[0];pp2[1]=p->p2[1];pp2[2]=p->p2[2];
  pp3[0]=p->p3[0];pp3[1]=p->p3[1];pp3[2]=p->p3[2];
  pp4[0]=p->p4[0];pp4[1]=p->p4[1];pp4[2]=p->p4[2];
  pp5[0]=p->p5[0];pp5[1]=p->p5[1];pp5[2]=p->p5[2];
  pp6[0]=p->p6[0];pp6[1]=p->p6[1];pp6[2]=p->p6[2];
   
  aa1[0]=p->a1[0];aa1[1]=p->a1[1];aa1[2]=p->a1[2];
  aa2[0]=p->a2[0];aa2[1]=p->a2[1];aa2[2]=p->a2[2];
  aa3[0]=p->a3[0];aa3[1]=p->a3[1];aa3[2]=p->a3[2];
  aa4[0]=p->a4[0];aa4[1]=p->a4[1];aa4[2]=p->a4[2];
  aa5[0]=p->a5[0];aa5[1]=p->a5[1];aa5[2]=p->a5[2];
  aa6[0]=p->a6[0];aa6[1]=p->a6[1];aa6[2]=p->a6[2];
  
  rot(xx,pp1,xx1);
  rot(xx,pp2,xx2);
  rot(xx,pp3,xx3);
  rot(xx,pp4,xx4);
  rot(xx,pp5,xx5);
  rot(xx,pp6,xx6);
    
  p->d[0]=sqrt(pow(aa1[0]-xx1[0],2.)+pow(aa1[1]-xx1[1],2.)+pow(aa1[2]-xx1[2],2.));
  p->d[1]=sqrt(pow(aa2[0]-xx2[0],2.)+pow(aa2[1]-xx2[1],2.)+pow(aa2[2]-xx2[2],2.));
  p->d[2]=sqrt(pow(aa3[0]-xx3[0],2.)+pow(aa3[1]-xx3[1],2.)+pow(aa3[2]-xx3[2],2.));
  p->d[3]=sqrt(pow(aa4[0]-xx4[0],2.)+pow(aa4[1]-xx4[1],2.)+pow(aa4[2]-xx4[2],2.));
  p->d[4]=sqrt(pow(aa5[0]-xx5[0],2.)+pow(aa5[1]-xx5[1],2.)+pow(aa5[2]-xx5[2],2.));
  p->d[5]=sqrt(pow(aa6[0]-xx6[0],2.)+pow(aa6[1]-xx6[1],2.)+pow(aa6[2]-xx6[2],2.));
  
}
 
int func_f (const gsl_vector * x, void *params, gsl_vector * f)
{
  int i;
  double aa1[3],aa2[3],aa3[3],aa4[3],aa5[3],aa6[3];
  double pp1[3],pp2[3],pp3[3],pp4[3],pp5[3],pp6[3];
  double xx1[3],xx2[3],xx3[3],xx4[3],xx5[3],xx6[3];
  double xx[_n],y[_n],dd[_n];
  
  struct rparams *p=(struct rparams *) params;
  
  for (i=0;i<_n;i++) xx[i] = gsl_vector_get (x, i);
  for (i=0;i<_n;i++) dd[i]=p->d[i];

  pp1[0]=p->p1[0];pp1[1]=p->p1[1];pp1[2]=p->p1[2];
  pp2[0]=p->p2[0];pp2[1]=p->p2[1];pp2[2]=p->p2[2];
  pp3[0]=p->p3[0];pp3[1]=p->p3[1];pp3[2]=p->p3[2];
  pp4[0]=p->p4[0];pp4[1]=p->p4[1];pp4[2]=p->p4[2];
  pp5[0]=p->p5[0];pp5[1]=p->p5[1];pp5[2]=p->p5[2];
  pp6[0]=p->p6[0];pp6[1]=p->p6[1];pp6[2]=p->p6[2];

  aa1[0]=p->a1[0];aa1[1]=p->a1[1];aa1[2]=p->a1[2];
  aa2[0]=p->a2[0];aa2[1]=p->a2[1];aa2[2]=p->a2[2];
  aa3[0]=p->a3[0];aa3[1]=p->a3[1];aa3[2]=p->a3[2];
  aa4[0]=p->a4[0];aa4[1]=p->a4[1];aa4[2]=p->a4[2];
  aa5[0]=p->a5[0];aa5[1]=p->a5[1];aa5[2]=p->a5[2];
  aa6[0]=p->a6[0];aa6[1]=p->a6[1];aa6[2]=p->a6[2];
   
  rot(xx,pp1,xx1);
  rot(xx,pp2,xx2);
  rot(xx,pp3,xx3);
  rot(xx,pp4,xx4);
  rot(xx,pp5,xx5);
  rot(xx,pp6,xx6);
        
  y[0]=pow(dd[0],2.)-(pow(aa1[0]-xx1[0],2.)+pow(aa1[1]-xx1[1],2.)+pow(aa1[2]-xx1[2],2.));
  y[1]=pow(dd[1],2.)-(pow(aa2[0]-xx2[0],2.)+pow(aa2[1]-xx2[1],2.)+pow(aa2[2]-xx2[2],2.));
  y[2]=pow(dd[2],2.)-(pow(aa3[0]-xx3[0],2.)+pow(aa3[1]-xx3[1],2.)+pow(aa3[2]-xx3[2],2.));
  y[3]=pow(dd[3],2.)-(pow(aa4[0]-xx4[0],2.)+pow(aa4[1]-xx4[1],2.)+pow(aa4[2]-xx4[2],2.));
  y[4]=pow(dd[4],2.)-(pow(aa5[0]-xx5[0],2.)+pow(aa5[1]-xx5[1],2.)+pow(aa5[2]-xx5[2],2.));
  y[5]=pow(dd[5],2.)-(pow(aa6[0]-xx6[0],2.)+pow(aa6[1]-xx6[1],2.)+pow(aa6[2]-xx6[2],2.));

  for (i=0;i<_n;i++) gsl_vector_set (f, i, y[i]);

  return GSL_SUCCESS;
}

int init_p(struct rparams *p)
{
   double l,h;
   double da0,da1[3],da2[3],da3[3],da4[3],da5[3],da6[3];
   double dp1[3],dp2[3],dp3[3],dp4[3],dp5[3],dp6[3];
   double ver[3],t1[3],t2[3],t3[3],t4[3];
   int i;
   
   l=_HX_L;                // lato del triangolo
   h=l/2.*sqrt(3.);        // altezza del triangolo
   da0=_HX_ATT;            // lung. attuatori
   
// attuatori in segno e coord.   
   da1[0]=0;    da1[1]=0;   da1[2]=-da0;
   da2[0]=0;    da2[1]=0;   da2[2]=-da0;
   da3[0]=0;    da3[1]=0;   da3[2]=-da0;
   da4[0]=-da0; da4[1]=0;   da4[2]=0;
   da5[0]=0;    da5[1]=da0; da5[2]=0;
   da6[0]=0;    da6[1]=da0; da6[2]=0;

// barre giunti card. in segno e coord.
   dp1[0]=0;        dp1[1]=0;        dp1[2]=_HX_BARZ;
   dp2[0]=0;        dp2[1]=0;        dp2[2]=_HX_BARZ;
   dp3[0]=0;        dp3[1]=0;        dp3[2]=_HX_BARZ;
   dp4[0]=_HX_BARX; dp4[1]=0;        dp4[2]=0;
   dp5[0]=0;        dp5[1]=_HX_BARY; dp5[2]=0;
   dp6[0]=0;        dp6[1]=_HX_BARY; dp6[2]=0;

   ver[0]=0; ver[1]=0; ver[2]=_HX_OR;  // centro triang. rispetto alla vera orig.

// vertici triangolo + p.to ancoraggio in x
   t1[0]=-l/2.+ver[0];             t1[1]=h/3.+ver[1];     t1[2]=ver[2];
   t2[0]= l/2.+ver[0];             t2[1]=h/3.+ver[1];     t2[2]=ver[2];
   t3[0]= ver[0];                  t3[1]=-2./3.*h+ver[1]; t3[2]=ver[2];
   t4[0]=-2./3.*h/sqrt(3.)+ver[0]; t4[1]=ver[1];          t4[2]=ver[2];

// coord punti di attacco sul triangolo (zzzxyy)
   for(i=0;i<3;i++) p->p1[i]=t1[i]+dp1[i];
   for(i=0;i<3;i++) p->p2[i]=t2[i]+dp2[i];
   for(i=0;i<3;i++) p->p3[i]=t3[i]+dp3[i];
   for(i=0;i<3;i++) p->p4[i]=t4[i]+dp4[i];
   for(i=0;i<3;i++) p->p5[i]=t1[i]+dp5[i];
   for(i=0;i<3;i++) p->p6[i]=t2[i]+dp6[i];

// coord punti di attacco sulla struttura (zzzxyy)
   for(i=0;i<3;i++) p->a1[i]=p->p1[i]+da1[i];
   for(i=0;i<3;i++) p->a2[i]=p->p2[i]+da2[i];
   for(i=0;i<3;i++) p->a3[i]=p->p3[i]+da3[i];
   for(i=0;i<3;i++) p->a4[i]=p->p4[i]+da4[i];
   for(i=0;i<3;i++) p->a5[i]=p->p5[i]+da5[i];
   for(i=0;i<3;i++) p->a6[i]=p->p6[i]+da6[i];
 
   p->status=-999;
   p->iter=-999;
   for (i=0;i<_n;i++) p->x[i]=0.0;
   for (i=0;i<_n;i++) p->y[i]=0.0;
   for (i=0;i<_n;i++) p->d[i]=0.0;

   return GSL_SUCCESS;
}

void inv(struct rparams *p)  
{
  gsl_multiroot_fsolver *s;
  const gsl_multiroot_fsolver_type *T;  
  int status;
  size_t i, iter = 0;
  const size_t n = _n;
  gsl_multiroot_function f = {&func_f, n, (void *)p};
  gsl_vector *x = gsl_vector_alloc (n);
    
  for(i=0;i<n;i++) gsl_vector_set (x, i, 0.0);
  T = gsl_multiroot_fsolver_hybrids;
  s = gsl_multiroot_fsolver_alloc (T, n);
  gsl_multiroot_fsolver_set (s, &f, x);
  do
  {
    iter++;
    status = gsl_multiroot_fsolver_iterate (s);
    if (status) break;
    status = gsl_multiroot_test_residual (s->f, _eps);
  } while (status == GSL_CONTINUE && iter < _niter);

  p->status=status;
  p->iter=iter;
  for(i=0;i<n;i++) p->x[i]=gsl_vector_get (s->x,i);
  for(i=0;i<n;i++) p->y[i]=gsl_vector_get (s->f,i);
  
  gsl_multiroot_fsolver_free (s); 
  gsl_vector_free (x);
}

int print_p(struct rparams *p)
{   
   printf("%8.3f\t%8.3f\t%8.3f\n",p->p1[0],p->p1[1],p->p1[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->p2[0],p->p2[1],p->p2[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->p3[0],p->p3[1],p->p3[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->p4[0],p->p4[1],p->p4[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->p5[0],p->p5[1],p->p5[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->p6[0],p->p6[1],p->p6[2]);

   printf("%8.3f\t%8.3f\t%8.3f\n",p->a1[0],p->a1[1],p->a1[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->a2[0],p->a2[1],p->a2[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->a3[0],p->a3[1],p->a3[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->a4[0],p->a4[1],p->a4[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->a5[0],p->a5[1],p->a5[2]);
   printf("%8.3f\t%8.3f\t%8.3f\n",p->a6[0],p->a6[1],p->a6[2]);
   
   return GSL_SUCCESS;
}

int load_p(struct rparams *p,char *fname)
{   
   int i;
   float d1,d2,d3;
   FILE *ptr;
   if((ptr=fopen(fname,"r"))==NULL) return(1);
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->p1[0]=(double)d1,p->p1[1]=(double)d2,p->p1[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->p2[0]=(double)d1,p->p2[1]=(double)d2,p->p2[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->p3[0]=(double)d1,p->p3[1]=(double)d2,p->p3[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->p4[0]=(double)d1,p->p4[1]=(double)d2,p->p4[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->p5[0]=(double)d1,p->p5[1]=(double)d2,p->p5[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->p6[0]=(double)d1,p->p6[1]=(double)d2,p->p6[2]=(double)d3;

// --------   
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->a1[0]=(double)d1,p->a1[1]=(double)d2,p->a1[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->a2[0]=(double)d1,p->a2[1]=(double)d2,p->a2[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->a3[0]=(double)d1,p->a3[1]=(double)d2,p->a3[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->a4[0]=(double)d1,p->a4[1]=(double)d2,p->a4[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->a5[0]=(double)d1,p->a5[1]=(double)d2,p->a5[2]=(double)d3;
   
   fscanf(ptr,"%f%f%f",&d1,&d2,&d3);
   p->a6[0]=(double)d1,p->a6[1]=(double)d2,p->a6[2]=(double)d3;

   fclose(ptr);

   p->status=-999;
   p->iter=-999;
   for (i=0;i<_n;i++) p->x[i]=0.0;
   for (i=0;i<_n;i++) p->y[i]=0.0;
   for (i=0;i<_n;i++) p->d[i]=0.0;

   return (0);
}

int gotopos(struct rparams *p,double *pos1, double *pos2, int nstp, h_array *x)
{
   int i,j;
   double d1[_n],d2[_n],deltad[_n];
   if(nstp<2) return(-1);

   for(i=0;i<_n;i++) p->x[i]=pos1[i];
   dir(p);
   for(i=0;i<_n;i++) d1[i]=p->d[i];

   for(i=0;i<_n;i++) p->x[i]=pos2[i];
   dir(p);
   for(i=0;i<_n;i++) d2[i]=p->d[i];

   for(i=0;i<_n;i++) deltad[i]=(d2[i]-d1[i])/(double)(nstp-1);

   for(i=0;i<nstp;i++)
   {
      for(j=0;j<_n;j++) p->d[j]=d1[j]+deltad[j]*(double)i;
      inv(p);
      for(j=0;j<6;j++)  x[i*13+j]=p->x[j];
      for(j=6;j<12;j++) x[i*13+j]=p->d[j-6];
      x[i*13+12]=(double)p->status;
   }
   return (0);
}
 
double *new_array(int m)
{
  h_array *x;
  x=(double *)malloc(sizeof(double[13*m]));
  return(x);
}

int get_array(int i, h_array *x, double *y)
{
  int j,status;
  for(j=0;j<12;j++) y[j]=x[i*13+j];
  status=(int)x[i*13+12];
  return(status);
}

void free_array(h_array *x)
{
  free(x);
}

int set_rot(int rot_s)
{   
   if(rot_s==RZRYRX)
   {
     rotation=RZRYRX;
   }
   else
   {
     rotation=RXRYRZ;
   }
   return GSL_SUCCESS;
}
