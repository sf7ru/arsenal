////////////////////////////////////////////////////////////////////////////////
// File: multiply_self_transpose.c                                            //
// Routine(s):                                                                //
//    Multiply_Self_Transpose                                                 //
////////////////////////////////////////////////////////////////////////////////

#include <mathlib.h>

////////////////////////////////////////////////////////////////////////////////
//  void Multiply_Self_Transpose(double *C, double *A, int nrows, int ncols ) //
//                                                                            //
//  Description:                                                              //
//     Post multiply an nrows x ncols matrix A by its transpose.   The result //
//     is an  nrows x nrows square symmetric matrix C, i.e. C = A A', where ' //
//     denotes the transpose.                                                 //
//     The matrix C should be declared as double C[nrows][nrows] in the       //
//     calling routine.  The memory allocated to C should not include any     //
//     memory allocated to A.                                                 //
//                                                                            //
//  Arguments:                                                                //
//     double *C    Pointer to the first element of the matrix C.             //
//     double *A    Pointer to the first element of the matrix A.             //
//     int    nrows The number of rows of matrix A.                           //
//     int    ncols The number of columns of the matrices A.                  //
//                                                                            //
//  Return Values:                                                            //
//     void                                                                   //
//                                                                            //
//  Example:                                                                  //
//     #define N                                                              //
//     #define M                                                              //
//     double A[M][N], C[M][M];                                               //
//                                                                            //
//     (your code to initialize the matrix A)                                 //
//                                                                            //
//     Multiply_Self_Transpose(&C[0][0], &A[0][0], M, N);                     //
//     printf("The matrix C = AA ' is \n"); ...                               //
////////////////////////////////////////////////////////////////////////////////
void Multiply_Self_Transpose(double *C, double *A, int nrows, int ncols)
{
   int i,j,k;
   double *pA;
   double *p_A = A;
   double *pB;
   double *pCdiag = C;
   double *pC = C;
   double *pCt;

   for (i = 0; i < nrows; pCdiag += nrows + 1, p_A = pA, i++) {
      pC = pCdiag;
      pCt = pCdiag;
      pB = p_A; 
      for (j = i; j < nrows; pC++, pCt += nrows, j++) {
         pA = p_A;
         *pC = 0.0; 
         for (k = 0; k < ncols; k++) *pC += *(pA++) * *(pB++);
         *pCt = *pC;
      }
   }
}

void Multiply_Self_Transpose_f(float *C, float *A, int nrows, int ncols)
{
   int i,j,k;
   float  *pA;
   float  *p_A = A;
   float  *pB;
   float  *pCdiag = C;
   float  *pC = C;
   float  *pCt;

   for (i = 0; i < nrows; pCdiag += nrows + 1, p_A = pA, i++) {
      pC = pCdiag;
      pCt = pCdiag;
      pB = p_A;
      for (j = i; j < nrows; pC++, pCt += nrows, j++) {
         pA = p_A;
         *pC = 0.0;
         for (k = 0; k < ncols; k++) *pC += *(pA++) * *(pB++);
         *pCt = *pC;
      }
   }
}

void Multiply_Self_Transpose_u(float *C, I16 *A, int nrows, int ncols)
{
   int i,j,k;
   I16    *pA;
   I16    *p_A = A;
   I16    *pB;
   float  *pCdiag = C;
   float  *pC = C;
   float  *pCt;

   for (i = 0; i < nrows; pCdiag += nrows + 1, p_A = pA, i++) {
      pC = pCdiag;
      pCt = pCdiag;
      pB = p_A;
      for (j = i; j < nrows; pC++, pCt += nrows, j++) {
         pA = p_A;
         *pC = 0.0;
         for (k = 0; k < ncols; k++) *pC += *(pA++) * *(pB++);
         *pCt = *pC;
      }
   }
}
