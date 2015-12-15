// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMirror.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMirror_H_
#define SketchSolver_ConstraintMirror_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintMirror
 *  \ingroup Plugins
 *  \brief   Convert fillet constraint to SolveSpace structure
 */
class SketchSolver_ConstraintMirror : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMirror(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myNumberOfObjects(0)
  {}

  /// \brief Update constraint
  virtual void update();

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<EntityWrapperPtr>& theAttributes)
  { /* do nothing here */ }

  /// \brief Generate list of entities of mirror constraint
  /// \param[out] theMirrorLine     entity corresponding to mirror line
  /// \param[out] theBaseEntities   list of entities to mirror
  /// \param[out] theMirrorEntities list of mirrored entities
  void getAttributes(EntityWrapperPtr& theMirrorLine,
                     std::vector<EntityWrapperPtr>& theBaseEntities,
                     std::vector<EntityWrapperPtr>& theMirrorEntities);

  /// \brief This method is used in derived objects to check consistence of constraint.
  ///        E.g. the distance between line and point may be signed.
  virtual void adjustConstraint();

private:
  size_t myNumberOfObjects;  ///< number of previously mirrored objects
};

#endif
