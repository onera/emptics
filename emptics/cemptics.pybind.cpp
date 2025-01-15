#include <pybind11/pybind11.h>

#include "emptics/hello.hpp"

namespace py = pybind11;

PYBIND11_MODULE(cemptics, m) {

  m.doc() = "emptics plugin";
  m.def("print_hello", &emptics::print_hello, "Print hello world");
}
