#include <pybind11/pybind11.h>

#include "emptics/hello.hpp"
#include "emptics/cblas_dependency.hpp"

namespace py = pybind11;

PYBIND11_MODULE(cemptics, m) {

  m.doc() = "emptics plugin";
  m.def("print_hello"   , &emptics::print_hello, "Print hello world");
  m.def("call_cblas_dep", &emptics::call_cblas_dep, "Test BLAS through its C API");
}
