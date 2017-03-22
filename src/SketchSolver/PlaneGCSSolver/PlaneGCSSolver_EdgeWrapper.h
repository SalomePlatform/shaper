// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_EdgeWrapper.h
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_EdgeWrapper_H_
#define PlaneGCSSolver_EdgeWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_EntityWrapper.h>

/**
 *  Wrapper providing operations with PlaneGCS entities (lines, circles and arcs).
 */
class PlaneGCSSolver_EdgeWrapper : public PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_EdgeWrapper(const GCSCurvePtr theEntity);

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

typedef std::shared_ptr<PlaneGCSSolver_EdgeWrapper> EdgeWrapperPtr;

#endif
