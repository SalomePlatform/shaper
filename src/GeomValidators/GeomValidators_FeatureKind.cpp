// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_FeatureKind.cpp
// Created:     22 March 2015
// Author:      Natalia Ermolaeva

#include "GeomValidators_FeatureKind.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Object.h>

#define DEBUG_EXTRUSION_INVALID_SKETCH
#ifdef DEBUG_EXTRUSION_INVALID_SKETCH
  #include <ModelAPI_CompositeFeature.h>
#endif

bool GeomValidators_FeatureKind::isValid(const AttributePtr& theAttribute,
                                      const std::list<std::string>& theArguments,
                                      std::string& theError) const
{
  bool isSketchEntities = true;
  std::set<std::string> anEntityKinds;
  std::string anEntityKindsStr;
  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();
  for (; anIt != aLast; anIt++) {
    anEntityKinds.insert(*anIt);
    if (!anEntityKindsStr.empty())
      anEntityKindsStr += ", ";
    anEntityKindsStr += *anIt;
  }

  std::string anAttributeType = theAttribute->attributeType();
  if (anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = 
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    // all context objects should be sketch entities
    for (int i = 0, aSize = aSelectionListAttr->size(); i < aSize && isSketchEntities; i++) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
      ObjectPtr anObject = aSelectAttr->context();
      // a context of the selection attribute is a feature result. It can be a case when the result
      // of the feature is null, e.g. the feature is modified and has not been executed yet.
      // The validator returns an invalid result here. The case is an extrusion built on a sketch
      // feature. A new sketch element creation leads to an empty result.
      if (!anObject.get())
        isSketchEntities = false;
      else {
        FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
        isSketchEntities = anEntityKinds.find(aFeature->getKind()) != anEntityKinds.end();
#ifdef DEBUG_EXTRUSION_INVALID_SKETCH
        CompositeFeaturePtr aComp = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
        if (aComp.get() && aComp->numberOfSubs() == 1)
          return false;
#endif
      }
    }
  }
  if (anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectAttr = 
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ObjectPtr anObject = aSelectAttr->context();
    // a context of the selection attribute is a feature result. It can be a case when the result
    // of the feature is null, e.g. the feature is modified and has not been executed yet.
    // The validator returns an invalid result here. The case is an extrusion built on a sketch
    // feature. A new sketch element creation leads to an empty result.
    if (!anObject.get())
      isSketchEntities = false;
    else {
      FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
      isSketchEntities = anEntityKinds.find(aFeature->getKind()) != anEntityKinds.end();
    }
  }
  if (anAttributeType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
    // all context objects should be sketch entities
    for (int i = 0, aSize = aRefListAttr->size(); i < aSize && isSketchEntities; i++) {
      ObjectPtr anObject = aRefListAttr->object(i);
      // a context of the selection attribute is a feature result. It can be a case when the result
      // of the feature is null, e.g. the feature is modified and has not been executed yet.
      // The validator returns an invalid result here. The case is an extrusion built on a sketch
      // feature. A new sketch element creation leads to an empty result.
      if (!anObject.get())
        isSketchEntities = false;
      else {
        FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
        isSketchEntities = anEntityKinds.find(aFeature->getKind()) != anEntityKinds.end();
      }
    }
  }
  if (anAttributeType == ModelAPI_AttributeRefAttr::typeId()) {
    std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
                     std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    isSketchEntities = false;
    if (aRef->isObject()) {
      ObjectPtr anObject = aRef->object();
      if (anObject.get() != NULL) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
        if (aFeature.get() != NULL)
          isSketchEntities = anEntityKinds.find(aFeature->getKind()) != anEntityKinds.end();
      }
    }
  }
  if (anAttributeType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr aRefAttr = 
                      std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    ObjectPtr anObject = aRefAttr->value();
    // a context of the selection attribute is a feature result. It can be a case when the result
    // of the feature is null, e.g. the feature is modified and has not been executed yet.
    // The validator returns an invalid result here. The case is an extrusion built on a sketch
    // feature. A new sketch element creation leads to an empty result.
    if (!anObject.get())
      isSketchEntities = false;
    else {
      FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
      isSketchEntities = anEntityKinds.find(aFeature->getKind()) != anEntityKinds.end();
    }
  }
  if (!isSketchEntities) {
    theError = "It refers to feature, which kind is not in the list: " + anEntityKindsStr;
  }

  return isSketchEntities;
}
