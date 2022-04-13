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

#include "FeaturesPlugin_SharedFaces.h"

#include <FeaturesPlugin_GroupSharedFaces.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <sstream>

//=================================================================================================
FeaturesPlugin_SharedFaces::FeaturesPlugin_SharedFaces()
{
}

//=================================================================================================
void FeaturesPlugin_SharedFaces::initAttributes()
{
  data()->addAttribute(OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(LIST_FACES_ID(), ModelAPI_AttributeSelectionList::typeId());

  data()->addAttribute(NUMBER_FACES_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(TRANSPARENCY_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(CREATE_GROUP_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(GROUP_NAME_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(COMPUTE_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), GROUP_NAME_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), COMPUTE_ID());
  data()->boolean(COMPUTE_ID())->setValue(true);
}

//=================================================================================================
AttributePtr FeaturesPlugin_SharedFaces::attributIsCompute()
{
  return attribute(COMPUTE_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_SharedFaces::attributObject()
{
  return attribute(OBJECT_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_SharedFaces::attributListFaces()
{
  return attribute(LIST_FACES_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_SharedFaces::attributNumberFaces()
{
  return attribute(NUMBER_FACES_ID());
}

//=================================================================================================
void FeaturesPlugin_SharedFaces::execute()
{
  if (boolean(CREATE_GROUP_ID())->value()) {

    if (string(GROUP_NAME_ID())->value() != ""
      && selectionList(LIST_FACES_ID())->isInitialized()) {

      if (lastResult().get()) {
        eraseResultFromList(lastResult());
      }

      if (!myCreateGroupFeature.get())
        createGroup();
      updateGroup();
    }

  } else {
    if (selectionList(LIST_FACES_ID())->isInitialized()) {

      if (myCreateGroupFeature.get()) {
        myCreateGroupFeature->eraseResults();
        SessionPtr aSession = ModelAPI_Session::get();
        DocumentPtr aDoc =  aSession->activeDocument();
        aDoc->removeFeature(myCreateGroupFeature);
        myCreateGroupFeature.reset();
      }

      if (lastResult().get())
        eraseResultFromList(lastResult());
      setFacesGroup(L"Group_SharedFaces");
    }
  }

  if (selection(OBJECT_ID())->isInitialized()) {
    AttributeSelectionPtr aCompSolidAttr = selection(OBJECT_ID());
    ResultPtr aResult = aCompSolidAttr->context();

    double aTranparency = integer(TRANSPARENCY_ID())->value()/100.0;
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
void FeaturesPlugin_SharedFaces::attributeChanged(const std::string& theID)
{
  if (theID == OBJECT_ID()) {
    updateFaces();
    if (myCreateGroupFeature.get())
      updateGroup();
  }
}


//=================================================================================================
void FeaturesPlugin_SharedFaces::createGroup()
{
  SessionPtr aSession = ModelAPI_Session::get();

  DocumentPtr aDoc =  aSession->activeDocument();

  if (aDoc.get()) {
    myCreateGroupFeature = aDoc->addFeature(FeaturesPlugin_GroupSharedFaces::ID());
  }
}

//=================================================================================================
void FeaturesPlugin_SharedFaces::updateGroup()
{
    myCreateGroupFeature->boolean(FeaturesPlugin_GroupSharedFaces::COMPUTE_ID())->setValue(false);
    myCreateGroupFeature->string(FeaturesPlugin_GroupSharedFaces::GROUP_NAME_ID())
      ->setValue(string(GROUP_NAME_ID())->value());

    myCreateGroupFeature->selection(FeaturesPlugin_GroupSharedFaces::OBJECT_ID())
      ->setValue(selection(OBJECT_ID())->context(), selection(OBJECT_ID())->value());
    AttributeSelectionListPtr aFacesFeatures =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>
                               (myCreateGroupFeature->attribute(LIST_FACES_ID()));

    AttributeSelectionListPtr aFaces =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(attribute(LIST_FACES_ID()));
    aFaces->copyTo(aFacesFeatures);

    myCreateGroupFeature->integer(FeaturesPlugin_GroupSharedFaces::TRANSPARENCY_ID())
                          ->setValue( integer(TRANSPARENCY_ID())->value());
    myCreateGroupFeature->execute();
    myCreateGroupFeature->boolean(FeaturesPlugin_GroupSharedFaces::COMPUTE_ID())->setValue(true);
}
