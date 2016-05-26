## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

set(CMAKE_SWIG_FLAGS "-Wall")

# For doxyhelp.i
include_directories(
  ${PROJECT_BINARY_DIR}/doc/swig
)
