## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

#TODO(spo): is -threads necessary?
set(CMAKE_SWIG_FLAGS -threads -Wall)

# Using doxy2swig:
# Add to ModuleName.i before any %inlcude:
#   %include "doxyhelp.i"
# Add to CMakeLists.txt before SWIG_ADD_MODULE(...):
#   SET(SWIG_MODULE_ModuleName_EXTRA_DEPS ${SWIG_MODULE_ModuleName_EXTRA_DEPS} doxyhelp.i)

#TODO(spo): why not use FindDoxygen.cmake module?
if(WIN32)
  find_program(DOXYGEN_EXECUTABLE Doxygen)
else()
  find_program(DOXYGEN_EXECUTABLE doxygen)
endif()

set(EXCLUDE_DOC_DIR "*/Test/* */Mock/*")

configure_file(${PROJECT_SOURCE_DIR}/doc/swig/doxyfile.in doxyfile @ONLY)

set(doc_sources)
foreach(it "*.h;*.cpp;*.hpp;*.cxx")
  file(GLOB doc_source RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${it})
  list(APPEND doc_sources ${doc_source})
endforeach()
#message(STATUS "${doc_sources}")

add_custom_command(OUTPUT doxyhelp.i
  COMMAND "${DOXYGEN_EXECUTABLE}"
  COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/doxy2swig.py xml/index.xml doxyhelp.i
  DEPENDS ${doc_sources}
)
