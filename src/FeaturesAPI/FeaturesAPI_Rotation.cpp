// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Rotation.cpp
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Rotation.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Rotation::FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Rotation::FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const std::list<ModelHighAPI_Selection>& theMainObjects,
                                           const ModelHighAPI_Selection& theAxisObject,
                                           const ModelHighAPI_Double& theAngle)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theAxisObject, myaxisObject);
    setAngle(theAngle);
  }
}

//==================================================================================================
FeaturesAPI_Rotation::~FeaturesAPI_Rotation()
{

}

//==================================================================================================
void FeaturesAPI_Rotation::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::setAxisObject(const ModelHighAPI_Selection& theAxisObject)
{
  fillAttribute(theAxisObject, myaxisObject);

  execute();
}

//==================================================================================================
void FeaturesAPI_Rotation::setAngle(const ModelHighAPI_Double& theAngle)
{
  fillAttribute(theAngle, myangle);

  execute();
}

//==================================================================================================
RotationPtr addRotation(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                        const ModelHighAPI_Selection& theAxisObject,
                        const ModelHighAPI_Double& theDistance)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Rotation::ID());
  return RotationPtr(new FeaturesAPI_Rotation(aFeature, theMainObjects, theAxisObject, theDistance));
}
