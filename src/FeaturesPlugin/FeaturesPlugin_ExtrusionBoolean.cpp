// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionBoolean.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_ExtrusionBoolean.h"

//=================================================================================================
void FeaturesPlugin_ExtrusionBoolean::initAttributes()
{
  FeaturesPlugin_Extrusion::initAttributes();

  initBooleanAttributes();
}

//=================================================================================================
bool FeaturesPlugin_ExtrusionBoolean::makeGeneration(ListOfShape& theBaseShapes,
                                                     ListOfMakeShape& theMakeShapes)
{
  return makeExtrusions(theBaseShapes, theMakeShapes);
}

//=================================================================================================
void FeaturesPlugin_ExtrusionBoolean::storeGenerationHistory(ResultBodyPtr theResultBody,
                                                             const GeomShapePtr theBaseShape,
                                                             const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                                             int& theTag)
{
  FeaturesPlugin_Extrusion::storeGenerationHistory(theResultBody, theBaseShape, theMakeShape, theTag);
}
