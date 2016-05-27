/* ModelHighAPI.i */
%module ModelHighAPI

%{
  #include "ModelHighAPI_swig.h"
%}

%include "doxyhelp.i"

// to avoid error on this
#define ModelHighAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(ModelHighAPI_Interface)

// typemaps
%typemap(in) const ModelHighAPI_Double & (ModelHighAPI_Double temp) {
  if (PyFloat_Check($input) || PyInt_Check($input) || PyLong_Check($input)) {
    temp = ModelHighAPI_Double(PyFloat_AsDouble($input));
    $1 = &temp;
  } else if (PyString_Check($input)) {
    temp = ModelHighAPI_Double(PyString_AsString($input));
    $1 = &temp;
  } else if ((SWIG_ConvertPtr($input, (void **)&$1, $1_descriptor, SWIG_POINTER_EXCEPTION)) == 0) {
  } else {
    PyErr_SetString(PyExc_ValueError, "argument must be ModelHighAPI_Double, float, int or string.");
    return NULL;
  }
}

%typecheck(SWIG_TYPECHECK_POINTER) ModelHighAPI_Double, const ModelHighAPI_Double & {
  $1 = (PyFloat_Check($input) || PyInt_Check($input) || PyLong_Check($input) || PyString_Check($input)) ? 1 : 0;
}

// all supported interfaces
%include "ModelHighAPI_Double.h"
%include "ModelHighAPI_Interface.h"
