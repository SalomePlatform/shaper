/* GeomDataAPI.i */
%module GeomDataAPI
%{
  #include <boost/shared_ptr.hpp>

  #include "ModelAPI_Attribute.h"
  #include "GeomDataAPI.h"
  #include "GeomDataAPI_Point.h"
  #include "GeomDataAPI_Dir.h"
  #include "GeomDataAPI_Point2D.h"
  
  template<class T> boost::shared_ptr<T> castTo(boost::shared_ptr<ModelAPI_Attribute> theObject) 
  { 
    return boost::dynamic_pointer_cast<T>(theObject); 
  } 
%}

// to avoid error on this
#define GEOMDATAAPI_EXPORT
#define MODELAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"

// boost pointers
%include <boost_shared_ptr.i>
%shared_ptr(ModelAPI_Attribute)
%shared_ptr(GeomDataAPI_Point)
%shared_ptr(GeomDataAPI_Dir)
%shared_ptr(GeomDataAPI_Point2D)

// all supported interfaces
%include "ModelAPI_Attribute.h"
%include "GeomDataAPI_Point.h"
%include "GeomDataAPI_Dir.h"
%include "GeomDataAPI_Point2D.h"

template<class T> boost::shared_ptr<T> castTo(boost::shared_ptr<ModelAPI_Attribute> theObject);
%template(geomDataAPI_Point) castTo<GeomDataAPI_Point>;
%template(geomDataAPI_Dir) castTo<GeomDataAPI_Dir>;
%template(geomDataAPI_Point2D) castTo<GeomDataAPI_Point2D>;
