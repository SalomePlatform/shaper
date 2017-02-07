// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_IEntityWrapper.h
// Created: 30 Nov 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_IEntityWrapper_H_
#define SketchSolver_IEntityWrapper_H_

#include <SketchSolver.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_Feature.h>

#include <list>
#include <memory>

/// Types of entities
enum SketchSolver_EntityType {
  ENTITY_UNKNOWN = 0,
  ENTITY_SCALAR,
  ENTITY_ANGLE,
  ENTITY_POINT,
  ENTITY_LINE,
  ENTITY_CIRCLE,
  ENTITY_ARC
};

/**
 *  Wrapper providing operations with entities regardless the solver.
 */
class SketchSolver_IEntityWrapper
{
public:
  SketchSolver_IEntityWrapper() : myExternal(false) {}
  virtual ~SketchSolver_IEntityWrapper() {}

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const = 0;

  /// \brief Change flag indicating the entity cannot be changed in the solver
  void setExternal(bool theExternal) { myExternal = theExternal; }
  /// \brief Return the External flag
  bool isExternal() const { return myExternal; }

private:
  bool myExternal;
};

typedef std::shared_ptr<SketchSolver_IEntityWrapper> EntityWrapperPtr;

#endif
