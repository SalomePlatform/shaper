// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "GeomValidators_ConstructionComposite.h"

#include "ModelAPI_AttributeSelection.h"
#include "ModelAPI_ResultConstruction.h"
#include "ModelAPI_CompositeFeature.h"

bool GeomValidators_ConstructionComposite::isValid(const AttributePtr& theAttribute,
                                                   const std::list<std::string>& theArguments,
                                                   std::string& theError) const
{
  bool aValid = false;
  AttributeSelectionPtr aSelectionAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                                                                (theAttribute);
  if (aSelectionAttr.get() == NULL) {
    theError = "Is not a selection attribute.";
    return aValid;
  }

  ResultPtr aResult = aSelectionAttr->context();
  GeomShapePtr aShape = aSelectionAttr->value();
  // global selection should be ignored, the filter processes only selected sub-shapes
  // that means, that the shape of the context result is equal to the shape value
  ///*ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theSelectedObject);
  if (aResult.get() != NULL) {
    GeomShapePtr aShapePtr = aResult->shape();
    // it is important to call isEqual of the shape of result.
    // It is a GeomAPI_Vertex shape for the point. The shape of the parameter is 
    // GeomAPI_Shape. It is important to use the realization of the isEqual method from
    // GeomAPI_Vertex class
    if (aShape.get()) {
      aValid = aShapePtr.get() != NULL && aShapePtr->isEqual(aShape);
    }
    else {
      // an empty shape is used in attribute selection if the shape of the result is equal to
      // the selected shape, so according to the upper condition, the result is true
      aValid = true;
    }
  }
  if (!aValid) {
    ResultConstructionPtr aConstr =
                            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aResult);
    if (aConstr != NULL) {
      // it provides selection only on composite features, construction without composite
      // feature is not selectable
      FeaturePtr aFeature = ModelAPI_Feature::feature(aConstr);
      CompositeFeaturePtr aComposite = 
                             std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
      aValid = aComposite && aComposite->numberOfSubs() > 0;
    }
    else {
      // non-construction results should be accepted by this filter, e.g. body results
      aValid = true;
    }
  }
  return aValid;
}
