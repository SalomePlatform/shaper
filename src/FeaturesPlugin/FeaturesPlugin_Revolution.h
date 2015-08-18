// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Revolution.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Revolution_H_
#define FeaturesPlugin_Revolution_H_

#include <FeaturesPlugin.h>

#include <GeomAlgoAPI_Revolution.h>
#include <ModelAPI_Feature.h>

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/** \class FeaturesPlugin_Revolution
 *  \ingroup Plugins
 *  \brief Feature for creation of revolution from the planar face.
 *  Revolution creates the lateral faces based on edges of the base face and
 *  the start and end faces and/or start and end angles.
 */
class FeaturesPlugin_Revolution : public ModelAPI_Feature
{
 public:
  /// Revolution kind.
  inline static const std::string& ID()
  {
    static const std::string MY_REVOLUTION_ID("Revolution");
    return MY_REVOLUTION_ID;
  }

  /// Attribute name of references sketch entities list, it should contain a sketch result or
  /// a pair a sketch result to sketch face.
  inline static const std::string& LIST_ID()
  {
    static const std::string MY_GROUP_LIST_ID("base");
    return MY_GROUP_LIST_ID;
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

  /// Attribute name of revolution angle.
  inline static const std::string& TO_ANGLE_ID()
  {
    static const std::string MY_TO_ANGLE_ID("to_angle");
    return MY_TO_ANGLE_ID;
  }

  /// Attribute name of revolution angle.
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

  /// attribute name of extrusion offset.
  inline static const std::string& TO_OFFSET_ID()
  {
    static const std::string MY_TO_OFFSET_ID("to_offset");
    return MY_TO_OFFSET_ID;
  }

  /// Attribute name of tool object.
  inline static const std::string& FROM_OBJECT_ID()
  {
    static const std::string MY_FROM_OBJECT_ID("from_object");
    return MY_FROM_OBJECT_ID;
  }

  /// attribute name of extrusion offset.
  inline static const std::string& FROM_OFFSET_ID()
  {
    static const std::string MY_FROM_OFFSET_ID("from_offset");
    return MY_FROM_OFFSET_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Revolution::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Revolution();

private:
  /// Load Naming data structure of the feature to the document.
  void loadNamingDS(GeomAlgoAPI_Revolution& theRevolAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBasis);
};

#endif
