/* PartSetAPI.i */

%module PartSetAPI

%{
  #include "PartSetAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define PARTSETAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(PartSetAPI_Part)

// all supported interfaces
%include "PartSetAPI_Part.h"
