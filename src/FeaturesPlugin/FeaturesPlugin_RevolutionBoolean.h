// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionBoolean.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_RevolutionBoolean_H_
#define FeaturesPlugin_RevolutionBoolean_H_

#include <FeaturesPlugin_CompositeBoolean.h>

/** \class FeaturesPlugin_RevolutionBoolean
 *  \ingroup Plugins
 */
class FeaturesPlugin_RevolutionBoolean : public FeaturesPlugin_CompositeBoolean
{
public:
  /// Attribute name of an revolution axis.
  inline static const std::string& AXIS_OBJECT_ID()
  {
    static const std::string MY_AXIS_ID("axis_object");
    return MY_AXIS_ID;
  }

  /// Attribute name of an object from which the revolution grows.
  inline static const std::string& FROM_OBJECT_ID()
  {
    static const std::string MY_FROM_OBJECT_ID("from_object");
    return MY_FROM_OBJECT_ID;
  }

  /// Attribute name of revolution from angle.
  inline static const std::string& FROM_ANGLE_ID()
  {
    static const std::string MY_FROM_ANGLE_ID("from_angle");
    return MY_FROM_ANGLE_ID;
  }

  /// attribute name of an object to which the revolution grows.
  inline static const std::string& TO_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("to_object");
    return MY_TO_OBJECT_ID;
  }

  /// Attribute name of revolution to angle.
  inline static const std::string& TO_ANGLE_ID()
  {
    static const std::string MY_TO_ANGLE_ID("to_angle");
    return MY_TO_ANGLE_ID;
  }

protected:
  /// Init attributes for revolution.
  virtual void initMakeSolidsAttributes();

  /// Create solids from faces with revolution.
  virtual ListOfShape MakeSolids(const ListOfShape& theFaces);

protected:
  FeaturesPlugin_RevolutionBoolean(){};
};

#endif
