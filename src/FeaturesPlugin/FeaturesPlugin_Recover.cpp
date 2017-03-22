// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Recover.cpp
// Created:     29 Jul 2016
// Author:      Natalia ERMOLAEVA

#include "FeaturesPlugin_Recover.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>
#include <GeomAlgoAPI_Copy.h>

FeaturesPlugin_Recover::FeaturesPlugin_Recover()
{
}

void FeaturesPlugin_Recover::initAttributes()
{
  data()->addAttribute(BASE_FEATURE(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(RECOVERED_ENTITIES(), ModelAPI_AttributeRefList::typeId());

}

void FeaturesPlugin_Recover::execute()
{
  int aResultIndex = 0;
  AttributeRefListPtr aRecovered = reflist(RECOVERED_ENTITIES());
  for(int anIndex = aRecovered->size() - 1; anIndex >= 0; anIndex--) {
    ObjectPtr anObj = aRecovered->object(anIndex);
    if (!anObj.get())
      continue;
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (!aResult.get())
      continue;
    GeomShapePtr aShape = aResult->shape();
    if (!aShape.get())
      continue;

    // Copy shape.
    GeomAlgoAPI_Copy aCopyAlgo(aShape);
    // Check that algo is done.
    if(!aCopyAlgo.isDone()) {
      setError("Error: recover algorithm failed.");
      return;
    }
    // Check if shape is not null.
    if(!aCopyAlgo.shape().get() || aCopyAlgo.shape()->isNull()) {
      setError("Error: resulting shape is null.");
      return;
    }
    // Check that resulting shape is valid.
    if(!aCopyAlgo.isValid()) {
      setError("Error: resulting shape is not valid.");
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->store(aCopyAlgo.shape());//, aCopyAlgo.shape());
    std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = aCopyAlgo.mapOfSubShapes();
    // like in import: forget any history
    int aTag(1);
    std::string aNameMS = "Shape";
    aResultBody->loadFirstLevel(aCopyAlgo.shape(), aNameMS, aTag);

    setResult(aResultBody, aResultIndex);
    ++aResultIndex;
  }

  removeResults(aResultIndex);
}
