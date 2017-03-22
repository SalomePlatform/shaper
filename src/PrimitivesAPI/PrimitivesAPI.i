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
%shared_ptr(PrimitivesAPI_Cone)
%shared_ptr(PrimitivesAPI_Cylinder)
%shared_ptr(PrimitivesAPI_Sphere)
%shared_ptr(PrimitivesAPI_Torus)

// all supported interfaces
%include "PrimitivesAPI_Box.h"
%include "PrimitivesAPI_Cone.h"
%include "PrimitivesAPI_Cylinder.h"
%include "PrimitivesAPI_Sphere.h"
%include "PrimitivesAPI_Torus.h"
