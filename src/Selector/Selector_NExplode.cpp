// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#include "Selector_NExplode.h"

#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_NExplode.h>

////#include <TopoDS_Shape.hxx>
////#include <BRep_Tool.hxx>
////#include <TopoDS.hxx>
////#include <GProp_GProps.hxx>
////#include <BRepGProp.hxx>
////#include <NCollection_DataMap.hxx>
////#include <Precision.hxx>
////#include <Bnd_Box.hxx>
////#include <BRepBndLib.hxx>
////#include <TopExp_Explorer.hxx>
////#include <TopTools_MapOfShape.hxx>

static GeomAlgoAPI_NExplode::ShapeOrder getOrder(const bool isOld)
{
  return isOld ? GeomAlgoAPI_NExplode::ORDER_BY_HASH_VALUE
               : GeomAlgoAPI_NExplode::ORDER_BY_MIDDLE_POINT;
}

static GeomShapePtr convertShape(const TopoDS_Shape& theShape)
{
  GeomShapePtr aNewShape(new GeomAPI_Shape);
  aNewShape->setImpl(new TopoDS_Shape(theShape));
  return aNewShape;
}

Selector_NExplode::Selector_NExplode(const TopoDS_ListOfShape& theShapes, const bool theOldOrder)
  : myToBeReordered(theOldOrder)
{
  ListOfShape aShapes;
  for (TopoDS_ListOfShape::Iterator anIt(theShapes); anIt.More(); anIt.Next())
    aShapes.push_back(convertShape(anIt.Value()));

  mySorted = std::make_shared<GeomAlgoAPI_NExplode>(aShapes, getOrder(theOldOrder));
}

Selector_NExplode::Selector_NExplode(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType,
                                     const bool theOldOrder)
  : myToBeReordered(theOldOrder)
{
  GeomShapePtr aShape = convertShape(theShape);
  GeomAPI_Shape::ShapeType aType = (GeomAPI_Shape::ShapeType)theType;
  mySorted = std::make_shared<GeomAlgoAPI_NExplode>(aShape, aType, getOrder(theOldOrder));
}


int Selector_NExplode::index(const TopoDS_Shape& theSubShape)
{
  int anIndex = mySorted->index(convertShape(theSubShape));
  return anIndex > 0 ? anIndex : -1; // -1 if not found
}

TopoDS_Shape Selector_NExplode::shape(int& theIndex)
{
  TopoDS_Shape aResult;
  GeomShapePtr aShape = mySorted->shape(theIndex);
  if (aShape) {
    aResult = aShape->impl<TopoDS_Shape>();
    if (myToBeReordered) {
      mySorted->reorder(GeomAlgoAPI_NExplode::ORDER_BY_MIDDLE_POINT);
      theIndex = mySorted->index(aShape);
    }
  }
  return aResult;
}
