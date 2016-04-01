// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionBoolean.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_RevolutionBoolean_H_
#define FeaturesPlugin_RevolutionBoolean_H_

#include "FeaturesPlugin_Revolution.h"
#include "FeaturesPlugin_CompositeBoolean.h"

/// \class FeaturesPlugin_RevolutionBoolean
/// \ingroup Plugins
/// \brief Interface for the composite revolution feature.
class FeaturesPlugin_RevolutionBoolean : public FeaturesPlugin_Revolution, public FeaturesPlugin_CompositeBoolean
{
public:
  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

protected:
  FeaturesPlugin_RevolutionBoolean(){};

  // Creates revolutions.
  bool makeGeneration(ListOfShape& theBaseShapes,
                      ListOfMakeShape& theMakeShapes);

  /// Stores generation history.
  void storeGenerationHistory(ResultBodyPtr theResultBody,
                              const GeomShapePtr theBaseShape,
                              const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                              int& theTag);
};

#endif
