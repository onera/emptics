print("importing mpi4py.MPI")
import mpi4py
from mpi4py import MPI
import subprocess

comm = MPI.COMM_WORLD

assert comm.size == 2, "this test should be launched on two processes"

if comm.rank==0:
  value = "Hello"
else:
  value = "World"

print(comm.allgather(value))


## TODO DEL (even if fails, can be correct)
## Check mpi4py uses the same MPI as the one loaded on the environment
## Note: the use of `mpicc` is just a trick to get both path,
##       but the test is not really about to `mpicc`
#mpi4py_mpicc = mpi4py.get_config()['mpicc']
#res = subprocess.run(["which", "mpicc"], capture_output=True)
#
#res.check_returncode()
#env_mpicc = res.stdout.decode('utf-8').strip()
#assert mpi4py_mpicc == env_mpicc
#print(env_mpicc)
