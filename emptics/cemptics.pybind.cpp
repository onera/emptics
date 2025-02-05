#include <pybind11/pybind11.h>

#include "emptics/hello.hpp"
#include "emptics/blas_dependency.hpp"

namespace py = pybind11;

PYBIND11_MODULE(cemptics, m) {

  m.doc() = "emptics plugin";
  m.def("print_hello"   , &emptics::print_hello, "Print hello world");
  m.def("call_blas", &emptics::call_blas, "Test BLAS through its C API");
}
