/* ParametersAPI.i */

%module ParametersAPI

%{
  #include "ParametersAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define PARAMETERSAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(ParametersAPI_Parameter)

// all supported interfaces
%include "ParametersAPI_Parameter.h"
