// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_Projection.h
// Created: 20 April 2016
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_Projection_H_
#define SketchPlugin_Projection_H_

#include "SketchPlugin_SketchEntity.h"

/** \class SketchPlugin_Projection
 *  \ingroup Plugins
 *  \brief Feature for creation of external feature as a projection onto the sketch plane.
 */
class SketchPlugin_Projection : public SketchPlugin_SketchEntity
{
public:
  /// Projection feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_PROJECTION_ID("SketchProjection");
    return MY_PROJECTION_ID;
  }
  /// Returns the kind of a feature
  virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Projection::ID();
    return MY_KIND;
  }

  static const std::string& EXTERNAL_FEATURE_ID()
  {
    static std::string MY_EXT_FEATURE_ID("ExternalFeature");
    return MY_EXT_FEATURE_ID;
  }

  static const std::string& PROJECTED_FEATURE_ID()
  {
    static std::string MY_PROJ_FEATURE_ID("ProjectedFeature");
    return MY_PROJ_FEATURE_ID;
  }

  /// Returns true because projected feature is always external
  virtual bool isFixed()
  { return true; }

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Called on change of any argument-attribute of this object: for external point
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  SketchPlugin_Projection();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  /// \brief Find projection of a feature onto sketch plane
  void computeProjection(const std::string& theID);

  bool myIsComputing;
};

#endif
