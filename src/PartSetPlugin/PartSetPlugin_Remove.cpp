// File:        PartSetPlugin_Remove.cxx
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_Remove.h"
#include "PartSetPlugin_Part.h"
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_ResultPart.h>

void PartSetPlugin_Remove::execute()
{
  boost::shared_ptr<ModelAPI_Session> aPManager = ModelAPI_Session::get();
  boost::shared_ptr<ModelAPI_Document> aRoot = aPManager->rootDocument();
  boost::shared_ptr<ModelAPI_Document> aCurrent;
  boost::shared_ptr<PartSetPlugin_Part> a;
  for (int a = aRoot->size(ModelAPI_ResultPart::group()) - 1; a >= 0; a--) {
    ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(
        aRoot->object(ModelAPI_ResultPart::group(), a));
    if (aPart
        && aPart->data()->docRef(ModelAPI_ResultPart::DOC_REF())->value()
            == aPManager->currentDocument()) {
      FeaturePtr aFeature = aRoot->feature(aPart);
      if (aFeature) {
        // do remove
        aPart->data()->docRef(ModelAPI_ResultPart::DOC_REF())->value()->close();
        aRoot->removeFeature(aFeature);
      }
    }
  }
}
