// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Line.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Line_H_
#define SketchPlugin_Line_H_

#include "SketchPlugin.h"
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Sketch.h>
#include <list>

/**\class SketchPlugin_Line
 * \ingroup Plugins
 * \brief Feature for creation of the new part in PartSet.
 */
class SketchPlugin_Line : public SketchPlugin_SketchEntity
{
 public:
  /// Arc feature kind
  inline static const std::string& ID()
  {
    static const std::string SKETCH_LINE_ID("SketchLine");
    return SKETCH_LINE_ID;
  }
  /// Start 2D point of the line
  inline static const std::string& START_ID()
  {
    static const std::string MY_START_ID("StartPoint");
    return MY_START_ID;
  }
  /// End 2D point of the line
  inline static const std::string& END_ID()
  {
    static const std::string MY_END_ID("EndPoint");
    return MY_END_ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Line::ID();
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

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  virtual double distanceToPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  SketchPlugin_Line();
};

#endif
