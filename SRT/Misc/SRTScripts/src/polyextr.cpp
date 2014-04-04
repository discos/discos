#include <cstdio>
#include <iostream>
#include <fstream>
#include </usr/local/include/gsl/gsl_multifit.h>
#include </usr/local/include/gsl/gsl_poly.h>

#define POLYNOMIALDEGREE 5

using namespace std;

int
main (int argc, char **argv)
{
  int i, n;
  double xi, yi, ei, chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *w, *c;

  if (argc != 2)
    {
      fprintf (stderr, "usage: fit n < data\n");
      exit (-1);
    }

  n = atoi (argv[1]);

  double xx[5];
  int elevations[] = {5,30,45,70,90};
  double error = 0.01;
  ifstream actuatorsCorrections_FEM_Initial ("/home/almamgr/actuatorsCorrections_FEM_Initial.txt");
  fstream actuatorsCorrections_FEM ("/home/almamgr/actuatorsCorrections_FEM.txt");

  int m, s;
  for (m = 1; m <= 1116; m++) {
      actuatorsCorrections_FEM_Initial >> xx[4] >> xx[3] >> xx[2] >> xx[1] >> xx[0];
      for (s = 1; s <= 5; s++) {
           actuatorsCorrections_FEM << elevations[s-1] << " " << xx[s-1] << " " << error << endl;
      }
  }





  int l;

  actuatorsCorrections_FEM.seekg(0, ios::beg);

  for (l = 1; l <= 1116; l++)
    {
      X = gsl_matrix_alloc (n, POLYNOMIALDEGREE);
      y = gsl_vector_alloc (n);
      w = gsl_vector_alloc (n);

      c = gsl_vector_alloc (POLYNOMIALDEGREE);
      cov = gsl_matrix_alloc (POLYNOMIALDEGREE, POLYNOMIALDEGREE);

      for (i = 0; i < n; i++)
	{
	  
        /*int count = fscanf (stdin, "%lg %lg %lg",
			      &xi, &yi, &ei);

	  if (count != 3)
	    {
	      fprintf (stderr, "error reading file\n");
	      exit (-1);
	    }*/

        actuatorsCorrections_FEM >> xi >> yi >> ei;

	  printf ("%g %g +/- %g\n", xi, yi, ei);

	  gsl_matrix_set (X, i, 0, 1.0);
	  gsl_matrix_set (X, i, 1, xi);
	  gsl_matrix_set (X, i, 2, xi * xi);
	  gsl_matrix_set (X, i, 3, xi * xi * xi);
	  gsl_matrix_set (X, i, 4, xi * xi * xi * xi);

	  gsl_vector_set (y, i, yi);
	  gsl_vector_set (w, i, 1.0 / (ei * ei));
	}

      {
	gsl_multifit_linear_workspace *work
	  = gsl_multifit_linear_alloc (n, POLYNOMIALDEGREE);
	gsl_multifit_wlinear (X, w, y, c, cov, &chisq, work);
	gsl_multifit_linear_free (work);
      }

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

      {
	printf ("# best fit: Y = %g + %g X + %g X^2 + %g X^3 + %g X^4\n",
		C (0), C (1), C (2), C (3), C (4));

	int elevation;
	double coefficients[5];

	coefficients[0] = C (0);
	coefficients[1] = C (1);
	coefficients[2] = C (2);
	coefficients[3] = C (3);
	coefficients[4] = C (4);

	for (elevation = 1; elevation <= 90; elevation++)
	  printf ("elevation = %d, position = %f\n", elevation,
		  gsl_poly_eval (coefficients, POLYNOMIALDEGREE, elevation));

	/*printf ("# covariance matrix:\n");
	   printf ("[ %+.5e, %+.5e, %+.5e  \n",
	   COV(0,0), COV(0,1), COV(0,2));
	   printf ("  %+.5e, %+.5e, %+.5e  \n", 
	   COV(1,0), COV(1,1), COV(1,2));
	   printf ("  %+.5e, %+.5e, %+.5e ]\n", 
	   COV(2,0), COV(2,1), COV(2,2));
	   printf ("# chisq = %g\n", chisq); */
     }

      gsl_matrix_free (X);
      gsl_vector_free (y);
      gsl_vector_free (w);
      gsl_vector_free (c);
      gsl_matrix_free (cov);
    }
  return 0;
}
