#include "emptics/blas_dependency.hpp"
#include <iostream>
#include <cassert>

extern "C" {
  void dgemm_(char *transA, char *transB, int *m, int *n, int *k,
                     double *alpha, double *A, int *lda,
                     double *B, int *ldb, double *beta,
                     double *C, int *ldc);
}

namespace emptics {

void call_blas() {
  int n = 2;
  double A[4] = {1.,2.,3.,4.};
  double B[4] = {5.,6.,7.,8.};
  double C[4] = {0.,0.,0.,0.};


  std::cout << "calling cblas_dgemm\n";
  std::cout << "A = [["
            << A[0] << ","
            << A[1] << "],["
            << A[2] << ","
            << A[3] << "]]\n";
  std::cout << "B = [["
            << B[0] << ","
            << B[1] << "],["
            << B[2] << ","
            << B[3] << "]]\n";

  char no_trans = 'N';
  double ONE = 1.;
  double ZERO = 0.;
  // we want to compute AB = C
  // we have A_c_layout = A_fortran_layout^T
  // we have B_c_layout = B_fortran_layout^T
  // we have C_c_layout = C_fortran_layout^T
  // So 
  //   A_c_layout * B_c_layout 
  // = A_fortran_layout^T * B_fortran_layout^T
  // = (B_fortran_layout * A_fortran_layout)^T // since the product of transposed is the swapped transposed product
  // = C_fortran_layout^T // and B_fortran_layout * A_fortran_layout = C_fortran_layout is computed by dgemm_
  // = C_c_layout
  dgemm_(&no_trans, &no_trans,
         &n, &n, &n, &ONE, B, &n, A, &n, &ZERO, C, &n);

  // (1 2)(5 6) = (19 22)
  // (3 4)(7 8)   (43 50)

  // convert to int to prevent round-off errors
  assert( int(C[0]) == 19 );
  assert( int(C[1]) == 22 );
  assert( int(C[2]) == 43 );
  assert( int(C[3]) == 50 );

  std::cout << "A*B = [["
            << C[0] << ","    // 19
            << C[1] << "],["  // 22
            << C[2] << ","    // 43
            << C[3] << "]]\n";// 50

}

} // emptics
