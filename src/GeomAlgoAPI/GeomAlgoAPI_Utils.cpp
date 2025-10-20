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

#include "GeomAlgoAPI_Utils.h"

#include <GEOMAlgo_AlgoTools.hxx>

#include <BRepCheck_Analyzer.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <TopAbs.hxx>


//==================================================================================================
bool GeomAlgoAPI_Utils::FixShapeTolerance(TopoDS_Shape& theShape,
                                          TopAbs_ShapeEnum theType,
                                          const double theTolerance,
                                          const bool theCheckGeometry,
                                          const bool theExactAdjust)
{
  ShapeFix_ShapeTolerance aSft;
  aSft.LimitTolerance(theShape, theTolerance, theTolerance, theType);
  if (theExactAdjust)
    GEOMAlgo_AlgoTools::FixCurveOnSurfaceTolerances(theShape);
  Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape(theShape);
  aSfs->Perform();
  theShape = aSfs->Shape();
  return CheckShape(theShape, theCheckGeometry, theExactAdjust);
}

//==================================================================================================
bool GeomAlgoAPI_Utils::CheckShape(TopoDS_Shape& theShape,
                                   const bool theCheckGeometry,
                                   const bool theExact)
{
  // Exact checking works only with enabled geometry checking
  Standard_Boolean isCheckGeom = theCheckGeometry || theExact;

  BRepCheck_Analyzer analyzer(theShape, isCheckGeom, Standard_False, theExact);
  return analyzer.IsValid();
}
