// Copyright (C) 2014-2025  CEA, EDF
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

#include "GeomAlgoAPI_LimitTolerance.h"

#include "GeomAlgoAPI_Utils.h"

#include <BRepBuilderAPI_Copy.hxx>
#include <TNaming_CopyShape.hxx>
#include <TopoDS_Shape.hxx>


//==================================================================================================
GeomAlgoAPI_LimitTolerance::GeomAlgoAPI_LimitTolerance(const GeomShapePtr theShape,
                                                       const double theTolerance,
                                                       const bool theCheckGeometry,
                                                       const bool theExactAdjust)
{
  build(theShape, theTolerance, theCheckGeometry, theExactAdjust);
}

//==================================================================================================
void GeomAlgoAPI_LimitTolerance::build(const GeomShapePtr theShape,
                                       const double theTolerance,
                                       const bool theCheckGeometry,
                                       const bool theExactAdjust)
{
  if (!theShape.get()) {
    return;
  }

  const TopoDS_Shape& aOriginalShape = theShape->impl<TopoDS_Shape>();
  Standard_Real aTol = theTolerance;

  if (aTol < Precision::Confusion())
    aTol = Precision::Confusion();

  // 1. Make a copy to prevent the original shape changes.
  TopoDS_Shape aShapeCopy;
  {
    BRepBuilderAPI_Copy aMC (aOriginalShape);
    if (aMC.IsDone()) {
      aShapeCopy = aMC.Shape();
    }
    else {
      TColStd_IndexedDataMapOfTransientTransient aMapTShapes;
      TNaming_CopyShape::CopyTool(aOriginalShape, aMapTShapes, aShapeCopy);
    }
  }

  // 2. Limit tolerance.
  if (!GeomAlgoAPI_Utils::FixShapeTolerance(aShapeCopy, TopAbs_SHAPE, aTol,
                                            theCheckGeometry, theExactAdjust))
  {
    return;
  }

  // 3. Set the result
  TopoDS_Shape aResult = aShapeCopy;

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
