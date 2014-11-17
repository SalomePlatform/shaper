/* GeomAPI.i */
%module GeomAPI
%{
  #include "GeomAPI.h"
  #include "GeomAPI_Interface.h"
  #include "GeomAPI_Pnt.h"
  #include "GeomAPI_Dir.h"
  #include "GeomAPI_Pln.h"
  #include "GeomAPI_Shape.h"
%}

// to avoid error on this
#define GEOMAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(GeomAPI_Interface)
%shared_ptr(GeomAPI_Pnt)
%shared_ptr(GeomAPI_Dir)
%shared_ptr(GeomAPI_Pln)
%shared_ptr(GeomAPI_Shape)

// all supported interfaces
%include "GeomAPI_Interface.h"
%include "GeomAPI_Pnt.h"
%include "GeomAPI_Dir.h"
%include "GeomAPI_Pln.h"
%include "GeomAPI_Shape.h"
