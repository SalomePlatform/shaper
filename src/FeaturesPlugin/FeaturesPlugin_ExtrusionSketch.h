// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionSketch.h
// Created:     11 September 2015
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_ExtrusionSketch_H_
#define FeaturesPlugin_ExtrusionSketch_H_

#include <FeaturesPlugin_CompositeSketch.h>

/** \class FeaturesPlugin_ExtrusionSketch
 *  \ingroup Plugins
 *  \brief This feature allows to create sketch and extrude faces from this sketch
 *         in a single operation.
 */
class FeaturesPlugin_ExtrusionSketch : public FeaturesPlugin_CompositeSketch
{
 public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_EXTRUSION_ID("ExtrusionSketch");
    return MY_EXTRUSION_ID;
  }

  /// \return the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_ExtrusionSketch::ID();
    return MY_KIND;
  }

  /// attribute name for creation method
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string METHOD_ATTR("CreationMethod");
    return METHOD_ATTR;
  }

  /// attribute name of extrusion size
  inline static const std::string& TO_SIZE_ID()
  {
    static const std::string MY_TO_SIZE_ID("to_size");
    return MY_TO_SIZE_ID;
  }

  /// attribute name of extrusion size
  inline static const std::string& FROM_SIZE_ID()
  {
    static const std::string MY_FROM_SIZE_ID("from_size");
    return MY_FROM_SIZE_ID;
  }

  /// attribute name of an object to which the extrusion grows.
  inline static const std::string& TO_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("to_object");
    return MY_TO_OBJECT_ID;
  }

  /// attribute name of extrusion offset
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

  /// attribute name of extrusion offset
  inline static const std::string& FROM_OFFSET_ID()
  {
    static const std::string MY_FROM_OFFSET_ID("from_offset");
    return MY_FROM_OFFSET_ID;
  }

protected:
  /// Init attributes for extrusion.
  virtual void initMakeSolidsAttributes();

  /// Create solid from face with extrusion.
  virtual void makeSolid(const std::shared_ptr<GeomAPI_Shape> theFace,
                         std::shared_ptr<GeomAlgoAPI_MakeShape>& theMakeShape);

public:
  /// Use plugin manager for features creation.
  FeaturesPlugin_ExtrusionSketch();
};

#endif
