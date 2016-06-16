// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Intersection.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Intersection.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Intersection::FeaturesAPI_Intersection(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Intersection::FeaturesAPI_Intersection(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                   const std::list<ModelHighAPI_Selection>& theObjects,
                                                   const std::list<ModelHighAPI_Selection>& theTools)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theObjects, myobjects);
    fillAttribute(theTools, mytools);

    execute();
  }
}

//==================================================================================================
FeaturesAPI_Intersection::~FeaturesAPI_Intersection()
{

}

//==================================================================================================
void FeaturesAPI_Intersection::setObjects(const std::list<ModelHighAPI_Selection>& theObjects)
{
  fillAttribute(theObjects, myobjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Intersection::setTools(const std::list<ModelHighAPI_Selection>& theTools)
{
  fillAttribute(theTools, mytools);

  execute();
}

// TODO(spo): make add* as static functions of the class
//==================================================================================================
IntersectionPtr addIntersection(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theObjects,
                                const std::list<ModelHighAPI_Selection>& theTools)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Intersection::ID());
  return IntersectionPtr(new FeaturesAPI_Intersection(aFeature, theObjects, theTools));
}
