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
#include "ModelAPI_Object.h"

#include <GeomAPI_Shape.h>

bool GeomValidators_DifferentShapes::isValid(const AttributePtr& theAttribute,
                                      const std::list<std::string>& theArguments,
                                      Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionPtr aSelectionAttribute =
                     std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  GeomShapePtr aShape = aSelectionAttribute->value();
  if (!aShape.get()) {
    ResultPtr aResult = aSelectionAttribute->context();
    if (aResult.get())
      aShape = aResult->shape();
  }

  std::string aCurrentAttributeId = theAttribute->id();
  // get all feature attributes
  std::list<AttributePtr> anAttrs =
      aFeature->data()->attributes(ModelAPI_AttributeSelection::typeId());
  if (anAttrs.size() > 0 && aShape.get() != NULL) {
    std::list<AttributePtr>::iterator anAttr = anAttrs.begin();
    for(; anAttr != anAttrs.end(); anAttr++) {
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
            theError = "The feature uses equal shapes.";
            return false;
          }
        }
      }
    }
  }
  return true;
}
