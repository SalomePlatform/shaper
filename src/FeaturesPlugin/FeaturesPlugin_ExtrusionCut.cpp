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

//=================================================================================================
FeaturesPlugin_ExtrusionCut::FeaturesPlugin_ExtrusionCut()
{
}

//=================================================================================================
void FeaturesPlugin_ExtrusionCut::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_ExtrusionCut::SKETCH_OBJECT_ID(), ModelAPI_AttributeReference::typeId());

  data()->addAttribute(FeaturesPlugin_ExtrusionCut::TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_ExtrusionCut::FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_ExtrusionCut::AXIS_OBJECT_ID(), ModelAPI_AttributeReference::typeId());

  data()->addAttribute(FeaturesPlugin_ExtrusionCut::FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_ExtrusionCut::TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FeaturesPlugin_ExtrusionCut::FROM_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FeaturesPlugin_ExtrusionCut::TO_OBJECT_ID());

  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_ExtrusionCut::LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("SOLID");
}

//=================================================================================================
void FeaturesPlugin_ExtrusionCut::execute()
{
}
