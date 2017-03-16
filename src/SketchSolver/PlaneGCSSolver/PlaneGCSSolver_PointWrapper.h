// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_PointWrapper.h
// Created: 16 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_PointWrapper_H_
#define PlaneGCSSolver_PointWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_EntityWrapper.h>

/**
 *  Wrapper providing operations with PlaneGCS points.
 */
class PlaneGCSSolver_PointWrapper : public PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_PointWrapper(const GCSPointPtr thePoint);

  /// \brief Return PlaneGCS point
  const GCSPointPtr& point() const
  { return myPoint; }
  /// \brief Return PlaneGCS point to change
  GCSPointPtr& changeEntity()
  { return myPoint; }

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const
  { return ENTITY_POINT; }

private:
  GCSPointPtr myPoint;
};

#endif
