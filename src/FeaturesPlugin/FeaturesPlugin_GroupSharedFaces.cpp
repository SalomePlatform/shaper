// Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_GroupSharedFaces.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <Locale_Convert.h>

#include <sstream>

//=================================================================================================
FeaturesPlugin_GroupSharedFaces::FeaturesPlugin_GroupSharedFaces()
{
}

//=================================================================================================
void FeaturesPlugin_GroupSharedFaces::initAttributes()
{
  data()->addAttribute(OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(LIST_FACES_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(NUMBER_FACES_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(TRANSPARENCY_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(GROUP_NAME_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(COMPUTE_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TRANSPARENCY_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), COMPUTE_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), NUMBER_FACES_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), LIST_FACES_ID());

  data()->boolean(COMPUTE_ID())->setValue(true);
}

//=================================================================================================
AttributePtr FeaturesPlugin_GroupSharedFaces::attributObject()
{
  return attribute(OBJECT_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_GroupSharedFaces::attributIsCompute()
{
  return attribute(COMPUTE_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_GroupSharedFaces::attributListFaces()
{
  return attribute(LIST_FACES_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_GroupSharedFaces::attributNumberFaces()
{
  return attribute(NUMBER_FACES_ID());
}

//=================================================================================================
void FeaturesPlugin_GroupSharedFaces::execute()
{
  if (selectionList(LIST_FACES_ID())->isInitialized()
    && string(GROUP_NAME_ID())->value() != "") {
    AttributeStringPtr aNameAtt = string(GROUP_NAME_ID()) ;
    std::wstring aNameFace = aNameAtt->isUValue() ?
                             Locale::Convert::toWString(aNameAtt->valueU()) :
                             Locale::Convert::toWString(aNameAtt->value());

    if (lastResult().get())
      eraseResultFromList(lastResult());
    setFacesGroup(aNameFace);
  } else {
    if (lastResult().get()) {
      eraseResultFromList(lastResult());
    }
  }
  if (selection(OBJECT_ID())->isInitialized() && integer(TRANSPARENCY_ID())->isInitialized()) {
    AttributeSelectionPtr aCompSolidAttr = selection(OBJECT_ID());
    ResultPtr aResult = aCompSolidAttr->context();

    double aTranparency = integer(TRANSPARENCY_ID())->value() / 100.0;
    ModelAPI_Tools::setTransparency(aResult, aTranparency);

    ResultBodyPtr aResultBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
    std::list<ResultPtr> allRes;
    ModelAPI_Tools::allSubs(aResultBody, allRes);
    std::list<ResultPtr>::iterator aRes;
    for(aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
      ModelAPI_Tools::setTransparency(*aRes, aTranparency);
    }
  }

  // Update the number of shared faces
  AttributeSelectionListPtr aFacesListAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList> (attributListFaces());
  AttributeStringPtr aNumberFacesAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeString> (attributNumberFaces());
  std::stringstream aLabel;
  aLabel << aFacesListAttr->size();
  aNumberFacesAttr->setValue(aLabel.str());
}

//=================================================================================================
void FeaturesPlugin_GroupSharedFaces::attributeChanged(const std::string& theID)
{
  if (theID == OBJECT_ID() || theID == LIST_FACES_ID())
    updateFaces();
}
