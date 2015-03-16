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

/**
* Class which creates constraints presentations
*/
class SKETCHERPRS_EXPORT SketcherPrs_Factory
{
public:
  /// Creates coincedent constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  static AISObjectPtr coincidentConstraint(SketchPlugin_Constraint* theConstraint, 
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Creates coincedent parallel presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  static AISObjectPtr parallelConstraint(SketchPlugin_Constraint* theConstraint, 
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  static AISObjectPtr perpendicularConstraint(SketchPlugin_Constraint* theConstraint, 
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  static AISObjectPtr rigidConstraint(SketchPlugin_Constraint* theConstraint, 
                                      const std::shared_ptr<GeomAPI_Ax3>& thePlane);
};

#endif
