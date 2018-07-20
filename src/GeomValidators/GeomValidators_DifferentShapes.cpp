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

#include "GeomValidators_DifferentShapes.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include "ModelAPI_Object.h"

#include <GeomAPI_Shape.h>
#include <GeomAPI_Tools.h>

//=================================================================================================
bool GeomValidators_DifferentShapes::isValid(const AttributePtr& theAttribute,
                                      const std::list<std::string>& theArguments,
                                      Events_InfoMessage& theError) const
{
  bool isValid = false;

  std::string anAttributeType = theAttribute->attributeType();
  bool isList = anAttributeType == ModelAPI_AttributeSelectionList::typeId();

  std::list<AttributePtr> anAttrs;
  if (isList) {
    AttributeSelectionListPtr aListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    // get all selection attributes from the list
    for (int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
      anAttrs.push_back(aListAttr->value(anIndex));
    }

    isValid = checkEquals(anAttrs);
  }
  else {
    // get all feature selection attributes
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
    anAttrs = aFeature->data()->attributes(ModelAPI_AttributeSelection::typeId());

    isValid = checkEqualToCurrent(anAttrs, theAttribute);
  }

  if (!isValid) {
    theError = isList ? "The selection list contains equal shapes." :
                        "The feature uses equal shapes.";
    return false;
  }

  return true;
}

//=================================================================================================
bool GeomValidators_DifferentShapes::checkEquals(std::list<AttributePtr>& theAttributes)
{
  std::list<AttributePtr>::iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); anIt++) {
    AttributePtr anAttribute = *anIt;

    std::list<AttributePtr>::iterator anOthersIt = std::next(anIt);
    for (; anOthersIt != theAttributes.end(); anOthersIt++) {
      AttributePtr anOtherAttribute = *anOthersIt;
      if (isAttrShapesEqual(anAttribute, anOtherAttribute)) {
        return false;
      }
    }
  }

  return true;
}

//=================================================================================================
bool GeomValidators_DifferentShapes::checkEqualToCurrent(std::list<AttributePtr>& theAttributes,
  const AttributePtr& theCurrentAttribute)
{
  AttributeSelectionPtr aSelectionAttribute =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theCurrentAttribute);

  GeomShapePtr aShape = aSelectionAttribute->value();
  if (!aShape.get()) {
    ResultPtr aResult = aSelectionAttribute->context();
    if (aResult.get())
      aShape = aResult->shape();
  }

  std::string aCurrentAttributeId = theCurrentAttribute->id();
  if (theAttributes.size() > 0 && aShape.get() != NULL) {
    std::list<AttributePtr>::iterator anAttr = theAttributes.begin();
    for (; anAttr != theAttributes.end(); anAttr++) {
      AttributePtr anAttribute = *anAttr;
      // take into concideration only other attributes
      if (anAttribute.get() != NULL && anAttribute->id() != aCurrentAttributeId) {
        aSelectionAttribute =
          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(anAttribute);
        // the shape of the attribute should be not the same
        if (aSelectionAttribute.get() != NULL) {
          GeomShapePtr anAttrShape = aSelectionAttribute->value();
          if (!anAttrShape.get()) {
            ResultPtr aResult = aSelectionAttribute->context();
            if (aResult.get())
              anAttrShape = aResult->shape();
          }
          if (aShape->isEqual(anAttrShape)) {
            return false;
          }
        }
      }
    }
  }

  return true;
}

bool GeomValidators_DifferentShapes::isAttrShapesEqual(const AttributePtr& theAttribute,
                                                       const AttributePtr& theOtherAttribute)
{
  AttributeSelectionPtr aSelectionAttribute =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anOtherSelectionAttribute =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theOtherAttribute);

  GeomShapePtr aShape = aSelectionAttribute->value();
  if (!aShape.get()) {
    ResultPtr aResult = aSelectionAttribute->context();
    if (aResult.get())
      aShape = aResult->shape();
  }
  GeomShapePtr aTypedShape = GeomAPI_Tools::getTypedShape(aShape);

  GeomShapePtr anOtherShape = anOtherSelectionAttribute->value();
  if (!anOtherShape.get()) {
    ResultPtr aResult = anOtherSelectionAttribute->context();
    if (aResult.get())
      anOtherShape = aResult->shape();
  }
  GeomShapePtr aOtherTypedShape = GeomAPI_Tools::getTypedShape(anOtherShape);

  return aTypedShape->isEqual(aOtherTypedShape);
}