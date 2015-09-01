// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_Point.h
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_Point_H_
#define SketchPlugin_Point_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_SketchEntity.h"
#include <list>

/**\class SketchPlugin_Point
 * \ingroup Plugins
 * \brief Feature for creation of a new point.
 */
class SketchPlugin_Point : public SketchPlugin_SketchEntity
{
 public:
  /// Point feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_POINT_ID("SketchPoint");
    return MY_POINT_ID;
  }
  /// Coordinates of the point
  inline static const std::string& COORD_ID()
  {
    static const std::string MY_COORD_ID("PointCoordindates");
    return MY_COORD_ID;
  }
  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Point::ID();
    return MY_KIND;
  }

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Called on change of any argument-attribute of this object: for external point
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  SketchPlugin_Point();
};

#endif
