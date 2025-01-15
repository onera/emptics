# emptics #

empty project to prepare SoNICS dependencies"

Example use:

```
source /tmp_user/juno/sonics/dist/socle_cfd7/source.sh
```

```
cmake -S .. -DCMAKE_CXX_STANDARD=20 -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF -Demptics_FORCE_TESTS=ON -Demptics_FORCE_CUDA=OFF -Demptics_FORCE_HIP=OFF -Demptics_FORCE_DOCUMENTATION=OFF -Demptics_FORCE_PROFILING=OFF
make -j
source source.s:
python ../test/test_emptics.py
python ../emptics/all_imports.py
mpirun -np 2 python ../emptics/check_h5py_with_mpi.py
```


