# emptics #

**emptics** checks if an environment satisfies the SoNICS dependencies. It looks for the libraries that SoNICS depends upon, and tests that they are installed correctly for SoNICS.

The sources of **emptics** are small, self-contained and non-confidential. They can be given to clients and IT managements, so that they can test that their environment conforms to the SoNICS prerequisites.

**emptics** mainly consists of:
- a CMake file that calls `find_packages` for all SoNICS dependencies,
- a C++ virtually empty library,
- a Python binary module using Pybind11,
- a Python file that `import`s the Python modules that SoNICS and its submodules import,
- a handful of Python files that make some basic tests with dependencies (e.g. check that `h5py` works with parallel support).

Example use:

1. Source an environment you want to test:
```
source /tmp_user/juno/sonics/dist/socle_cfd7/source.sh
```

2. Configure, build and run **emptics** tests:
```
cmake -S .. -DCMAKE_CXX_STANDARD=20 -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF -Demptics_FORCE_TESTS=ON -Demptics_FORCE_CUDA=OFF -Demptics_FORCE_HIP=OFF -Demptics_FORCE_DOCUMENTATION=OFF -Demptics_FORCE_PROFILING=OFF
make -j
source source.sh
python ../test/test_emptics.py
mpirun -np 2 python ../emptics/check_h5py_with_mpi.py
```
