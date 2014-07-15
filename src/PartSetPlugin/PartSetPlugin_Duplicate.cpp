// File:        PartSetPlugin_Duplicate.cxx
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_Duplicate.h"
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>

using namespace std;

PartSetPlugin_Duplicate::PartSetPlugin_Duplicate()
{
}

void PartSetPlugin_Duplicate::initAttributes()
{
  PartSetPlugin_Part::initAttributes();
  data()->addAttribute(PartSetPlugin_Duplicate::DOC_REF(), ModelAPI_AttributeRefAttr::type());

  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = data()->refattr(PartSetPlugin_Duplicate::DOC_REF());
  if (!aRef->attr()) { // create a copy: if not created yet attribute is not initialized
    boost::shared_ptr<ModelAPI_PluginManager> aPManager = ModelAPI_PluginManager::get();
    boost::shared_ptr<ModelAPI_Document> aRoot = aPManager->rootDocument();
    boost::shared_ptr<PartSetPlugin_Part> aSource; // searching for source document attribute
    for(int a = aRoot->size(getGroup()) - 1; a >= 0; a--) {
      aSource = boost::dynamic_pointer_cast<PartSetPlugin_Part>(
        aRoot->feature(getGroup(), a, true));
      if (aSource->data()->docRef(PartSetPlugin_Part::DOC_REF())->value() == aPManager->currentDocument())
        break;
      aSource.reset();
    }
    if (aSource) {
      boost::shared_ptr<ModelAPI_Document> aCopy = 
        aPManager->copy(aSource->data()->docRef(PartSetPlugin_Part::DOC_REF())->value(), data()->getName());
      aRef->setFeature(aSource);
    }
  }
}
