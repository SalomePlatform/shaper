// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionBoolean.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_ExtrusionBoolean_H_
#define FeaturesPlugin_ExtrusionBoolean_H_

#include "FeaturesPlugin_Extrusion.h"
#include "FeaturesPlugin_CompositeBoolean.h"

/// \class FeaturesPlugin_ExtrusionBoolean
/// \ingroup Plugins
/// \brief Interface for the composite extrusion feature.
class FeaturesPlugin_ExtrusionBoolean: public FeaturesPlugin_Extrusion, public FeaturesPlugin_CompositeBoolean
{
public:
  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

protected:
  FeaturesPlugin_ExtrusionBoolean(){};

  // Creates extrusions.
  bool makeGeneration(ListOfShape& theBaseShapes,
                      ListOfMakeShape& theMakeShapes);

  /// Stores generation history.
  void storeGenerationHistory(ResultBodyPtr theResultBody,
                              const GeomShapePtr theBaseShape,
                              const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                              int& theTag);
};

#endif
