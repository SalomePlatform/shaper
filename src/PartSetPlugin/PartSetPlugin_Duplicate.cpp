// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSetPlugin_Duplicate.cxx
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_Duplicate.h"
#include "PartSetPlugin_Part.h"
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>

using namespace std;

void PartSetPlugin_Duplicate::execute()
{
  std::shared_ptr<ModelAPI_Session> aPManager = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aRoot = aPManager->moduleDocument();
  std::shared_ptr<ModelAPI_ResultPart> aSource;  // searching for source document attribute
  for (int a = aRoot->size(getGroup()) - 1; a >= 0; a--) {
    aSource = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aRoot->object(getGroup(), a));
    if (aSource && aSource->data()
        && aSource->data()->document(ModelAPI_ResultPart::DOC_REF())->value()
            == aPManager->activeDocument())
      break;
    aSource.reset();
  }
  if (aSource) {
    // create a new Part feature
    FeaturePtr aNewPart = aRoot->addFeature(PartSetPlugin_Part::ID(), false);
    aNewPart->execute(); // to make result and generate a unique name

    std::shared_ptr<ModelAPI_Document> aCopy = aPManager->copy(
      aSource->data()->document(ModelAPI_ResultPart::DOC_REF())->value(),
      std::dynamic_pointer_cast<ModelAPI_ResultPart>(aNewPart->firstResult())->partDoc()->id());
  }
}
