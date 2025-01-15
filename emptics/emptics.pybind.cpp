#include <pybind11/pybind11.h>

int emptics_add(int i, int j) {
  return i + j;
}

PYBIND11_MODULE(emptics, m) {
  m.doc() = "emptics plugin";
  m.def("add", &emptics_add, "A function that adds two numbers");
}
