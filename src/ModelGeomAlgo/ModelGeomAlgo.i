/* ModelAPI.i */
%module(directors="1") ModelGeomAlgo
%feature("director:except") {
    if ($error != NULL) {
      PyErr_Print();
      std::cerr << std::endl;
      throw Swig::DirectorMethodException();
    }
}

%{
  #include "ModelGeomAlgo_swig.h"
%}

// import other modules
%import "GeomAPI.i"
%import "GeomDataAPI.i"
%import "ModelAPI.i"


// to avoid error on this
#define MODELGEOMALGO_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
%include "std_shared_ptr.i"
%include "std_set.i"

// shared pointers
// For Point2D.method()
%shared_ptr(ModelGeomAlgo_Point2D)

// all supported interfaces
%include "ModelGeomAlgo_Point2D.h"

