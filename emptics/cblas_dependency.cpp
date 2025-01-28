#include "emptics/cblas_dependency.hpp"
#include <iostream>

extern "C" {
  #include "cblas.h"
}

namespace emptics {

void call_cblas_dep() {
  const int n = 2;
  double A[4] = {1.,0.,0.,2.};
  double B[4] = {3.,0.,0.,4.};
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
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
              n, n, n, 1., A, n, B, n, 0., C, n);

  std::cout << "A*B = [["
            << C[0] << ","
            << C[1] << "],["
            << C[2] << ","
            << C[3] << "]]\n";
}

} // emptics
