// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Factory.h
// Created:     13 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Factory_H_
#define SketcherPrs_Factory_H_

#include "SketcherPrs.h"

#include <GeomAPI_Ax3.h>
#include <GeomAPI_AISObject.h>

class SketchPlugin_Constraint;

#define GET_CONSTRAINT_PRS(NAME) \
  static AISObjectPtr NAME(SketchPlugin_Constraint* theConstraint, \
                           const std::shared_ptr<GeomAPI_Ax3>& thePlane); \


/**
* Class which creates constraints presentations
*/
class SKETCHERPRS_EXPORT SketcherPrs_Factory
{
public:
  /// Creates coincedent constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(coincidentConstraint)

  /// Creates coincedent parallel presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(parallelConstraint)

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(perpendicularConstraint)

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(rigidConstraint)

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(horisontalConstraint)

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(verticalConstraint)

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(equalConstraint)

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(tangentConstraint)
};

#endif
