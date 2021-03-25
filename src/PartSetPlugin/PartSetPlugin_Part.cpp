// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "PartSetPlugin_Part.h"
#include "ModelAPI_Session.h"
#include "ModelAPI_Document.h"
#include "ModelAPI_Data.h"
#include "ModelAPI_AttributeDocRef.h"
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Events.h>

PartSetPlugin_Part::PartSetPlugin_Part()
{
}

void PartSetPlugin_Part::initAttributes()
{  // all is in part result
}

void PartSetPlugin_Part::execute()
{
  ResultPartPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(firstResult());
  if (!aResult) {
    aResult = document()->createPart(data());
    setResult(aResult);
    // do not activate part by simple execution if it is not loaded yet: it must be explicitly
    // activated for this
    std::shared_ptr<ModelAPI_AttributeDocRef> aDocRef =
      aResult->data()->document(ModelAPI_ResultPart::DOC_REF());
    if (!ModelAPI_Session::get()->isLoadByDemand(aResult->data()->name(), aDocRef->docId())) {
      // On undo/redo creation of the part result the Object Browser must get creation event
      // earlier that activation of this part event (otherwise the crash is produced)
      // So, send a creation event earlier, without any grouping
      static Events_ID aCreateID = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
      ModelAPI_EventCreator::get()->sendUpdated(aResult, aCreateID, false);

      aResult->activate();
    }
  } else { // execute is called for result update anyway
    aResult->updateShape();
  }
}

const std::string& PartSetPlugin_Part::documentToAdd()
{
  // part must be added only to the module document
  return ModelAPI_Session::get()->moduleDocument()->kind();
}

std::shared_ptr<ModelAPI_Feature> PartSetPlugin_Part::addFeature(std::string theID)
{
  ResultPartPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(firstResult());
  if (aResult.get()) {
    DocumentPtr aDoc = aResult->partDoc();
    if (aDoc.get() && aDoc->isOpened())
      return aDoc->addFeature(theID);
  }
  return FeaturePtr();
}

int PartSetPlugin_Part::numberOfSubs(bool /*forTree*/) const
{
  ResultPartPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(firstResult());
  if (aResult.get()) {
    DocumentPtr aDoc = aResult->partDoc();
    if (aDoc.get() && aDoc->isOpened())
      return aDoc->numInternalFeatures();
  }
  return 0;
}

std::shared_ptr<ModelAPI_Feature> PartSetPlugin_Part::subFeature(const int theIndex,
                                                                 bool /*forTree*/)
{
  ResultPartPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(firstResult());
  if (aResult.get()) {
    DocumentPtr aDoc = aResult->partDoc();
    if (aDoc.get() && aDoc->isOpened()) {
      return aDoc->internalFeature(theIndex);
    }
  }
  return FeaturePtr();
}
//LCOV_EXCL_START
int PartSetPlugin_Part::subFeatureId(const int theIndex) const
{
  ResultPartPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(firstResult());
  if (aResult.get()) {
    DocumentPtr aDoc = aResult->partDoc();
    if (aDoc.get() && aDoc->isOpened()) {
      return aDoc->object(ModelAPI_Feature::group(), theIndex)->data()->featureId();
    }
  }
  return 0; // none
}
//LCOV_EXCL_STOP
bool PartSetPlugin_Part::isSub(ObjectPtr theObject) const
{
  ResultPartPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(firstResult());
  if (aResult.get()) {
    DocumentPtr aDoc = aResult->partDoc();
    return theObject->document() == aDoc;
  }
  return false;
}

void PartSetPlugin_Part::erase() {
  ResultPartPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(firstResult());
  if (aResult.get()) {
    DocumentPtr aDoc = aResult->partDoc();
    if (aDoc.get())
      aDoc->eraseAllFeatures();
  }
  ModelAPI_Feature::erase();
}
