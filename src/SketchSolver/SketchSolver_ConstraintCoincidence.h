// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintCoincidence.h
// Created: 29 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintCoincidence_H_
#define SketchSolver_ConstraintCoincidence_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>
#include <SketchSolver_Storage.h>

/** \class   SketchSolver_ConstraintCoincidence
 *  \ingroup Plugins
 *  \brief   Convert coincidence constraint to SolveSpace structure
 */
class SketchSolver_ConstraintCoincidence : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintCoincidence(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myType(SLVS_C_UNKNOWN)
  {}

  virtual int getType() const
  { return myType; }

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Checks the constraint is used by current object
  virtual bool hasConstraint(ConstraintPtr theConstraint) const;

  /// \brief Return list of SketchPlugin constraints attached to this object
  virtual std::list<ConstraintPtr> constraints() const;

  /// \brief Verifies the two Coincidence constraints are intersects (have shared point)
  bool isCoincide(std::shared_ptr<SketchSolver_ConstraintCoincidence> theConstraint) const;

  /// \brief Append all data of coincidence constaint to the current
  void attach(std::shared_ptr<SketchSolver_ConstraintCoincidence> theConstraint);

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<Slvs_hEntity>& theAttributes);

private:
  /// \brief Creates new coincidence constraint
  Slvs_hConstraint addConstraint(Slvs_hEntity thePoint1, Slvs_hEntity thePoint2);

  /// \brief Create full SolveSpace structure according to given constraint
  void addConstraint(ConstraintPtr theConstraint);

  /// \brief Create constraint of point concident to the line or circle
  Slvs_hConstraint addPointOnEntity(Slvs_hEntity thePoint, Slvs_hEntity theEntity);

private:
  int myType; ///< type of constraint (applicable SLVS_C_POINTS_COINCIDENT or SLVS_C_PT_ON_LINE or SLVS_C_PT_ON_CIRCLE)
  std::map<ConstraintPtr, Slvs_hConstraint> myExtraCoincidence; ///< multiple coincidence of points
  std::set<AttributePtr> myCoincidentPoints; ///< list of points under the Coincidence constraint
};

#endif
