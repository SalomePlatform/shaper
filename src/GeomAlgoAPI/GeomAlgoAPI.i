/* GeomAPI.i */
%module GeomAlgoAPI
%{
  #include "GeomAlgoAPI.h"
  #include "GeomAlgoAPI_Boolean.h"
  #include "GeomAlgoAPI_CompoundBuilder.h"
  #include "GeomAlgoAPI_DFLoader.h"
  #include "GeomAlgoAPI_EdgeBuilder.h"
  #include "GeomAlgoAPI_Extrusion.h"
  #include "GeomAlgoAPI_FaceBuilder.h"
  #include "GeomAlgoAPI_MakeShape.h"
  #include "GeomAlgoAPI_PointBuilder.h"
  #include "GeomAlgoAPI_SketchBuilder.h"
%}

// to avoid error on this
#define GEOMALGOAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
%include "std_shared_ptr.i"

// all supported interfaces
%include "GeomAlgoAPI_Boolean.h"
%include "GeomAlgoAPI_CompoundBuilder.h"
%include "GeomAlgoAPI_DFLoader.h"
%include "GeomAlgoAPI_EdgeBuilder.h"
%include "GeomAlgoAPI_Extrusion.h"
%include "GeomAlgoAPI_FaceBuilder.h"
%include "GeomAlgoAPI_MakeShape.h"
%include "GeomAlgoAPI_PointBuilder.h"
%include "GeomAlgoAPI_SketchBuilder.h"

%template(ShapeList) std::list<std::shared_ptr<GeomAPI_Shape> >;