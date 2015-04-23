// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMultiRotation.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMultiRotation_H_
#define SketchSolver_ConstraintMultiRotation_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintMultiRotation
 *  \ingroup Plugins
 *  \brief   Convert rotated features to the list of SolveSpace constraints
 */
class SketchSolver_ConstraintMultiRotation : public SketchSolver_Constraint
{
public:
  SketchSolver_ConstraintMultiRotation(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myNumberOfObjects(0),
      myNumberOfCopies(0)
  {}

  virtual int getType() const
  { return SLVS_C_MULTI_ROTATION; }

  /// \brief Update constraint
  virtual void update(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove(ConstraintPtr theConstraint = ConstraintPtr());

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<Slvs_hEntity>& theAttributes)
  { /* do nothing here */ }

  /// \brief Generate list of rotated entities
  /// \param[out] theCenter   ID of central point of rotation
  /// \param[out] theAngle    rotation angle
  /// \param[out] thePoints   list of IDs of initial points and their rotated copies
  /// \param[out] theCircular list of IDs of arcs and circles and their copies
  void getAttributes(Slvs_hEntity& theCenter, double& theAngle,
                     std::vector<std::vector<Slvs_hEntity> >& thePoints,
                     std::vector<std::vector<Slvs_hEntity> >& theCircular);

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

private:
  size_t myNumberOfObjects; ///< number of previous initial objects
  size_t myNumberOfCopies;  ///< number of previous copies of initial objects
  Slvs_hEntity myRotationCenter; ///< ID of center of rotation
  double myAngle;           ///< angle of rotation
};

#endif
