/* GeomAPI.i */
%module GeomAlgoAPI
%{
  #include "memory"
  #include "GeomAlgoAPI.h"
  #include "GeomAlgoAPI_FaceBuilder.h"
  #include "GeomAlgoAPI_EdgeBuilder.h"
  #include "GeomAlgoAPI_PointBuilder.h"
  #include "GeomAlgoAPI_SketchBuilder.h"
%}

// to avoid error on this
#define GEOMALGOAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
//%include <std_shared_ptr.i>
%include <boost_shared_ptr.i>

%template(ShapeList) std::list<boost::shared_ptr<GeomAPI_Shape> >;

// all supported interfaces
%include "GeomAlgoAPI_FaceBuilder.h"
%include "GeomAlgoAPI_EdgeBuilder.h"
%include "GeomAlgoAPI_PointBuilder.h"
%include "GeomAlgoAPI_SketchBuilder.h"
