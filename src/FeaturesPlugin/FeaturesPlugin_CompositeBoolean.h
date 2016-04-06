// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_CompositeBoolean.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_CompositeBoolean_H_
#define FeaturesPlugin_CompositeBoolean_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <GeomAlgoAPI_MakeShape.h>

/// \class FeaturesPlugin_CompositeBoolean
/// \ingroup Plugins
/// \brief Interface for the composite boolean feature.
class FeaturesPlugin_CompositeBoolean
{
public:
  enum OperationType {
    BOOL_CUT,
    BOOL_FUSE,
    BOOL_COMMON,
    BOOL_SMASH
  };

  /// Attribute name of main objects.
  inline static const std::string& OBJECTS_ID()
  {
    static const std::string MY_OBJECTS_ID("main_objects");
    return MY_OBJECTS_ID;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void executeCompositeBoolean();

protected:
  FeaturesPlugin_CompositeBoolean(){};

  /// Initializes boolean attributes.
  void initBooleanAttributes();

  /// This function need to be defined for extrusion/revolution generation.
  virtual bool makeGeneration(ListOfShape& theBaseShapes,
                              ListOfMakeShape& theMakeShapes) = 0;

  /// Makes boolean operation.
  /// \param[in] theTools list of tools.
  /// \param[out] theObjects list of objects.
  /// \param[out] theMakeShapes list of according algos.
  /// \return false in failed.
  bool makeBoolean(const ListOfShape& theTools,
                   ListOfShape& theObjects,
                   ListOfMakeShape& theMakeShapes);

  /// Stores generation history.
  virtual void storeGenerationHistory(ResultBodyPtr theResultBody,
                                      const GeomShapePtr theBaseShape,
                                      const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                      int& theTag) = 0;

  /// Stores modification history.
  void storeModificationHistory(ResultBodyPtr theResultBody,
                                const GeomShapePtr theObject,
                                const ListOfShape& theTools,
                                const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                int& theTag);

protected:
  ModelAPI_Feature* myFeature;
  OperationType myOperationType;
};

#endif
