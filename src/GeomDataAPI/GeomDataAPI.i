/* GeomDataAPI.i */
%module GeomDataAPI
%{
  #include "GeomDataAPI.h"
  #include "GeomDataAPI_Point.h"
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

// all supported interfaces
%include "GeomDataAPI_Point.h"
