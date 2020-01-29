// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_ImportResult.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultPart.h>
#include <Events_InfoMessage.h>

void FeaturesPlugin_ImportResult::initAttributes()
{
  data()->addAttribute(OBJECTS(), ModelAPI_AttributeSelectionList::typeId());
}

void FeaturesPlugin_ImportResult::execute()
{
  AttributeSelectionListPtr aList = selectionList(OBJECTS());
  int aResultIndex = 0;
  for (int aSelIndex = 0; aSelIndex < aList->size(); aSelIndex++) {
    AttributeSelectionPtr aSel = aList->value(aSelIndex);
    ResultPtr aContext = aSel->context();
    if (!aContext.get())
      continue;
    GeomShapePtr aShape = aContext->shape();
    if (!aShape.get() || aShape->isNull())
      continue;
    std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->store(aShape);
    aResultBody->loadFirstLevel(aShape, "ImportResult");
    setResult(aResultBody, aResultIndex++);
  }
  removeResults(aResultIndex);
}

bool FeaturesPlugin_ValidatorImportResults::isValid(const AttributePtr& theAttribute,
  const std::list<std::string>&, Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr aList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (aList->size() == 0) {
    // LCOV_EXCL_START
    theError = "Please select sources.";
    return false;
    // LCOV_EXCL_STOP
  }
  // store documents in the Part-fesatures in order to check
  // the selection is located in the previous documents
  std::map<DocumentPtr, int> aDocIndices;
  DocumentPtr aRoot = ModelAPI_Session::get()->moduleDocument();
  int aNum = aRoot->size(ModelAPI_Feature::group());
  for (int a = 0; a < aNum; a++) {
    FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(
      aRoot->object(ModelAPI_Feature::group(), a));
    if (aFeat.get() && aFeat->data() && aFeat->data()->isValid() && aFeat->getKind() == "Part" &&
      aFeat->results().size()) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aFeat->firstResult());
      if (aPart.get() && aPart->partDoc()) {
        aDocIndices[aPart->partDoc()] = a;
      }
    }
  }
  int aThisIndex = aDocIndices[aList->owner()->document()];
  for (int aSelIndex = 0; aSelIndex < aList->size(); aSelIndex++) {
    AttributeSelectionPtr aSel = aList->value(aSelIndex);
    ResultPtr aContext = aSel->context();
    if (!aContext.get()) {
      // LCOV_EXCL_START
      theError = "Empty context.";
      return false;
      // LCOV_EXCL_STOP
    }
    GeomShapePtr aShape = aSel->value();
    if (aShape.get() && !aShape->isNull() && !aShape->isSame(aContext->shape())) {
      // LCOV_EXCL_START
      theError = "Import results does not support selection of sub-shapes";
      return false;
      // LCOV_EXCL_STOP
    }
    ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContext);
    if (!aBody.get()) {
      // LCOV_EXCL_START
      theError = "Only results may be selected.";
      return false;
      // LCOV_EXCL_STOP
    }
    if (!aBody->shape()) {
      // LCOV_EXCL_START
      theError = "Result is empty.";
      return false;
      // LCOV_EXCL_STOP
    }
    int aBodyIndex = aDocIndices[aBody->document()];
    if (aBodyIndex >= aThisIndex) {
      theError = "The selected result must be located in earlier part.";
      return false;
    }
  }
  return true;
}
