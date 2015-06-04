// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Rotation.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Rotation_H_
#define FeaturesPlugin_Rotation_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>

/** \class FeaturesPlugin_Rotation
 *  \ingroup Plugins
 *  \brief Feature for creation of revolution from the planar face.
 *  Revolution creates the lateral faces based on edges of the base face and
 *  the start and end faces and/or start and end angles.
 */
class FeaturesPlugin_Rotation : public ModelAPI_Feature
{
 public:
  /// Revolution kind.
  inline static const std::string& ID()
  {
    static const std::string MY_REVOLUTION_ID("Rotation");
    return MY_REVOLUTION_ID;
  }

  /// Attribute name of references sketch entities list, it should contain a sketch result or
  /// a pair a sketch result to sketch face.
  inline static const std::string& LIST_ID()
  {
    static const std::string MY_GROUP_LIST_ID("base");
    return MY_GROUP_LIST_ID;
  }

  /// Attribute name of an object to which the extrusion grows.
  inline static const std::string& AXIS_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("axis_object");
    return MY_TO_OBJECT_ID;
  }

  /// Attribute name of revolution angle.
  inline static const std::string& ANGLE_ID()
  {
    static const std::string MY_TO_ANGLE_ID("angle");
    return MY_TO_ANGLE_ID;
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
};

#endif
