/* SketchAPI.i */

%module SketchAPI

%{
  #include "SketchAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define SKETCHAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_list.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(SketchAPI_Arc)
%shared_ptr(SketchAPI_Circle)
%shared_ptr(SketchAPI_Constraint)
%shared_ptr(SketchAPI_IntersectionPoint)
%shared_ptr(SketchAPI_Line)
%shared_ptr(SketchAPI_Mirror)
%shared_ptr(SketchAPI_Sketch)
%shared_ptr(SketchAPI_SketchEntity)
%shared_ptr(SketchAPI_Point)
%shared_ptr(SketchAPI_Projection)
%shared_ptr(SketchAPI_Rectangle)
%shared_ptr(SketchAPI_Rotation)
%shared_ptr(SketchAPI_Translation)

// all supported interfaces (the order is very important according dependencies: base class first)
%include "SketchAPI_SketchEntity.h"
%include "SketchAPI_Point.h"
%include "SketchAPI_IntersectionPoint.h"
%include "SketchAPI_Line.h"
%include "SketchAPI_Circle.h"
%include "SketchAPI_Arc.h"
%include "SketchAPI_Projection.h"
%include "SketchAPI_Mirror.h"
%include "SketchAPI_Translation.h"
%include "SketchAPI_Rectangle.h"
%include "SketchAPI_Rotation.h"
%include "SketchAPI_Sketch.h"
%include "SketchAPI_Constraint.h"
