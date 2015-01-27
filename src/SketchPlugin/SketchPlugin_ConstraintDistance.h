// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintDistance.h
// Created: 23 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintDistance_H_
#define SketchPlugin_ConstraintDistance_H_

#include "SketchPlugin.h"
#include "SketchPlugin_ConstraintBase.h"
#include "SketchPlugin_Sketch.h"
#include "ModelAPI_Data.h"

#include <list>

class SketchPlugin_Line;
class GeomDataAPI_Point2D;

#define DISTANCE_COLOR "#ff00ff"

/** \class SketchPlugin_ConstraintDistance
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which defines a distance
 *         between a point and another feature (point, line, plane or face)
 *
 *  This constraint has three attributes:
 *  SketchPlugin_Constraint::VALUE(), SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintDistance : public SketchPlugin_ConstraintBase
{
 public:
  /// Distance constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_DISTANCE_ID("SketchConstraintDistance");
    return MY_CONSTRAINT_DISTANCE_ID;
  }

  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintDistance::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the current distance between the feature attributes
  double calculateCurrentDistance() const;

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintDistance();
};

/// Obtain the point object from specified constraint parameter
std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                       const std::string& theAttribute);

std::shared_ptr<SketchPlugin_Line> getFeatureLine(DataPtr theData,
                                                    const std::string& theAttribute);

std::shared_ptr<GeomAPI_Pnt2d> getProjectionPoint(
    const std::shared_ptr<SketchPlugin_Line>& theLine,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

#endif
