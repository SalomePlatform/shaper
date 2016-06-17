// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Translation.cpp
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Translation.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Translation::FeaturesAPI_Translation(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Translation::FeaturesAPI_Translation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                 const std::list<ModelHighAPI_Selection>& theMainObjects,
                                                 const ModelHighAPI_Selection& theAxisObject,
                                                 const ModelHighAPI_Double& theDistance)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theAxisObject, myaxisObject);
    setDistance(theDistance);
  }
}

//==================================================================================================
FeaturesAPI_Translation::~FeaturesAPI_Translation()
{

}

//==================================================================================================
void FeaturesAPI_Translation::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Translation::setAxisObject(const ModelHighAPI_Selection& theAxisObject)
{
  fillAttribute(theAxisObject, myaxisObject);

  execute();
}

//==================================================================================================
void FeaturesAPI_Translation::setDistance(const ModelHighAPI_Double& theDistance)
{
  fillAttribute(theDistance, mydistance);

  execute();
}

//==================================================================================================
TranslationPtr addTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const std::list<ModelHighAPI_Selection>& theMainObjects,
                              const ModelHighAPI_Selection& theAxisObject,
                              const ModelHighAPI_Double& theDistance)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Translation::ID());
  return TranslationPtr(new FeaturesAPI_Translation(aFeature, theMainObjects, theAxisObject, theDistance));
}
