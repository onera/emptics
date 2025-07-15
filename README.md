# emptics #

**emptics** checks if an environment satisfies the SoNICS dependencies. It looks for the libraries that SoNICS depends upon, and tests that they are installed correctly for SoNICS.

The sources of **emptics** are small, self-contained and non-confidential. They can be given to clients and IT managements, so that they can test that their environment conforms to the SoNICS prerequisites.

**emptics** mainly consists of:
- a CMake file that calls `find_packages` for all SoNICS dependencies,
- a C++ virtually empty library,
- a Python binary module using Pybind11,
- a Python file that `import`s the Python modules that SoNICS and its submodules import,
- a handful of executables that test some libraries that SoNICS depend upon (HDF5, ParMETIS, PtSCOTCH, CUDA...),
- a handful of Python scripts that make some basic tests with dependencies (e.g. check that `h5py` works with parallel support).

Example use:

1. Source an environment you want to test:
```
source /tmp_user/juno/sonics/dist/socle_cfd7/source.sh
```

2. Configure, build and run **emptics** tests:
```
cmake -S $EMPTICS_SOURCE_DIR -DCMAKE_CXX_STANDARD=20 -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF -Demptics_TEST_HDF5_THROUGH_CMAKE=OFF -DCMAKE_GPU_RUNTIME=CUDA
make -j
source source.sh # source local artifacts (.so, Python modules...)
python ../test/test_emptics.py
mpirun -np 2 ./emptics_parmetis # test parmetis
mpirun -np 2 ./emptics_ptscotch # test ptscotch
mpirun -np 1 python ../test/test_emptics.py # test python modules can be imported
mpirun -np 2 python ../emptics/check_mpi4py.py # test mpi4py
mpirun -np 2 python ../emptics/check_h5py_with_mpi.py # test h5py with mpi capabilities
./emptics_gpu # test basic GPU access
```

Emptics-specific CMake options:
- `emptics_TEST_PARMETIS`: build a ParMETIS test executable
- `emptics_TEST_PTSCOTCH`: build a PtSCOTCH test executable
- `emptics_TEST_HDF5_THROUGH_CMAKE`: build a HDF5 test executable (note: some versions of hdf5 won't build because of [this bug](https://github.com/HDFGroup/hdf5/issues/2621). This doesn't mean that the h5py installation does not work, so this test can be deactivated)

General CMake options of particular interest:
- `CMAKE_INTERPROCEDURAL_OPTIMIZATION`: Enable link-time optimizations. Always turned ON or OFF for all SoNICS targets (default: mix of ON for pybind targets and OFF for other targets, may crash the Python imports).
- `CMAKE_CXX_STANDARD=20`: Recommended. `CMAKE_CXX_STANDARD=17` is also possible.
