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
%shared_ptr(SketchAPI_Circle)
%shared_ptr(SketchAPI_Line)
%shared_ptr(SketchAPI_Sketch)
%shared_ptr(SketchAPI_SketchEntity)

// all supported interfaces
%include "SketchAPI_Circle.h"
%include "SketchAPI_Line.h"
%include "SketchAPI_Sketch.h"
%include "SketchAPI_SketchEntity.h"
