// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_Fillet.h
// Created: 19 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_Fillet_H_
#define SketchPlugin_Fillet_H_

#include "SketchPlugin.h"

#include "SketchPlugin_SketchEntity.h"

#include <GeomAPI_IPresentable.h>

class GeomAPI_XY;

/// \class SketchPlugin_Fillet
/// \ingroup Plugins
/// \brief Feature for creation of a new arc filleting two objects which have
/// coincident point.
class SketchPlugin_Fillet: public SketchPlugin_SketchEntity, public GeomAPI_IPresentable
{
 public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_FEATURE_ID("SketchFillet");
    return MY_FEATURE_ID;
  }

  /// Attribute name of fillet point.
  inline static const std::string& FILLET_POINT_ID()
  {
    static const std::string ID("fillet_point");
    return ID;
  }

  /// \return the kind of a feature.
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Fillet::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed.
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes.
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;};

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;};

  /// Reimplemented from SketchPlugin_Feature::move().
  /// Do nothing.
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY) {};

  /// \brief Use plugin manager for features creation
  SketchPlugin_Fillet();

private:
    FeaturePtr myBaseFeatures[2];
    std::string myFeatAttributes[4]; // attributes of features
    bool myIsReversed;
    bool myIsNotInversed[2]; // indicates which point the features share
    bool myFilletCreated;
    std::shared_ptr<GeomAPI_XY> myCenterXY;
    std::shared_ptr<GeomAPI_XY> myTangentXY1;
    std::shared_ptr<GeomAPI_XY> myTangentXY2;
};

#endif
