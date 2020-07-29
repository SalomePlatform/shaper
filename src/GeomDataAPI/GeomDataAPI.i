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

/* GeomDataAPI.i */
%module GeomDataAPI
%{
  #include "GeomDataAPI_swig.h"

  #define SWIGPY_UNICODE_ARG(obj) ((PyObject*) (obj))
%}

// import other modules
%import "GeomAPI.i"
%import "ModelAPI.i"

// to avoid error on this
#define GEOMDATAAPI_EXPORT
#define MODELAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
%include "std_shared_ptr.i"

%shared_ptr(GeomDataAPI_Point)
%shared_ptr(GeomDataAPI_Dir)
%shared_ptr(GeomDataAPI_Point2D)
%shared_ptr(GeomDataAPI_Point2DArray)

// all supported interfaces
%include "GeomDataAPI_Point.h"
%include "GeomDataAPI_Dir.h"
%include "GeomDataAPI_Point2D.h"
%include "GeomDataAPI_Point2DArray.h"

template<class T> std::shared_ptr<T> castTo(std::shared_ptr<ModelAPI_Attribute> theObject);
%template(geomDataAPI_Point) castTo<GeomDataAPI_Point>;
%template(geomDataAPI_Dir) castTo<GeomDataAPI_Dir>;
%template(geomDataAPI_Point2D) castTo<GeomDataAPI_Point2D>;
%template(geomDataAPI_Point2DArray) castTo<GeomDataAPI_Point2DArray>;
