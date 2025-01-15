# ------------------------------------------------------------------------------
# General CMake settings
# ------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
cmake_policy(SET CMP0074 NEW) # force find_package to take <PackageName>_ROOT variables into account

if(NOT DEFINED PROJECT_ROOT)
  set(PROJECT_ROOT ${CMAKE_SOURCE_DIR} CACHE PATH "Root directory, where the submodules are populated")
endif()

# Bootstrap project_utils
set(sub_repo_path "${PROJECT_ROOT}/external/project_utils")
file(GLOB sub_repo_files ${sub_repo_path}/*)
list(LENGTH sub_repo_files sub_repo_nb_files)
if(sub_repo_nb_files EQUAL 0)
  message(FATAL_ERROR
    "${sub_repo_path} is empty.
    Maybe you forgot to initialize it with \"git submodule update --init\""
  )
endif()
set(PROJECT_UTILS_DIR ${PROJECT_ROOT}/external/project_utils)
set(PROJECT_UTILS_CMAKE_DIR ${PROJECT_UTILS_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_UTILS_CMAKE_DIR}/find_package") # for custom Find*.cmake files
include(${PROJECT_UTILS_CMAKE_DIR}/dependency_management.cmake) # project_add_subdirectory, project_find_package and target_install

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")


# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------
project(
  emptics VERSION 0.1.0
  DESCRIPTION "empty project to prepare SoNICS dependencies"
  LANGUAGES C CXX Fortran
)

option(${PROJECT_NAME}_FORCE_TESTS "Make CTest run the tests" ON)
option(${PROJECT_NAME}_FORCE_CUDA "Fail if CUDA is not detected or has an error" OFF)
option(${PROJECT_NAME}_FORCE_HIP "Fail if HIP is not detected or has an error" OFF)
option(${PROJECT_NAME}_FORCE_DOCUMENTATION "Build ${PROJECT_NAME} documentation" OFF)
option(${PROJECT_NAME}_FORCE_PROFILING "Enable profiling tools for ${PROJECT_NAME}" OFF)

## Compiler flags
### C++ standard
set(SUPPORTED_CXX_STANDARDS 17 20)
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard to compile against")
endif()
if(NOT CMAKE_CXX_STANDARD IN_LIST SUPPORTED_CXX_STANDARDS)
  message(FATAL_ERROR "Supported CXX standards are: ${SUPPORTED_CXX_STANDARDS}.")
endif()

set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON )
### fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
### Compiler-dependent flags
include(${PROJECT_UTILS_CMAKE_DIR}/default_flags.cmake)
### Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE) # default to Release
endif()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # Needed for Topaze https://stackoverflow.com/a/61439452
### Additionnal build types
include(${PROJECT_UTILS_CMAKE_DIR}/additional_build_types.cmake)


# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------
find_package(MPI REQUIRED COMPONENTS C CXX Fortran)

find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development NumPy)
find_package(Mpi4Py REQUIRED)

find_package(Cython REQUIRED)
#find_package(pybind11 2.8 REQUIRED CONFIG)
include(${PROJECT_UTILS_CMAKE_DIR}/find_or_fetch.cmake)
find_or_fetch_pybind11()

find_package(HWLOC REQUIRED)
find_package(Threads REQUIRED) # Used by tasky

find_package(fmt 6.2 REQUIRED CONFIG)

find_package(PTSCOTCH 6.0.9)
find_package(ParMETIS 4.0.3)
if (NOT (PTSCOTCH_FOUND OR ParMETIS_FOUND))
  message(FATAL_ERROR "Need a partitioner but neither PtScotch nor ParMetis")
endif()

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

if(${PROJECT_NAME}_FORCE_TESTS)
  find_package(doctest 2.4.11 REQUIRED CONFIG)
endif()

# For GPU
if(${PROJECT_NAME}_FORCE_CUDA)
  find_package(CUDAToolkit REQUIRED)
  find_package(Thrust REQUIRED)
endif()
if(${PROJECT_NAME}_FORCE_HIP)
  find_package(rocprim REQUIRED)
  find_package(rocthrust REQUIRED)
endif()

## Profiling
if(${PROJECT_NAME}_FORCE_PROFILING)
  find_package(PAPI REQUIRED)
endif()

# Not used (yet?)
#find_package(OpenMP)

# Optim Intel
# Not used?
#find_package(MKL)

# Doc
if(${PROJECT_NAME}_FORCE_DOCUMENTATION)
  find_package(Doxygen 1.8.19 REQUIRED)
  find_package(Breathe REQUIRED)
  find_package(Jupyter REQUIRED)
  find_package(Jupytext REQUIRED)
  find_package(Sphinx 3 REQUIRED)
endif()


# ------------------------------------------------------------------------------
# Compilation
# ------------------------------------------------------------------------------
set(src_dir ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
set(test_dir ${PROJECT_SOURCE_DIR}/test)
set(include_dir ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE all_src_files
  CONFIGURE_DEPENDS "${src_dir}/*.cpp"
)

set(src_files ${all_src_files})
list(FILTER src_files EXCLUDE REGEX ".*\\.test\\.cpp$")
list(FILTER src_files EXCLUDE REGEX ".*\\.pybind\\.cpp$")

add_library(${PROJECT_NAME} SHARED ${src_files})

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${include_dir}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    MPI::MPI_CXX
    Mpi4Py::Mpi4Py
    Python::Python
    Python::NumPy
    Mpi4Py::Mpi4Py
    BLAS::BLAS
    LAPACK::LAPACK
    fmt::fmt
    Threads::Threads
    pybind11::pybind11_headers
)

if(${PROJECT_NAME}_FORCE_CUDA)
  target_link_libraries(${PROJECT_NAME_DEVICE}
    PUBLIC
      CUDA::cudart
  )
endif()
if(${PROJECT_NAME}_FORCE_HIP)
  target_link_libraries(${PROJECT_NAME_DEVICE}
    PRIVATE
      hip::device
      roc::rocprim
      roc::rocthrust
  )
endif()

# ------------------------------------------------------------------------------
# Create file to source build environnement
# ------------------------------------------------------------------------------
include(${PROJECT_UTILS_CMAKE_DIR}/write_build_env_file.cmake)
write_build_env_file()

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
target_install(${PROJECT_NAME})

# ------------------------------------------------------------------------------
# Python and wrapping
# ------------------------------------------------------------------------------
include(${PROJECT_UTILS_CMAKE_DIR}/python_wrapping.cmake)
compile_install_pybind_module(${PROJECT_NAME})
install_python_modules(${PROJECT_NAME})


# ------------------------------------------------------------------------------
# Testing
# ------------------------------------------------------------------------------
include(CTest)
if (${PROJECT_NAME}_ENABLE_TESTS)
  add_subdirectory(test)
endif()


# Not done
## ------------------------------------------------------------------------------
## Build documentation
## ------------------------------------------------------------------------------
#if(${PROJECT_NAME}_ENABLE_DOCUMENTATION)
#  include(${PROJECT_UTILS_CMAKE_DIR}/build_documentation.cmake)
#  build_sphinx_documentation()
#endif()

# ------------------------------------------------------------------------------
# Summary
# ------------------------------------------------------------------------------
get_directory_property(top_lvl_cmakelists PARENT_DIRECTORY)
if(top_lvl_cmakelists STREQUAL ${PROJECT_SOURCE_DIR})
  include(FeatureSummary)
  feature_summary(WHAT ALL)
endif()
