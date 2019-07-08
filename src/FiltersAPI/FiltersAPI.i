// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

/* FiltersAPI.i */

%module FiltersAPI

%{
  #include "FiltersAPI_swig.h"

  // fix for SWIG v2.0.4
  #define SWIGPY_SLICE_ARG(obj) ((PyObject*)(obj))
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define FILTERSAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_list.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(FiltersAPI_Feature)
%shared_ptr(FiltersAPI_Filter)

// function with named parameters
%feature("kwargs") addFilter;


// std::list -> []
%template(FilterList) std::list<std::shared_ptr<FiltersAPI_Filter> >;
%template(ArgumentList) std::list<FiltersAPI_Argument>;

// fix compilarion error: 'res*' was not declared in this scope
%typemap(freearg) const std::list<FiltersAPI_Argument> & {}

%typecheck(SWIG_TYPECHECK_POINTER) std::list<FiltersAPI_Argument>, const std::list<FiltersAPI_Argument> & {
  ModelHighAPI_Selection* temp_selection;
  std::string* temp_string;
  int newmem = 0;
  $1 = 1;
  if (PySequence_Check($input)) {
    for (Py_ssize_t i = 0; i < PySequence_Size($input); ++i) {
      PyObject * item = PySequence_GetItem($input, i);
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_selection, $descriptor(ModelHighAPI_Selection *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_selection) {
          $1 = 0;
        }
      } else
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_string, $descriptor(std::string *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_string) {
          $1 = 0;
        }
      } else
      if (!PyUnicode_Check(item) && !PyBool_Check(item))
        $1 = 0;
    }
  }
}

%typemap(in) const std::list<FiltersAPI_Argument> & (std::list<FiltersAPI_Argument> temp) {
  ModelHighAPI_Selection* temp_selection;
  std::string* temp_string;
  int newmem = 0;
  if (PySequence_Check($input)) {
    for (Py_ssize_t i = 0; i < PySequence_Size($input); ++i) {
      PyObject * item = PySequence_GetItem($input, i);
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_selection, $descriptor(ModelHighAPI_Selection*), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_selection) {
          PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Selection, string or boolean.");
          return NULL;
        }
        temp.push_back(FiltersAPI_Argument(*temp_selection));
        if (newmem & SWIG_CAST_NEW_MEMORY) {
          delete temp_selection;
        }
      } else
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_string, $descriptor(std::string*), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_string) {
          PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Selection, string or boolean.");
          return NULL;
        }
        temp.push_back(FiltersAPI_Argument(*temp_string));
        if (newmem & SWIG_CAST_NEW_MEMORY) {
          delete temp_string;
        }
      } else
      if (PyUnicode_Check(item)) {
        temp.push_back(FiltersAPI_Argument(PyUnicode_AsUTF8(item)));
      } else
      if (PyBool_Check(item)) {
        temp.push_back(FiltersAPI_Argument(item == Py_True));
      } else {
        PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Selection, string or boolean.");
        return NULL;
      }
      Py_DECREF(item);
    }
    $1 = &temp;
  } else {
    PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Selection, string or boolean.");
    return NULL;
  }
}


// all supported interfaces
%include "FiltersAPI_Argument.h"
%include "FiltersAPI_Feature.h"
%include "FiltersAPI_Filter.h"
%include "FiltersAPI_Selection.h"
