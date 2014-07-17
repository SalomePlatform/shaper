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
  boost::shared_ptr<ModelAPI_PluginManager> aPManager = ModelAPI_PluginManager::get();
  boost::shared_ptr<ModelAPI_Document> aRoot = aPManager->rootDocument();
  boost::shared_ptr<ModelAPI_Document> aCurrent;
  boost::shared_ptr<PartSetPlugin_Part> a;
  for(int a = aRoot->size(ModelAPI_Feature::group()) - 1; a >= 0; a--) {
    FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(
      aRoot->object(ModelAPI_Feature::group(), a));
    if (aFeature->getKind() == PartSetPlugin_Part::ID()) {
      boost::shared_ptr<PartSetPlugin_Part> aPart = 
        boost::static_pointer_cast<PartSetPlugin_Part>(aFeature);
      if (aPart && aPart->firstResult() &&
          aPart->firstResult()->data()->docRef(ModelAPI_ResultPart::DOC_REF())->value() == 
            aPManager->currentDocument()) {
        // do remove
        aPart->firstResult()->data()->docRef(ModelAPI_ResultPart::DOC_REF())->value()->close();
        aRoot->removeFeature(aPart);
      }
    }
  }
}
