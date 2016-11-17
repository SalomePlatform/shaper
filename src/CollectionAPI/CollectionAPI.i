/* CollectionAPI.i */

%module CollectionAPI

%{
#ifndef CollectionAPI_swig_H_
#define CollectionAPI_swig_H_

  #include <ModelHighAPI_swig.h>

  #include "CollectionAPI.h"
  #include "CollectionAPI_Group.h"
  #include "CollectionAPI_Field.h"

#endif // CollectionAPI_swig_H_
%}

%{
  #include "ModelHighAPI_swig.h"

  // fix for SWIG v2.0.4
  #define SWIGPY_SLICE_ARG(obj) ((PySliceObject*)(obj))
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define COLLECTIONAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_list.i"
%include "std_string.i"
%include "std_shared_ptr.i"

%template(StringList) std::list<std::string>;
%template(IntegerList) std::list<int>;
%template(DoubleList) std::list<double>;
%template(BooleanList) std::list<bool>;
%template(StringListList) std::list<std::list<std::string> >;
%template(IntegerListList) std::list<std::list<int> >;
%template(DoubleListList) std::list<std::list<double> >;
%template(BooleanListList) std::list<std::list<bool> >;

// shared pointers
%shared_ptr(CollectionAPI_Group)
%shared_ptr(CollectionAPI_Field)

// all supported interfaces
%include "CollectionAPI_Group.h"
%include "CollectionAPI_Field.h"
