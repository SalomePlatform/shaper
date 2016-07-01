/* PrimitivesAPI.i */

%module PrimitivesAPI

%{
  #include "PrimitivesAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define PRIMITIVESAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(PrimitivesAPI_Box)

// all supported interfaces
%include "PrimitivesAPI_Box.h"
