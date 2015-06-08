// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Movement.h
// Created:     8 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Movement_H_
#define FeaturesPlugin_Movement_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>

#include <GeomAlgoAPI_Movement.h>

/** \class FeaturesPlugin_Movement
 *  \ingroup Plugins
 *  \brief Feature for movement objects along the axis.
 */
class FeaturesPlugin_Movement : public ModelAPI_Feature
{
 public:
  /// Movement kind.
  inline static const std::string& ID()
  {
    static const std::string MY_MOVEMENT_ID("Movement");
    return MY_MOVEMENT_ID;
  }

  /// Attribute name of referenced objects.
  inline static const std::string& OBJECTS_LIST_ID()
  {
    static const std::string MY_OBJECTS_LIST_ID("main_objects");
    return MY_OBJECTS_LIST_ID;
  }

  /// Attribute name of an axis.
  inline static const std::string& AXIS_OBJECT_ID()
  {
    static const std::string MY_AXIS_OBJECT_ID("axis_object");
    return MY_AXIS_OBJECT_ID;
  }

  /// Attribute name of distance.
  inline static const std::string& DISTANCE_ID()
  {
    static const std::string MY_DISTANCE_ID("distance");
    return MY_DISTANCE_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Movement::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Movement();

private:
  void LoadNamingDS(const GeomAlgoAPI_Movement& theMovementAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBaseShape);
};

#endif
