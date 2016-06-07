/* FeaturesAPI.i */

%module FeaturesAPI

%{
  #include "FeaturesAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define FEATURESAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(FeaturesAPI_Placement)
%shared_ptr(FeaturesAPI_Rotation)
%shared_ptr(FeaturesAPI_Translation)

// all supported interfaces
%include "FeaturesAPI_Placement.h"
%include "FeaturesAPI_Rotation.h"
%include "FeaturesAPI_Translation.h"
