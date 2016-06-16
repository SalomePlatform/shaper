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
%shared_ptr(FeaturesAPI_Boolean)
%shared_ptr(FeaturesAPI_Extrusion)
%shared_ptr(FeaturesAPI_ExtrusionBoolean)
%shared_ptr(FeaturesAPI_ExtrusionCut)
%shared_ptr(FeaturesAPI_ExtrusionFuse)
%shared_ptr(FeaturesAPI_Group)
%shared_ptr(FeaturesAPI_Intersection)
%shared_ptr(FeaturesAPI_Partition)
%shared_ptr(FeaturesAPI_Pipe)
%shared_ptr(FeaturesAPI_Placement)
%shared_ptr(FeaturesAPI_Revolution)
%shared_ptr(FeaturesAPI_RevolutionBoolean)
%shared_ptr(FeaturesAPI_RevolutionCut)
%shared_ptr(FeaturesAPI_RevolutionFuse)
%shared_ptr(FeaturesAPI_Rotation)
%shared_ptr(FeaturesAPI_Translation)

// all supported interfaces
%include "FeaturesAPI_Boolean.h"
%include "FeaturesAPI_Extrusion.h"
%include "FeaturesAPI_ExtrusionBoolean.h"
%include "FeaturesAPI_Group.h"
%include "FeaturesAPI_Intersection.h"
%include "FeaturesAPI_Partition.h"
%include "FeaturesAPI_Pipe.h"
%include "FeaturesAPI_Placement.h"
%include "FeaturesAPI_Revolution.h"
%include "FeaturesAPI_RevolutionBoolean.h"
%include "FeaturesAPI_Rotation.h"
%include "FeaturesAPI_Translation.h"
