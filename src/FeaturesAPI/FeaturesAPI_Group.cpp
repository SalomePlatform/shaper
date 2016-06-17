// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Group.cpp
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Group.h"

#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Group::FeaturesAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Group::FeaturesAPI_Group(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theGroupList)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setGroupList(theGroupList);
  }
}

//==================================================================================================
FeaturesAPI_Group::~FeaturesAPI_Group()
{

}

//==================================================================================================
void FeaturesAPI_Group::setGroupList(const std::list<ModelHighAPI_Selection>& theGroupList)
{
  fillAttribute(theGroupList, mygroupList);

  execute();
}

//==================================================================================================
GroupPtr addGroup(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theGroupList)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Group::ID());
  return GroupPtr(new FeaturesAPI_Group(aFeature, theGroupList));
}
