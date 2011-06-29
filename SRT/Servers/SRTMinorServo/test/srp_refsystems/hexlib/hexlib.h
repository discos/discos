#ifndef __HEXLIB_H__
#define __HEXLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>
#include <math.h>
#define _n 6
#define _niter 1000
#define _eps 1.0e-7
#define PI M_PI
#define _HX_L 3204.0
#define _HX_ATT 1570.0
#define _HX_BARX -406.0   
#define _HX_BARY  306.0  
#define _HX_BARZ -394.0
#define _HX_OR -1893.6   

typedef double h_array;

struct rparams 
{
   double p1[3],p2[3],p3[3],p4[3],p5[3],p6[3];
   double a1[3],a2[3],a3[3],a4[3],a5[3],a6[3];
   int status;
   size_t iter;
   double x[_n],y[_n],d[_n];
};

// Prototipi
int func_f (const gsl_vector * x, void *params, gsl_vector * f);
void rot(double *w,double *x, double *y);
void print_state (struct rparams *p);
void dir(struct rparams *p);
int init_p(struct rparams *p);
void inv(struct rparams *p);
int print_p(struct rparams *p);
int load_p(struct rparams *p,char *fname);
double *new_array(int m);
void free_array(h_array *x);
int get_array(int i, h_array *x, double *y);
int eval_array(double *pos1, double *pos2, int nstp, h_array *x);


#ifdef __cplusplus
}
#endif

#endif
