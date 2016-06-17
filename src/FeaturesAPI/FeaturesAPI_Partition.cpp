// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Partition.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Partition.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Partition::FeaturesAPI_Partition(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Partition::FeaturesAPI_Partition(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBaseObjects);
  }
}

//==================================================================================================
FeaturesAPI_Partition::~FeaturesAPI_Partition()
{

}

//==================================================================================================
void FeaturesAPI_Partition::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
PartitionPtr addPartition(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Partition::ID());
  return PartitionPtr(new FeaturesAPI_Partition(aFeature, theBaseObjects));
}
