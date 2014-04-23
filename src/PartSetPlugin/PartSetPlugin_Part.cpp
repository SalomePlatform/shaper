// File:        PartSetPlugin_Part.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_Part.h"
#include "ModelAPI_PluginManager.h"
#include "ModelAPI_Document.h"
#include "ModelAPI_Data.h"
#include "ModelAPI_Iterator.h"
#include "ModelAPI_AttributeDocRef.h"

using namespace std;

PartSetPlugin_Part::PartSetPlugin_Part()
{
}

void PartSetPlugin_Part::initAttributes()
{
  data()->addAttribute(PART_ATTR_DOC_REF, ModelAPI_AttributeDocRef::type());
}

void PartSetPlugin_Part::execute() 
{
  shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->docRef(PART_ATTR_DOC_REF);
  if (!aDocRef->value()) { // create a document if not yet created
    shared_ptr<ModelAPI_Document> aPartSetDoc = ModelAPI_PluginManager::get()->rootDocument();
    aDocRef->setValue(aPartSetDoc->subDocument(data()->getName()));
  }
}

shared_ptr<ModelAPI_Document> PartSetPlugin_Part::documentToAdd() {
  return ModelAPI_PluginManager::get()->rootDocument();
}
