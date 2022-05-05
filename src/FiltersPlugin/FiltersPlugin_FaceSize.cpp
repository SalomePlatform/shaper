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

#include "FiltersPlugin_FaceSize.h"

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeDouble.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Wire.h>

#include <Precision.hxx>

#include <map>
#include <cmath>

//=================================================================================================
bool FiltersPlugin_FaceSize::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::FACE;
}

//=================================================================================================
bool FiltersPlugin_FaceSize::isOk(const GeomShapePtr& theShape, const ResultPtr&,
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

  double aSurfArea = GeomAlgoAPI_ShapeTools::area(theShape);

  anAttr = theArgs.argument("comparatorType");
  AttributeStringPtr aCompAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(anAttr);
  if (!aCompAttr)
    return false;
  std::string aCompString = aCompAttr->value();

  bool isOK = false;
  if (aCompString == "inf")
    isOK = aSurfArea < aVal - Precision::Confusion();
  else if (aCompString == "infEq")
    isOK = aSurfArea < aVal + Precision::Confusion();
  else if (aCompString == "sup")
    isOK = aSurfArea > aVal + Precision::Confusion();
  else if (aCompString == "supEq")
    isOK = aSurfArea > aVal - Precision::Confusion();
  else if (aCompString == "isBetween")
    isOK = aVal <= aValMax
           && aSurfArea > aVal - Precision::Confusion()
           && aSurfArea < aValMax + Precision::Confusion();
  else if (aCompString == "isStrictlyBetween")
    isOK = aVal <= aValMax
           && aSurfArea > aVal + Precision::Confusion()
           && aSurfArea < aValMax - Precision::Confusion();
  return isOK;
}

//=================================================================================================
std::string FiltersPlugin_FaceSize::xmlRepresentation() const
{
  return xmlFromFile("filter-FaceSize.xml");
}

//=================================================================================================
void FiltersPlugin_FaceSize::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("comparatorType", ModelAPI_AttributeString::typeId());
  theArguments.initAttribute("value", ModelAPI_AttributeDouble::typeId());
  theArguments.initAttribute("valueMax", ModelAPI_AttributeDouble::typeId());
}
