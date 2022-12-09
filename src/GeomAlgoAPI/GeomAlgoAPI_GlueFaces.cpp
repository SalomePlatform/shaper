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

#include "GeomAlgoAPI_GlueFaces.h"

#include <GeomAPI_ShapeIterator.h>
#include <GEOMAlgo_Gluer2.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//==================================================================================================
GeomAlgoAPI_GlueFaces::GeomAlgoAPI_GlueFaces(const ListOfShape& theShapes, const double theTolerance/*=1.e-7*/, const bool theKeepNonSolids/*=true*/)
{
  build(theShapes, theTolerance, theKeepNonSolids);
}

//==================================================================================================
void GeomAlgoAPI_GlueFaces::build(const ListOfShape& theShapes, const double theTolerance, const bool theKeepNonSolids)
{
  if (theShapes.empty())
  {
    return;
  }

  // Create a compound of all shapes in the list
  TopoDS_Shape aBaseShape;
  if (theShapes.size() == 1)
  {
    aBaseShape = theShapes.front()->impl<TopoDS_Shape>();
  }
  else
  {
    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound(aCompound);
    ListOfShape::const_iterator anIt = theShapes.cbegin();
    for (; anIt != theShapes.cend(); ++anIt)
    {
      const TopoDS_Shape& aShape = (*anIt)->impl<TopoDS_Shape>();
      if (aShape.IsNull()) break;
      aBuilder.Add(aCompound, aShape);
    }
    if (anIt == theShapes.cend())
    {
      aBaseShape = aCompound;
    }
    else
    {
      aBaseShape.Nullify();
    }
  }
  if (aBaseShape.IsNull())
  {
    myError = "Shape for gluing is null";
    return;
  }

  GEOMAlgo_Gluer2 aGlueAlgo;

  // Initialization
  aGlueAlgo.SetArgument(aBaseShape);
  aGlueAlgo.SetTolerance(theTolerance);
  Standard_Boolean aIsKeepNonSolids = theKeepNonSolids;
  aGlueAlgo.SetKeepNonSolids(aIsKeepNonSolids);

  // Detect interfered shapes
  aGlueAlgo.Detect();
  Standard_Integer iWrnDetect = aGlueAlgo.WarningStatus();
  if (iWrnDetect == 2)
  {
    myError = "Glue Error Sticked Shapes";
    return;
  }

  Standard_Integer iErr = aGlueAlgo.ErrorStatus();
  if (iErr)
  {
    switch (iErr)
    {
    case 11:
      myError = "GEOMAlgo_GlueDetector failed";
      break;
    case 13:
    case 14:
      myError = "PerformImagesToWork failed";
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer2.cxx
        myError = "Error in GEOMAlgo_Gluer2 with code ";
        myError += std::to_string(iErr);
        break;
      }
    }
    return;
  }

  // Gluing
  aGlueAlgo.Perform();
  iErr = aGlueAlgo.ErrorStatus();
  if (iErr)
  {
    switch (iErr)
    {
    case 11:
      myError = "GEOMAlgo_GlueDetector failed";
      break;
    case 13:
    case 14:
      myError = "PerformImagesToWork failed";
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer2.cxx
        myError = "Error in GEOMAlgo_Gluer2 with code ";
        myError += std::to_string(iErr);
        break;
      }
    }
    return;
  }

  Standard_Integer iWrn = aGlueAlgo.WarningStatus();
  if (iWrn)
  {
    switch (iWrn)
    {
    case 1:
      myError = "No shapes to glue";
      break;
    default:
      // description of all warnings see in GEOMAlgo_Gluer2.cxx
      myError = "Warning in GEOMAlgo_Gluer2 with code ";
      myError += std::to_string(iWrn);
      break;
    }
  }

  // Result
  TopoDS_Shape aResult = aGlueAlgo.Shape();

  std::shared_ptr<GeomAPI_Shape> aResShape(new GeomAPI_Shape());
  aResShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aResShape);
  this->setDone(true);
}
