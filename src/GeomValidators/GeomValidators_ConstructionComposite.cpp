// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "GeomValidators_ConstructionComposite.h"

#include <Events_InfoMessage.h>

#include "ModelAPI_AttributeSelection.h"
#include "ModelAPI_ResultConstruction.h"
#include "ModelAPI_CompositeFeature.h"

bool GeomValidators_ConstructionComposite::isValid(const AttributePtr& theAttribute,
                                                   const std::list<std::string>& theArguments,
                                                   Events_InfoMessage& theError) const
{
  bool aValid = true;
  if (theAttribute->attributeType() != ModelAPI_AttributeSelection::typeId()) {
// LCOV_EXCL_START
    aValid = false;
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return aValid;
// LCOV_EXCL_STOP
  }

  AttributeSelectionPtr aSelectionAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                                                                (theAttribute);
  ResultPtr aResult = aSelectionAttr->context();
  // global selection should be ignored, the filter processes only selected sub-shapes
  // that means, that the shape of the context result is equal to the shape value
  ///*ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theSelectedObject);
  if (!aResult.get()) {
    aValid = false;
    theError = "The result is empty";
  }
  else {
    GeomShapePtr aShape = aSelectionAttr->value();
    GeomShapePtr aShapePtr = aResult->shape();
    // it is important to call isEqual of the shape of result.
    // It is a GeomAPI_Vertex shape for the point. The shape of the parameter is
    // GeomAPI_Shape. It is important to use the realization of the isEqual method from
    // GeomAPI_Vertex class
    if (!aShape.get()) {
      // an empty shape is used in attribute selection if the shape of the result is equal to
      // the selected shape, so according to the upper condition, the result is true
      aValid = true;
    }
    else {
      if (aShapePtr->isEqual(aShape)) {
        aValid = true;
      }
      else {
        ResultConstructionPtr aConstr =
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aResult);
        if (!aConstr.get()) {
          // non-construction results should be accepted by this filter, e.g. body results
          aValid = true;
        }
        else {
          // it provides selection only on composite features, construction without composite
          // feature is not selectable.
          FeaturePtr aFeature = ModelAPI_Feature::feature(aConstr);
          CompositeFeaturePtr aComposite =
            std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
          aValid = aComposite && aComposite->numberOfSubs() > 0;
          if (!aValid)
            theError = "Uses composite construction feature without sub-features.";
        }
      }
    }
  }
  return aValid;
}
