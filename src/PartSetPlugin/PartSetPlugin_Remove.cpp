// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSetPlugin_Remove.cxx
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_Remove.h"
#include "PartSetPlugin_Part.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Events.h>

#include <Events_Loop.h>

void PartSetPlugin_Remove::execute()
{
  std::shared_ptr<ModelAPI_Session> aPManager = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aRoot = aPManager->moduleDocument();
  DocumentPtr aThisDoc = document();
  ResultPtr aPart = ModelAPI_Tools::findPartResult(aRoot, aThisDoc);
  if (aPart.get()) {
    FeaturePtr aFeature = aRoot->feature(aPart);
    if (aFeature) {
      // do remove
      aPart->data()->document(ModelAPI_ResultPart::DOC_REF())->value()->close();
      std::set<std::shared_ptr<ModelAPI_Feature> > aRefFeatures;
      aRoot->refsToFeature(aFeature, aRefFeatures);
      if (aRefFeatures.empty()) {
        aRoot->removeFeature(aFeature);
        // the redisplay signal should be flushed in order to erase the feature presentation in the viewer
        // after removeFeature from the document
        Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
      }
    }
  }
}
