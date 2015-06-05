// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Rotation.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Rotation_H_
#define FeaturesPlugin_Rotation_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>

#include <GeomAlgoAPI_Rotation.h>

/** \class FeaturesPlugin_Rotation
 *  \ingroup Plugins
 *  \brief Feature for rotation objects around the axis.
 */
class FeaturesPlugin_Rotation : public ModelAPI_Feature
{
 public:
  /// Rotation kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ROTATION_ID("Rotation");
    return MY_ROTATION_ID;
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

  /// Attribute name of angle.
  inline static const std::string& ANGLE_ID()
  {
    static const std::string MY_ANGLE_ID("angle");
    return MY_ANGLE_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Rotation::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Rotation();

private:
  void LoadNamingDS(const GeomAlgoAPI_Rotation& theRotaionAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBaseShape);
};

#endif
