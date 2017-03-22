// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_EntityWrapper.h
// Created: 30 Nov 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_EntityWrapper_H_
#define PlaneGCSSolver_EntityWrapper_H_

#include <PlaneGCSSolver_Defs.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_Feature.h>

#include <list>
#include <memory>

/**
 *  Wrapper providing operations with entities regardless the solver.
 */
class PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_EntityWrapper() : myExternal(false) {}
  virtual ~PlaneGCSSolver_EntityWrapper() {}

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const = 0;

  /// \brief Change flag indicating the entity cannot be changed in the solver
  void setExternal(bool theExternal) { myExternal = theExternal; }
  /// \brief Return the External flag
  bool isExternal() const { return myExternal; }

private:
  bool myExternal;
};

typedef std::shared_ptr<PlaneGCSSolver_EntityWrapper> EntityWrapperPtr;

#endif
