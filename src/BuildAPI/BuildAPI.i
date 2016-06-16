/* FeaturesAPI.i */

%module BuildAPI

%{
  #include "BuildAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define BUILDAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(BuildAPI_Edge)
%shared_ptr(BuildAPI_Vertex)

// all supported interfaces
%include "BuildAPI_Edge.h"
%include "BuildAPI_Vertex.h"
