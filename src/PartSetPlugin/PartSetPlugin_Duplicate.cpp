// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSetPlugin_Duplicate.cxx
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_Duplicate.h"
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>

using namespace std;

PartSetPlugin_Duplicate::PartSetPlugin_Duplicate()
{
}

void PartSetPlugin_Duplicate::initAttributes()
{
  PartSetPlugin_Part::initAttributes();

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
    std::shared_ptr<ModelAPI_Document> aCopy = aPManager->copy(
        aSource->data()->document(ModelAPI_ResultPart::DOC_REF())->value(), data()->name());
  } else { // invalid part copy: do nothing, keep this in empty name
    data()->setName("");
  }
}

void PartSetPlugin_Duplicate::execute()
{
  if (!data()->name().empty())
    PartSetPlugin_Part::execute();
}
