#include "emptics/mpi4py.hpp"
#include "mpi4py/mpi4py.h"

namespace emptics {

MPI_Comm mpi4py_comm_to_comm(PyObject* mpi4py_comm) {
  return ((PyMPICommObject*)mpi4py_comm)->ob_mpi;
}

} // emptics
