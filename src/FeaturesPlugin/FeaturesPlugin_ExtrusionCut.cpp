// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionCut.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_ExtrusionCut.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeRefList.h>

//=================================================================================================
FeaturesPlugin_ExtrusionCut::FeaturesPlugin_ExtrusionCut()
{
}

//=================================================================================================
void FeaturesPlugin_ExtrusionCut::initAttributes()
{

  data()->addAttribute(SKETCH_OBJECT_ID(), ModelAPI_AttributeReference::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FeaturesPlugin_ExtrusionCut::FROM_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FeaturesPlugin_ExtrusionCut::TO_OBJECT_ID());

  data()->addAttribute(CUTLIST_ID(), ModelAPI_AttributeSelectionList::typeId());

  // extrusion works with faces always
  AttributeSelectionListPtr aSelection = data()->selectionList(CUTLIST_ID());
  aSelection->setSelectionType("SOLID");
}


std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_ExtrusionCut::addFeature(std::string theID)
{
  std::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID, false);
  if (aNew) {
    data()->reference(SKETCH_OBJECT_ID())->setValue(aNew);
  }
   // set as current also after it becomes sub to set correctly enabled for other sketch subs
  //document()->setCurrentFeature(aNew, false);
  return aNew;
}

  
int FeaturesPlugin_ExtrusionCut::numberOfSubs() const
{
  ObjectPtr aObj = data()->reference(SKETCH_OBJECT_ID())->value();
  return aObj.get()? 1 : 0;
}

std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_ExtrusionCut::subFeature(const int theIndex) const
{
  if (theIndex == 0)
    return std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_OBJECT_ID())->value());
  return std::shared_ptr<ModelAPI_Feature>();
}

int FeaturesPlugin_ExtrusionCut::subFeatureId(const int theIndex) const
{
  std::shared_ptr<ModelAPI_Feature> aFeature = subFeature(theIndex);
  if (aFeature.get())
    return aFeature->data()->featureId();
  return -1;
}

bool FeaturesPlugin_ExtrusionCut::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature)
    return false;
 
  ObjectPtr aSub = data()->reference(SKETCH_OBJECT_ID())->value();
  return aSub == theObject;
}

void FeaturesPlugin_ExtrusionCut::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
}

//=================================================================================================
void FeaturesPlugin_ExtrusionCut::execute()
{
}
