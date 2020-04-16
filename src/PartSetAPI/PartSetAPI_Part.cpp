// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "PartSetAPI_Part.h"
//--------------------------------------------------------------------------------------
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
//--------------------------------------------------------------------------------------
#include <PartSetPlugin_Duplicate.h>
#include <PartSetPlugin_Remove.h>
//--------------------------------------------------------------------------------------
PartSetAPI_Part::PartSetAPI_Part(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

PartSetAPI_Part::~PartSetAPI_Part()
{
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_Document> PartSetAPI_Part::document() const
{
  return std::dynamic_pointer_cast<ModelAPI_ResultPart>(defaultResult())->partDoc();
}

void PartSetAPI_Part::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addPart(" << aDocName << ")" << std::endl;
}

//--------------------------------------------------------------------------------------
PartPtr addPart(const std::shared_ptr<ModelAPI_Document> & thePart)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PartSetAPI_Part::ID());
  aFeature->execute();
  return PartPtr(new PartSetAPI_Part(aFeature));
}

PartPtr duplicatePart(const ModelHighAPI_Interface& thePart)
{
  PartPtr aResult;
  // only the active part is duplicated, so, activate it
  FeaturePtr aPartFeature = thePart.feature();
  if (aPartFeature) {
    // duplication of the current part is performed
    DocumentPtr aPartSet = ModelAPI_Session::get()->moduleDocument();
    aPartSet->setCurrentFeature(aPartFeature, false);
    DocumentPtr aPartDoc =
      std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->firstResult())->partDoc();
    if (aPartDoc.get()) {
      ModelAPI_Session::get()->setActiveDocument(aPartDoc);
      FeaturePtr aDuplicate = aPartDoc->addFeature(PartSetPlugin_Duplicate::ID());
      //aDuplicate->execute(); // it is executed at creation because it is action
      FeaturePtr aNewPart = aPartSet->currentFeature(false); // a copy becomes a current
      if (aNewPart.get()) {
        aResult = PartPtr(new PartSetAPI_Part(aNewPart));
        DocumentPtr aNewDoc = std::dynamic_pointer_cast<ModelAPI_ResultPart>(
          aNewPart->firstResult())->partDoc();
        ModelAPI_Session::get()->setActiveDocument(aNewDoc);
      }
    }
  }
  return aResult;
}

void removePart(const ModelHighAPI_Interface& thePart)
{
  FeaturePtr aPartFeature = thePart.feature();
  if (aPartFeature) {
    // duplication of the current part is performed
    DocumentPtr aPartSet = ModelAPI_Session::get()->moduleDocument();
    aPartSet->setCurrentFeature(aPartFeature, false);
    DocumentPtr aPartDoc =
      std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->firstResult())->partDoc();
    if (aPartDoc.get()) {
      ModelAPI_Session::get()->setActiveDocument(aPartDoc);
      aPartDoc->addFeature(PartSetPlugin_Remove::ID());
      ModelAPI_Session::get()->setActiveDocument(aPartSet);
    }
  }
}
