// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

#ifndef __MATHLIB_H__
#define __MATHLIB_H__

#include <arsenal.h>

#include <modp_numtoa.h>


// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

#ifdef __cplusplus
extern "C" {
#endif                                      //  #ifdef __cplusplus

void Multiply_Self_Transpose(double*, double*, int, int);
void Multiply_Self_Transpose_f(float*, float*, int, int);
void Multiply_Self_Transpose_u(float*, I16*, int, int);
void Get_Submatrix(double*, int, int, double*, int, int, int);
int Choleski_LU_Decomposition(double*, int);
int Choleski_LU_Decomposition_f(float*, int);
int Choleski_LU_Inverse(double*, int);
int Choleski_LU_Inverse_f(float*, int);
void Multiply_Matrices(double*, double*, int, int, double*, int);
void Multiply_Matrices_f(float*, float*, int, int, float*, int);
void Multiply_Matrices_u(float*, float*, int, int, I16*, int);
void Multiply_Matrices_u8(float*, float*, int, int, I8*, int);
void Identity_Matrix(double*, int);
int Hessenberg_Form_Elementary(double*, double*, int);
void Hessenberg_Elementary_Transform(double*, double*, int[], int);
void Copy_Vector(double*, double*, int);
int QR_Hessenberg_Matrix(double*, double*, double[], double[], int, int);
void One_Real_Eigenvalue(double[], double[], double[], int, double);
void Two_Eigenvalues(double*, double*, double[], double[], int, int, double);
void Update_Row(double*, double, double, int, int);
void Update_Column(double*, double, double, int, int);
void Update_Transformation(double*, double, double, int, int);
void Double_QR_Iteration(double*, double*, int, int, int, double*, int);
void Product_and_Sum_of_Shifts(double*, int, int, double*, double*, double*, int);
int Two_Consecutive_Small_Subdiagonal(double*, int, int, int, double, double);
void Double_QR_Step(double*, int, int, int, double, double, double*, int);
void BackSubstitution(double*, double[], double[], int);
void BackSubstitute_Real_Vector(double*, double[], double[], int, double, int);
void BackSubstitute_Complex_Vector(double*, double[], double[], int, double, int);
void Calculate_Eigenvectors(double*, double*, double[], double[], int);
void Complex_Division(double, double, double, double, double*, double*);
void Transpose_Square_Matrix(double*, int);

int sphere_fit_least_squares(const float x[], const float y[], const float z[],
			     unsigned int size, unsigned int max_iterations, float delta, float *sphere_x, float *sphere_y, float *sphere_z, float *sphere_radius);


int ftoa( char *outbuf, float f );
int ftoa_prec_f0( char *outbuf, float f );

#ifdef __cplusplus
}
#endif                                      //  #ifdef __cplusplus


#endif // __MATHLIB_H__
