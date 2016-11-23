// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Translation.h
// Created:     8 June 2015
// Author:      Dmitry Bobylev
//
// Modified by Clarisse Genrault (CEA) : 17 Nov 2016

#ifndef FeaturesPlugin_Translation_H_
#define FeaturesPlugin_Translation_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>

#include <GeomAlgoAPI_Translation.h>

/// \class FeaturesPlugin_Translation
/// \ingroup Plugins
/// \brief Feature for translation objects along the axis.
class FeaturesPlugin_Translation : public ModelAPI_Feature
{
 public:
  /// Translation kind.
  inline static const std::string& ID()
  {
    static const std::string MY_TRANSLATION_ID("Translation");
    return MY_TRANSLATION_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByAxisAndDistance".
  inline static const std::string& CREATION_METHOD_BY_DISTANCE()
  {
    static const std::string MY_CREATION_METHOD_ID("ByAxisAndDistance");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByDimensions".
  inline static const std::string& CREATION_METHOD_BY_DIMENSIONS()
  {
    static const std::string MY_CREATION_METHOD_ID("ByDimensions");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByTwoPoints".
  inline static const std::string& CREATION_METHOD_BY_TWO_POINTS()
  {
    static const std::string MY_CREATION_METHOD_ID("ByTwoPoints");
    return MY_CREATION_METHOD_ID;
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

  /// Attribute name of dimension in X.
  inline static const std::string& DX_ID()
  {
    static const std::string MY_DX_ID("dx");
    return MY_DX_ID;
  }

  /// Attribute name of dimension in Y.
  inline static const std::string& DY_ID()
  {
    static const std::string MY_DY_ID("dy");
    return MY_DY_ID;
  }

  /// Attribute name of dimension in Z.
  inline static const std::string& DZ_ID()
  {
    static const std::string MY_DZ_ID("dz");
    return MY_DZ_ID;
  }

  /// Attribute name of start point.
  inline static const std::string& START_POINT_ID()
  {
    static const std::string MY_START_POINT_ID("start_point");
    return MY_START_POINT_ID;
  }

  /// Attribute name of end point.
  inline static const std::string& END_POINT_ID()
  {
    static const std::string MY_END_POINT_ID("end_point");
    return MY_END_POINT_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Translation::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Translation();

private:
  ///Perform the translation using an axis and a distance.
  void performTranslationByAxisAndDistance();

  ///Perform the translation using three dimensions X, Y and Z
  void performTranslationByDimensions();

  ///Perform the translation usind two points
  void performTranslationByTwoPoints();

  void loadNamingDS(GeomAlgoAPI_Translation& theTranslationAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBaseShape);
};

#endif
