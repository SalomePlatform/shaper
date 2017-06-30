// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "GeomAlgoAPI_Intersection.h"

#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BRepAlgoAPI_Section.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Builder.hxx>

//=================================================================================================
GeomAlgoAPI_Intersection::GeomAlgoAPI_Intersection(const ListOfShape& theObjects,
                                                   const ListOfShape& theTools)
{
  build(theObjects, theTools);
}

//=================================================================================================
void GeomAlgoAPI_Intersection::build(const ListOfShape& theObjects,
                                     const ListOfShape& theTools)
{
  if (theObjects.empty() || theTools.empty()) {
    return;
  }

  // Creating partition operation.
  BRepAlgoAPI_Section* anOperation = new BRepAlgoAPI_Section;
  this->setImpl(anOperation);
  this->setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  TopAbs_ShapeEnum aShapeType = TopAbs_COMPOUND;

  // Getting objects.
  TopTools_ListOfShape anObjects;
  for (ListOfShape::const_iterator
    anObjectsIt = theObjects.begin(); anObjectsIt != theObjects.end(); anObjectsIt++) {
    const TopoDS_Shape& aShape = (*anObjectsIt)->impl<TopoDS_Shape>();
    if(!aShape.IsNull()) {
      anObjects.Append(aShape);
    }
  }
  anOperation->SetArguments(anObjects);

  // Getting tools.
  TopTools_ListOfShape aTools;
  for (ListOfShape::const_iterator
    aToolsIt = theTools.begin(); aToolsIt != theTools.end(); aToolsIt++) {
    const TopoDS_Shape& aShape = (*aToolsIt)->impl<TopoDS_Shape>();
    if(!aShape.IsNull()) {
      aTools.Append(aShape);
    }
  }
  anOperation->SetTools(aTools);

  // Building and getting result.
  anOperation->Build();
  if(!anOperation->IsDone()) {
    return;
  }
  TopoDS_Shape aResult = anOperation->Shape();

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
