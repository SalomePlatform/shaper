/* GeomDataAPI.i */
%module GeomDataAPI
%{
  #include "GeomDataAPI.h"
  #include "GeomDataAPI_Point.h"
  #include "GeomDataAPI_Dir.h"
  #include "GeomDataAPI_Point2D.h"
  #include <boost/shared_ptr.hpp>
%}

// to avoid error on this
#define GEOMDATAAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
//%include <std_shared_ptr.i>

// boost pointers
%include <boost_shared_ptr.i>
%shared_ptr(GeomDataAPI_Point)
%shared_ptr(GeomDataAPI_Dir)
%shared_ptr(GeomDataAPI_Point2D)

// all supported interfaces
%include "GeomDataAPI_Point.h"
%include "GeomDataAPI_Dir.h"
%include "GeomDataAPI_Point2D.h"
