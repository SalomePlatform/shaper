// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Solver.h
// Created: 14 Dec 2014
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_Solver_H_
#define PlaneGCSSolver_Solver_H_

#include <SketchSolver_ISolver.h>
#include <SketchSolver_IConstraintWrapper.h>
#include <PlaneGCSSolver_Defs.h>

#include <GCS.h>

/// \brief The main class that performs the high-level operations for connection to the PlaneGCS.
class PlaneGCSSolver_Solver : public SketchSolver_ISolver
{
public:
  PlaneGCSSolver_Solver();
  ~PlaneGCSSolver_Solver();

  /// \brief Clear system of equations
  void clear();

  /// \brief Add constraint to the system of equations
  void addConstraint(GCSConstraintPtr theConstraint);

  /// \brief Remove constraint from the system of equations
  void removeConstraint(ConstraintID theID);

  /// \brief Initialize list of unknowns
  void setParameters(const GCS::VEC_pD& theParams)
  { myParameters = theParams; }

  /// \brief Solve the set of equations
  /// \return identifier whether solution succeeded
  virtual SketchSolver_SolveStatus solve() override;

  /// \brief Prepare for solving. Store initial values of parameters for undo
  virtual void prepare() override
  { /* do nothing */ }

  /// \brief Revert solution to initial values
  virtual void undo() override;

  /// \brief Check the constraint is conflicted with others
  virtual bool isConflicting(const ConstraintID& theConstraint) const override;

  /// \brief Degrees of freedom
  virtual int dof() const override;

private:
  void collectConflicting();

private:
  GCS::VEC_pD                  myParameters;     ///< list of unknowns
  std::shared_ptr<GCS::System> myEquationSystem; ///< set of equations for solving in FreeGCS

  GCS::SET_I                   myConflictingIDs; ///< list of IDs of conflicting constraints

  /// specifies the conflicting constraints are already collected
  bool                         myConfCollected;

  /// lists of parameters used in the Equal constraints (to avoid multiple equalities)
  std::list<GCS::SET_pD>       myEqualParameters;
  /// lists of the Equal constraints
  std::map<ConstraintID, std::set<GCSConstraintPtr> > myEqualConstraints;
};

#endif
