// File:        PartSetPlugin_NewPart.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_NewPart.h"
#include "ModelAPI_PluginManager.h"
#include "ModelAPI_Document.h"
#include "ModelAPI_Object.h"
#include "ModelAPI_AttributeDocRef.h"

using namespace std;

PartSetPlugin_NewPart::PartSetPlugin_NewPart()
{
}

void PartSetPlugin_NewPart::initAttributes()
{
  data()->addAttribute(ModelAPI_AttributeDocRef::type(), PART_DOC_REF);
}

void PartSetPlugin_NewPart::execute() 
{
  shared_ptr<ModelAPI_Document> aPartSetDoc = ModelAPI_PluginManager::get()->rootDocument();
  data()->setName(string("Part_") + "1");
  aPartSetDoc->subDocument(string("Part_") + "1");
}
