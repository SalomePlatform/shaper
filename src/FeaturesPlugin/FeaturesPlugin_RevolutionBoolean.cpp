// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionBoolean.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_RevolutionBoolean.h"

//=================================================================================================
void FeaturesPlugin_RevolutionBoolean::initAttributes()
{
  FeaturesPlugin_Revolution::initAttributes();

  initBooleanAttributes();
}

//=================================================================================================
bool FeaturesPlugin_RevolutionBoolean::makeGeneration(ListOfShape& theBaseShapes,
                                                      ListOfMakeShape& theMakeShapes)
{
  return makeRevolutions(theBaseShapes, theMakeShapes);
}

//=================================================================================================
void FeaturesPlugin_RevolutionBoolean::storeGenerationHistory(ResultBodyPtr theResultBody,
                                                             const GeomShapePtr theBaseShape,
                                                             const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                                             int& theTag)
{
  FeaturesPlugin_Revolution::storeGenerationHistory(theResultBody, theBaseShape, theMakeShape, theTag);
}
