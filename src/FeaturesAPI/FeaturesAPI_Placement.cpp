// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Placement.cpp
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Placement.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Placement::FeaturesAPI_Placement(const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Placement::FeaturesAPI_Placement(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                                             const std::list<ModelHighAPI_Selection>& theObjects,
                                             const ModelHighAPI_Selection& theStartShape,
                                             const ModelHighAPI_Selection& theEndShape,
                                             const bool theReverseDirection,
                                             const bool theCentering)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setObjects(theObjects);
    setStartShape(theStartShape);
    setEndShape(theEndShape);
    setReverseDirection(theReverseDirection);
    setCentering(theCentering);
  }
}

//==================================================================================================
FeaturesAPI_Placement::~FeaturesAPI_Placement()
{

}

//==================================================================================================
void FeaturesAPI_Placement::setObjects(const std::list<ModelHighAPI_Selection>& theObjects)
{
  fillAttribute(theObjects, myobjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Placement::setStartShape(const ModelHighAPI_Selection& theStartShape)
{
  fillAttribute(theStartShape, mystartShape);

  execute();
}

//==================================================================================================
void FeaturesAPI_Placement::setEndShape(const ModelHighAPI_Selection& theEndShape)
{
  fillAttribute(theEndShape, myendShape);

  execute();
}

//==================================================================================================
void FeaturesAPI_Placement::setReverseDirection(const bool theReverseDirection)
{
  fillAttribute(theReverseDirection, myreverseDirection);

  execute();
}

//==================================================================================================
void FeaturesAPI_Placement::setCentering(const bool theCentering)
{
  fillAttribute(theCentering, mycentering);

  execute();
}

// TODO(spo): make add* as static functions of the class
//==================================================================================================
PlacementPtr addPlacement(const std::shared_ptr<ModelAPI_Document> & thePart,
                          const std::list<ModelHighAPI_Selection>& theObjects,
                          const ModelHighAPI_Selection& theStartShape,
                          const ModelHighAPI_Selection& theEndShape,
                          const bool theReverseDirection,
                          const bool theCentering)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Placement::ID());
  return PlacementPtr(new FeaturesAPI_Placement(aFeature,
                                                theObjects,
                                                theStartShape,
                                                theEndShape,
                                                theReverseDirection,
                                                theCentering));
}
