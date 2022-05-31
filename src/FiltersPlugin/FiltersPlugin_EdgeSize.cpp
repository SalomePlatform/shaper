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

#include "FiltersPlugin_EdgeSize.h"

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Wire.h>

#include <GeomAlgoAPI_ShapeTools.h>

#include <Precision.hxx>

#include <map>
#include <cmath>

//=================================================================================================
bool FiltersPlugin_EdgeSize::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::EDGE;
}

//=================================================================================================
bool FiltersPlugin_EdgeSize::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                  const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr anAttr = theArgs.argument("value");
  AttributeDoublePtr aValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr);
  if (!aValue.get()|| !anAttr->isInitialized() )
    return false;
  double aVal = aValue->value();

  anAttr = theArgs.argument("valueMax");
  aValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr);
  if (!aValue.get()|| !anAttr->isInitialized() )
    return false;
  double aValMax = aValue->value();

  if (aVal < 0.0)
    return false;

  GeomEdgePtr anEdge;
  switch (theShape->shapeType()) {
  case GeomAPI_Shape::EDGE:
    anEdge = GeomEdgePtr(new GeomAPI_Edge(theShape));
    break;
  case GeomAPI_Shape::WIRE:
    anEdge = GeomAlgoAPI_ShapeTools::wireToEdge(
        GeomWirePtr(new GeomAPI_Wire(theShape)));
    break;
  default:
    return false;
  }

  double aLength = anEdge->length();

  anAttr = theArgs.argument("comparatorType");
  AttributeStringPtr aCompAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(anAttr);
  if (!aCompAttr)
    return false;
  std::string aCompString = aCompAttr->value();

  bool isOK = false;
  if (aCompString == "inf")
    isOK = aLength < aVal - Precision::Confusion();
  else if (aCompString == "infEq")
    isOK = aLength < aVal + Precision::Confusion();
  else if (aCompString == "sup")
    isOK = aLength > aVal + Precision::Confusion();
  else if (aCompString == "supEq")
    isOK = aLength > aVal - Precision::Confusion();
  else if (aCompString == "isBetween")
    isOK = aVal <= aValMax
           && aLength > aVal - Precision::Confusion()
           && aLength < aValMax + Precision::Confusion();
  else if (aCompString == "isStrictlyBetween")
    isOK = aVal <= aValMax
           && aLength > aVal + Precision::Confusion()
           && aLength < aValMax - Precision::Confusion();
  return isOK;
}

//=================================================================================================
std::string FiltersPlugin_EdgeSize::xmlRepresentation() const
{
  return xmlFromFile("filter-EdgeSize.xml");
}

//=================================================================================================
void FiltersPlugin_EdgeSize::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("comparatorType", ModelAPI_AttributeString::typeId());
  theArguments.initAttribute("value", ModelAPI_AttributeDouble::typeId());
  theArguments.initAttribute("valueMax", ModelAPI_AttributeDouble::typeId());
}
