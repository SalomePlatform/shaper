/* CollectionAPI.i */

%module CollectionAPI

%{
#ifndef CollectionAPI_swig_H_
#define CollectionAPI_swig_H_

  #include <ModelHighAPI_swig.h>

  #include "CollectionAPI.h"
  #include "CollectionAPI_Group.h"

#endif // CollectionAPI_swig_H_
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define COLLECTIONAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(CollectionAPI_Group)

// all supported interfaces
%include "CollectionAPI_Group.h"
