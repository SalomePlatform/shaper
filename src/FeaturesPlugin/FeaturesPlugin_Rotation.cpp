// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Rotation.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_Rotation.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>

//=================================================================================================
FeaturesPlugin_Rotation::FeaturesPlugin_Rotation()
{
}

//=================================================================================================
void FeaturesPlugin_Rotation::initAttributes()
{
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Rotation::LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // revolution works with faces always
  aSelection->setSelectionType("SOLID");

  data()->addAttribute(FeaturesPlugin_Rotation::AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Rotation::ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void FeaturesPlugin_Rotation::execute()
{
}
