// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef SRC_GEOMALGOAPI_GEOMALGOAPI_SWIG_H_
#define SRC_GEOMALGOAPI_GEOMALGOAPI_SWIG_H_

  #include <GeomAPI_swig.h>

  #include "GeomAlgoAPI.h"
  #include "GeomAlgoAPI_MakeShape.h"
  #include "GeomAlgoAPI_Boolean.h"
  #include "GeomAlgoAPI_Circ2dBuilder.h"
  #include "GeomAlgoAPI_CompoundBuilder.h"
  #include "GeomAlgoAPI_CurveBuilder.h"
  #include "GeomAlgoAPI_DFLoader.h"
  #include "GeomAlgoAPI_EdgeBuilder.h"
  #include "GeomAlgoAPI_FaceBuilder.h"
  #include "GeomAlgoAPI_MakeShapeCustom.h"
  #include "GeomAlgoAPI_MakeShapeList.h"
  #include "GeomAlgoAPI_MakeSweep.h"
  #include "GeomAlgoAPI_Transform.h"
  #include "GeomAlgoAPI_Translation.h"
  #include "GeomAlgoAPI_Placement.h"
  #include "GeomAlgoAPI_PointBuilder.h"
  #include "GeomAlgoAPI_Prism.h"
  #include "GeomAlgoAPI_Revolution.h"
  #include "GeomAlgoAPI_Rotation.h"
  #include "GeomAlgoAPI_ShapeTools.h"
  #include "GeomAlgoAPI_SketchBuilder.h"
  #include "GeomAlgoAPI_BREPExport.h"
  #include "GeomAlgoAPI_IGESExport.h"
  #include "GeomAlgoAPI_STEPExport.h"
  #include "GeomAlgoAPI_BREPImport.h"
  #include "GeomAlgoAPI_IGESImport.h"
  #include "GeomAlgoAPI_STEPImport.h"
  #include "GeomAlgoAPI_Tools.h"
  #include "GeomAlgoAPI_PaveFiller.h"
  #include "GeomAlgoAPI_Intersection.h"
  #include "GeomAlgoAPI_Pipe.h"
  #include "GeomAlgoAPI_WireBuilder.h"
  #include "GeomAlgoAPI_Sewing.h"
  #include "GeomAlgoAPI_ShapeBuilder.h"
  #include "GeomAlgoAPI_Exception.h"
  #include "GeomAlgoAPI_ShapeAPI.h"
  #include "GeomAlgoAPI_Box.h"
  #include "GeomAlgoAPI_ConeSegment.h"
  #include "GeomAlgoAPI_Copy.h"
  #include "GeomAlgoAPI_Symmetry.h"
  #include "GeomAlgoAPI_MapShapesAndAncestors.h"

  #include <memory>
  #include <string>
  #include <list>

  #ifdef _MSC_VER
  # pragma warning(disable: 4456) // declaration of variable hides previous local declaration
  # pragma warning(disable: 4459) // declaration of variable hides global declaration
  # pragma warning(disable: 4701) // potentially uninitialized local variable
  # pragma warning(disable: 4703) // potentially uninitialized local pointer variable
  #endif

#endif /* SRC_GEOMALGOAPI_GEOMALGOAPI_SWIG_H_ */
