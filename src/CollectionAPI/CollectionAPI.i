// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

/* CollectionAPI.i */

%module CollectionAPI

%{
#ifndef CollectionAPI_swig_H_
#define CollectionAPI_swig_H_

  #include <ModelHighAPI_swig.h>

  #include "CollectionAPI.h"
  #include "CollectionAPI_Group.h"
  #include "CollectionAPI_GroupAddition.h"
  #include "CollectionAPI_GroupIntersection.h"
  #include "CollectionAPI_GroupSubstraction.h"
  #include "CollectionAPI_Field.h"

#endif // CollectionAPI_swig_H_

  // fix for SWIG v2.0.4
  #define SWIGPY_SLICE_ARG(obj) ((PyObject*)(obj))
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
%shared_ptr(CollectionAPI_GroupAddition)
%shared_ptr(CollectionAPI_GroupIntersection)
%shared_ptr(CollectionAPI_GroupSubstraction)
%shared_ptr(CollectionAPI_Field)

// all supported interfaces
%include "CollectionAPI_Group.h"
%include "CollectionAPI_GroupAddition.h"
%include "CollectionAPI_GroupIntersection.h"
%include "CollectionAPI_GroupSubstraction.h"
%include "CollectionAPI_Field.h"
