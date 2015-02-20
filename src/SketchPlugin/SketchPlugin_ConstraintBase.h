// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintBase.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintBase_H_
#define SketchPlugin_ConstraintBase_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Sketch.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <GeomAPI_IPresentable.h>

#include <list>

/** Description: 
 *    Each constraint uses a set of parameters. In the SolveSpace library 
 *    these parameters are named "valA", "ptA", "ptB", "entityA", "entityB". 
 *    The "ptA" and "ptB" parameters represents a point in the constraint.
 *    The "entityA" and "entityB" represents any other object (and a point too).
 *    And the "valA" represents a real value.
 *
 *    The attributes below are named independent of the SolveSpace.
 *    Some of them may be unused. 
 *
 *    Also the list of possible attributes is provided to simplify assignment.
 */

#define SKETCH_CONSTRAINT_COLOR "#ffff00"
#define SKETCH_DIMENSION_COLOR "#ff00ff"

/** \class SketchPlugin_ConstraintBase
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint between other features.
 *         Some feature's methods implemented here as dummy to
 *         Base class for all constraints.
 */
class SketchPlugin_ConstraintBase : public SketchPlugin_Constraint, public GeomAPI_IPresentable
{
 public:
  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /** \brief Adds sub-feature of the higher level feature (sub-element of the sketch)
   *  \param theFeature sub-feature
   */
  SKETCHPLUGIN_EXPORT virtual const void addSub(const FeaturePtr& theFeature);
  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  virtual double distanceToPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

 protected:
  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintBase()
  {
  }
};

#endif
