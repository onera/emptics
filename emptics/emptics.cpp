#pragma once
#include <mpi.h>
#include "mpi4py/mpi4py.h"
#include <pybind11/pybind11.h>

namespace maia {

namespace py = pybind11;

inline auto
mpi4py_comm_to_comm(py::handle mpi4py_comm) -> MPI_Comm {
  return ((PyMPICommObject*)mpi4py_comm.ptr())->ob_mpi;
}

} // maia
