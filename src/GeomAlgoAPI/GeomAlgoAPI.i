// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

/* GeomAPI.i */
%module GeomAlgoAPI
%{
  #include "GeomAlgoAPI_swig.h"
%}

// import other modules
%import "GeomAPI.i"

// to avoid error on this
#define GEOMALGOAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(GeomAlgoAPI_Boolean)
%shared_ptr(GeomAlgoAPI_Intersection)
%shared_ptr(GeomAlgoAPI_MakeShape)
%shared_ptr(GeomAlgoAPI_MakeShapeCustom)
%shared_ptr(GeomAlgoAPI_MakeShapeList)
%shared_ptr(GeomAlgoAPI_MakeSweep)
%shared_ptr(GeomAlgoAPI_PaveFiller)
%shared_ptr(GeomAlgoAPI_Pipe)
%shared_ptr(GeomAlgoAPI_Placement)
%shared_ptr(GeomAlgoAPI_Prism)
%shared_ptr(GeomAlgoAPI_Revolution)
%shared_ptr(GeomAlgoAPI_Rotation)
%shared_ptr(GeomAlgoAPI_Sewing)
%shared_ptr(GeomAlgoAPI_SketchBuilder)
%shared_ptr(GeomAlgoAPI_ShapeBuilder)
%shared_ptr(GeomAlgoAPI_Translation)
%shared_ptr(GeomAlgoAPI_Transform)
%shared_ptr(GeomAlgoAPI_Box)
%shared_ptr(GeomAlgoAPI_ConeSegment)
%shared_ptr(GeomAlgoAPI_Copy)
%shared_ptr(GeomAlgoAPI_Symmetry)

// all supported interfaces
%include "GeomAlgoAPI_MakeShape.h"
%include "GeomAlgoAPI_Boolean.h"
%include "GeomAlgoAPI_Circ2dBuilder.h"
%include "GeomAlgoAPI_CompoundBuilder.h"
%include "GeomAlgoAPI_CurveBuilder.h"
%include "GeomAlgoAPI_DFLoader.h"
%include "GeomAlgoAPI_EdgeBuilder.h"
%include "GeomAlgoAPI_FaceBuilder.h"
%include "GeomAlgoAPI_MakeShapeCustom.h"
%include "GeomAlgoAPI_MakeShapeList.h"
%include "GeomAlgoAPI_MakeSweep.h"
%include "GeomAlgoAPI_Translation.h"
%include "GeomAlgoAPI_Placement.h"
%include "GeomAlgoAPI_PointBuilder.h"
%include "GeomAlgoAPI_Prism.h"
%include "GeomAlgoAPI_Revolution.h"
%include "GeomAlgoAPI_Rotation.h"
%include "GeomAlgoAPI_ShapeTools.h"
%include "GeomAlgoAPI_SketchBuilder.h"
%include "GeomAlgoAPI_BREPExport.h"
%include "GeomAlgoAPI_IGESExport.h"
%include "GeomAlgoAPI_STEPExport.h"
%include "GeomAlgoAPI_BREPImport.h"
%include "GeomAlgoAPI_IGESImport.h"
%include "GeomAlgoAPI_STEPImport.h"
%include "GeomAlgoAPI_Tools.h"
%include "GeomAlgoAPI_Transform.h"
%include "GeomAlgoAPI_PaveFiller.h"
%include "GeomAlgoAPI_Intersection.h"
%include "GeomAlgoAPI_Pipe.h"
%include "GeomAlgoAPI_WireBuilder.h"
%include "GeomAlgoAPI_Sewing.h"
%include "GeomAlgoAPI_ShapeBuilder.h"
%include "GeomAlgoAPI_Exception.h"
%include "GeomAlgoAPI_ShapeAPI.h"
%include "GeomAlgoAPI_Copy.h"
%include "GeomAlgoAPI_Symmetry.h"
%include "GeomAlgoAPI_Box.h"

%typemap(out) std::list< std::shared_ptr< GeomAPI_Shape > >::value_type & {
  $result = SWIG_NewPointerObj(SWIG_as_voidptr(new std::shared_ptr<GeomAPI_Shape>(*$1)), $descriptor(std::shared_ptr<GeomAPI_Shape> *), SWIG_POINTER_OWN | 0 );
}
%template(ShapeList) std::list<std::shared_ptr<GeomAPI_Shape> >;
