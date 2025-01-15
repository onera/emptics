#include <Python.h>
#include <mpi.h>

namespace emptics {

MPI_Comm mpi4py_comm_to_comm(PyObject* mpi4py_comm);

} // emptics
