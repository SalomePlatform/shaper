// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_EntityWrapper.h
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_EntityWrapper_H_
#define PlaneGCSSolver_EntityWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <SketchSolver_IEntityWrapper.h>

/**
 *  Wrapper providing operations with PlaneGCS entities (lines, circles and arcs).
 */
class PlaneGCSSolver_EntityWrapper : public SketchSolver_IEntityWrapper
{
public:
  PlaneGCSSolver_EntityWrapper(const GCSCurvePtr theEntity);

  /// \brief Return PlaneGCS geometric entity
  const GCSCurvePtr& entity() const
  { return myEntity; }
  /// \brief Return PlaneGCS geometric entity to change
  GCSCurvePtr& changeEntity()
  { return myEntity; }

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const
  { return myType; }

private:
  SketchSolver_EntityType myType;
  GCSCurvePtr myEntity;
};

#endif
