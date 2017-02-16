// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Scale.h
// Created:     13 Jan 2017
// Author:      Clarisse Genrault (CEA)

#ifndef FEATURESPLUGIN_SCALE_H_
#define FEATURESPLUGIN_SCALE_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>

#include <GeomAlgoAPI_Scale.h>

/** \class FeaturesPlugin_Scale
 *  \ingroup Plugins
 *  \brief Feature for changing the scale of an object.
 */
class FeaturesPlugin_Scale : public ModelAPI_Feature
{
 public:
  /// Scale kind.
  inline static const std::string& ID()
  {
    static const std::string MY_SCALE_ID("Scale");
    return MY_SCALE_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByFactor".
  inline static const std::string& CREATION_METHOD_BY_FACTOR()
  {
    static const std::string MY_CREATION_METHOD_ID("ByFactor");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method "ByFactor".
  inline static const std::string& CREATION_METHOD_BY_DIMENSIONS()
  {
    static const std::string MY_CREATION_METHOD_ID("ByDimensions");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name of referenced objects.
  inline static const std::string& OBJECTS_LIST_ID()
  {
    static const std::string MY_OBJECTS_LIST_ID("main_objects");
    return MY_OBJECTS_LIST_ID;
  }

  /// Attribute name of center point.
  inline static const std::string& CENTER_POINT_ID()
  {
    static const std::string MY_CENTER_POINT_ID("center_point");
    return MY_CENTER_POINT_ID;
  }

  /// Attribute name of scale factor.
  inline static const std::string& SCALE_FACTOR_ID()
  {
    static const std::string MY_SCALE_FACTOR_ID("scale_factor");
    return MY_SCALE_FACTOR_ID;
  }

  /// Attribute name of scale factor in X.
  inline static const std::string& SCALE_FACTOR_X_ID()
  {
    static const std::string MY_SCALE_FACTOR_X_ID("scale_factor_x");
    return MY_SCALE_FACTOR_X_ID;
  }

  /// Attribute name of scale factor in Y.
  inline static const std::string& SCALE_FACTOR_Y_ID()
  {
    static const std::string MY_SCALE_FACTOR_Y_ID("scale_factor_y");
    return MY_SCALE_FACTOR_Y_ID;
  }

  /// Attribute name of scale factor in Z.
  inline static const std::string& SCALE_FACTOR_Z_ID()
  {
    static const std::string MY_SCALE_FACTOR_Z_ID("scale_factor_z");
    return MY_SCALE_FACTOR_Z_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Scale::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Scale();

private:
  /// Perform scale using a central point and a value of the scale.
  void performScaleByFactor();

  /// Perform symmetry using a central point and three dimensions
  void performScaleByDimensions();

  /// Perform the naming
  void loadNamingDS(GeomAlgoAPI_Scale& theScaleAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBaseShape);
};

#endif // FEATURESPLUGIN_SCALE_H_