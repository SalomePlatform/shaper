// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_IntersectionPoint.h
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_IntersectionPoint_H_
#define SketchPlugin_IntersectionPoint_H_

#include "SketchPlugin_Point.h"

/**\class SketchPlugin_IntersectionPoint
 * \ingroup Plugins
 * \brief Feature for creation of external point as an intersection
 *        between external edge and a plane of the sketch.
 */
class SketchPlugin_IntersectionPoint : public SketchPlugin_Point
{
public:
  /// Point feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_POINT_ID("SketchIntersectionPoint");
    return MY_POINT_ID;
  }
  /// Returns the kind of a feature
  virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_IntersectionPoint::ID();
    return MY_KIND;
  }

  static const std::string& EXTERNAL_LINE_ID()
  {
    static std::string MY_LINE_ID("ExternalLine");
    return MY_LINE_ID;
  }

  /// Returns true because intersection point is always external
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
  SketchPlugin_IntersectionPoint();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  /// \brief Find intersection between a line and a sketch plane
  void computePoint();
};

#endif
