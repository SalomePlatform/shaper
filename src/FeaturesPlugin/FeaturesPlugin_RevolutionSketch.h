// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionSketch.h
// Created:     11 September 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_RevolutionSketch_H_
#define FeaturesPlugin_RevolutionSketch_H_

#include <FeaturesPlugin_CompositeSketch.h>

/** \class FeaturesPlugin_RevolutionSketch
 *  \ingroup Plugins
 *  \brief This feature allows to create sketch and revol faces from this sketch 
 *         in a single operation.
 */
class FeaturesPlugin_RevolutionSketch : public FeaturesPlugin_CompositeSketch
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_REVOLUTION_ID("RevolutionSketch");
    return MY_REVOLUTION_ID;
  }

  /// \return the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_RevolutionSketch::ID();
    return MY_KIND;
  }

  /// Attribute name of an revolution axis.
  inline static const std::string& AXIS_OBJECT_ID()
  {
    static const std::string MY_AXIS_ID("axis_object");
    return MY_AXIS_ID;
  }

  /// attribute name for creation method
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string METHOD_ATTR("CreationMethod");
    return METHOD_ATTR;
  }

  /// Attribute name of revolution to angle.
  inline static const std::string& TO_ANGLE_ID()
  {
    static const std::string MY_TO_ANGLE_ID("to_angle");
    return MY_TO_ANGLE_ID;
  }

  /// Attribute name of revolution from angle.
  inline static const std::string& FROM_ANGLE_ID()
  {
    static const std::string MY_FROM_ANGLE_ID("from_angle");
    return MY_FROM_ANGLE_ID;
  }
  
  /// Attribute name of an object to which the revolution grows.
  inline static const std::string& TO_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("to_object");
    return MY_TO_OBJECT_ID;
  }

  /// Attribute name of extrusion offset.
  inline static const std::string& TO_OFFSET_ID()
  {
    static const std::string MY_TO_OFFSET_ID("to_offset");
    return MY_TO_OFFSET_ID;
  }

  /// Attribute name of an object from which the revolution grows.
  inline static const std::string& FROM_OBJECT_ID()
  {
    static const std::string MY_FROM_OBJECT_ID("from_object");
    return MY_FROM_OBJECT_ID;
  }

  /// Attribute name of extrusion offset.
  inline static const std::string& FROM_OFFSET_ID()
  {
    static const std::string MY_FROM_OFFSET_ID("from_offset");
    return MY_FROM_OFFSET_ID;
  }

protected:
  /// Init attributes for revolution.
  virtual void initMakeSolidsAttributes();

  /// Create solids from faces with revolution.
  virtual void makeSolid(const std::shared_ptr<GeomAPI_Shape> theFace,
                         std::shared_ptr<GeomAlgoAPI_MakeShape>& theMakeShape);

public:
  /// Use plugin manager for features creation.
  FeaturesPlugin_RevolutionSketch();
};

#endif
