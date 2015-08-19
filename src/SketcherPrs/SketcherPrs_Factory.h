// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Factory.h
// Created:     13 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Factory_H_
#define SketcherPrs_Factory_H_

#include "SketcherPrs.h"

#include <ModelAPI_Feature.h>

#include <GeomAPI_Ax3.h>
#include <GeomAPI_AISObject.h>

#define GET_CONSTRAINT_PRS(NAME) \
  static AISObjectPtr NAME(ModelAPI_Feature* theConstraint, \
                           const std::shared_ptr<GeomAPI_Ax3>& thePlane);


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

  /// Creates parallel constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(parallelConstraint)

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(perpendicularConstraint)

  /// Creates rigid constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(rigidConstraint)

  /// Creates horizontal constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(horisontalConstraint)

  /// Creates vertical constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(verticalConstraint)

  /// Creates equal constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(equalConstraint)

  /// Creates tangent constraiont presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(tangentConstraint)

  /// Creates radius dimension presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(radiusConstraint)

  /// Creates length dimension presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(lengthDimensionConstraint)

  /// Creates mirror constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(mirrorConstraint)

  /// Creates translate constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(translateConstraint)

  /// Creates rotate constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(rotateConstraint)

  /// Creates angle constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  GET_CONSTRAINT_PRS(angleConstraint)
};

#endif
