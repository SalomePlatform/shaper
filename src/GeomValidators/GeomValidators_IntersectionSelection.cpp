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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "GeomValidators_IntersectionSelection.h"

#include <GeomAPI_Shape.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Feature.h>

bool GeomValidators_IntersectionSelection::isValid(const AttributePtr& theAttribute,
                                                   const std::list<std::string>& theArguments,
                                                   Events_InfoMessage& theError) const
{
  if(!theAttribute.get()) {
    theError = "Error: empty selection.";
    return false;
  }
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionListPtr anAttrSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  for(int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if(!anAttrSelection.get()) {
      theError = "Error: empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if(!aContext.get()) {
      theError = "Error: empty selection context.";
      return false;
    }
    FeaturePtr aFeature = ModelAPI_Feature::feature(aContext);
    if(!aFeature.get()) {
      theError = "Error: empty feature.";
      return false;
    }
    std::string aFeatureKind = aFeature->getKind();
    if(aFeatureKind == "Sketch" ||
       aFeatureKind == "Plane" ||
       aFeatureKind == "Axis") {
      theError = "Error: %1 shape is not allowed for selection.";
      theError.arg(aFeatureKind);
      return false;
    }
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    GeomShapePtr aContextShape = aContext->shape();
    if(!aShape.get()) {
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      theError = "Error: empty shape.";
      return false;
    }
    if(!aShape->isEqual(aContextShape)) {
      theError = "Error: Local selection not allowed.";
      return false;
    }

    int aShapeType = aShape->shapeType();
    // Allow to select edges, faces and solids.
    if(aShapeType != GeomAPI_Shape::EDGE &&
       aShapeType != GeomAPI_Shape::FACE &&
       aShapeType != GeomAPI_Shape::SOLID &&
       aShapeType != GeomAPI_Shape::COMPSOLID &&
       aShapeType != GeomAPI_Shape::COMPOUND) {
      theError = "Error: selected shape has the wrong type.";
      return false;
    }
  }

  return true;
}
