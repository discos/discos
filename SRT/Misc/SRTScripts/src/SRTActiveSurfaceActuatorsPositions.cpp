#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include </usr/local/include/gsl/gsl_multifit.h>
#include </usr/local/include/gsl/gsl_poly.h>

#define POLYNOMIALDEGREE 5

using namespace std;

int main (int argc, char **argv)
{
    int i, m, s, l;
    double xi, yi, ei, chisq;
    gsl_matrix *X, *cov;
    gsl_vector *y, *w, *c;

    double xx[5];
    int elevations[] = {5,30,45,70,90};
    double error = 0.001;

    ifstream actuatorsCorrections_FEM_Initial ("actuatorsCorrections_FEM_Initial.txt");
    fstream actuatorsCorrections_FEM ("actuatorsCorrections_FEM.txt");
    fstream actuatorsCorrections ("actuatorsCorrections.txt");

    for (m = 1; m <= 1116; m++) {
        actuatorsCorrections_FEM_Initial >> xx[4] >> xx[3] >> xx[2] >> xx[1] >> xx[0];
        for (s = 1; s <= 5; s++) {
            actuatorsCorrections_FEM << elevations[s-1] << " " << xx[s-1] << " " << error << endl;
        }
    }

    actuatorsCorrections_FEM.seekg(0, ios::beg);
    
    for (l = 1; l <= 1116; l++) {
        X = gsl_matrix_alloc (POLYNOMIALDEGREE, POLYNOMIALDEGREE);
        y = gsl_vector_alloc (POLYNOMIALDEGREE);
        w = gsl_vector_alloc (POLYNOMIALDEGREE);
        c = gsl_vector_alloc (POLYNOMIALDEGREE);
        cov = gsl_matrix_alloc (POLYNOMIALDEGREE, POLYNOMIALDEGREE);
        for (i = 0; i < POLYNOMIALDEGREE; i++) {
            actuatorsCorrections_FEM >> xi >> yi >> ei;
            gsl_matrix_set (X, i, 0, 1.0);
            gsl_matrix_set (X, i, 1, xi);
            gsl_matrix_set (X, i, 2, xi * xi);
            gsl_matrix_set (X, i, 3, xi * xi * xi);
            gsl_matrix_set (X, i, 4, xi * xi * xi * xi);
            gsl_vector_set (y, i, yi);
            gsl_vector_set (w, i, 1.0 / (ei * ei));
        }
        {
	        gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc (POLYNOMIALDEGREE, POLYNOMIALDEGREE);
            gsl_multifit_wlinear (X, w, y, c, cov, &chisq, work);
            gsl_multifit_linear_free (work);
        }

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

        {
            int elevation;
            double coefficients[5];
            double position;

            coefficients[0] = C (0);
            coefficients[1] = C (1);
            coefficients[2] = C (2);
            coefficients[3] = C (3);
            coefficients[4] = C (4);

            for (elevation = 1; elevation <= 90; elevation++) {
                position = gsl_poly_eval (coefficients, POLYNOMIALDEGREE, elevation);
                actuatorsCorrections << elevation << " ";
                actuatorsCorrections.setf(ios::fixed);
                actuatorsCorrections.precision(3);
                actuatorsCorrections.width(5);
                actuatorsCorrections << position << endl;
            }
        }

        gsl_matrix_free (X);
        gsl_vector_free (y);
        gsl_vector_free (w);
        gsl_vector_free (c);
        gsl_matrix_free (cov);
    }

    return 0;
}
