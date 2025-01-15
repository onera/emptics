# from https://docs.h5py.org/en/stable/mpi.html

print("importing mpi4py.MPI")
from mpi4py import MPI
print("importing h5py")
import h5py

print("Try to write an HDF5 file in parallel")
rank = MPI.COMM_WORLD.rank  # The process ID (integer 0-3 for 4-process run)

f = h5py.File('parallel_test.hdf5', 'w', driver='mpio', comm=MPI.COMM_WORLD)

dset = f.create_dataset('test', (4,), dtype='i')
dset[rank] = rank

f.close()
print("done")
