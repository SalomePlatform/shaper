// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Extrusion.h
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#ifndef FeaturesPlugin_Extrusion_H_
#define FeaturesPlugin_Extrusion_H_

#include "FeaturesPlugin.h"

#include "FeaturesPlugin_CompositeSketch.h"

#include <GeomAlgoAPI_MakeShape.h>

/// \class FeaturesPlugin_Extrusion
/// \ingroup Plugins
/// \brief Feature for creation of extrusion from the planar face.
/// Extrusion creates the lateral faces based on edges of the base face and
/// the top and bottom faces equal to the base face or this faces can be projection on the
/// bounding planes if they were set. Direction of extrusion is taken from the face
/// plane or if the bounding faces were set then it will be from the bottom to the top plane.
class FeaturesPlugin_Extrusion: public FeaturesPlugin_CompositeSketch
{
public:
  /// Use plugin manager for features creation
  FeaturesPlugin_Extrusion();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Extrusion");
    return MY_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name of an object to which the extrusion grows.
  inline static const std::string& DIRECTION_OBJECT_ID()
  {
    static const std::string MY_DIRECTION_OBJECT_ID("direction_object");
    return MY_DIRECTION_OBJECT_ID;
  }

  /// Attribute name of extrusion to size.
  inline static const std::string& TO_SIZE_ID()
  {
    static const std::string MY_TO_SIZE_ID("to_size");
    return MY_TO_SIZE_ID;
  }

  /// Attribute name of extrusion from size.
  inline static const std::string& FROM_SIZE_ID()
  {
    static const std::string MY_FROM_SIZE_ID("from_size");
    return MY_FROM_SIZE_ID;
  }

  /// Attribute name of an object to which the extrusion grows.
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

  /// Attribute name of an object from which the extrusion grows.
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

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Extrusion::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

protected:
  /// Generates extrusions.
  /// \param[out] theBaseShapes list of base shapes.
  /// \param[out] theMakeShapes list of according algos.
  /// \return false in case one of algo failed.
  bool makeExtrusions(ListOfShape& theBaseShapes,
                      ListOfMakeShape& theMakeShapes);
};

#endif
