// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Solver.h
// Created: 14 Dec 2014
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_Solver_H_
#define PlaneGCSSolver_Solver_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_ConstraintWrapper.h>

#include <GCS.h>

/// \brief The main class that performs the high-level operations for connection to the PlaneGCS.
class PlaneGCSSolver_Solver
{
public:
  /// The result of constraints solution
  enum SolveStatus {
    STATUS_OK,
    STATUS_INCONSISTENT,
    STATUS_EMPTYSET,
    STATUS_FAILED, // set if no one other status is applicable
    STATUS_UNKNOWN // set for newly created groups
  };

  PlaneGCSSolver_Solver();
  ~PlaneGCSSolver_Solver();

  /// \brief Clear system of equations
  void clear();

  /// \brief Add constraint to the system of equations
  void addConstraint(GCSConstraintPtr theConstraint);

  /// \brief Remove constraint from the system of equations
  void removeConstraint(ConstraintID theID);

  /// \brief Initialize memory for new solver's parameter
  double* createParameter();
  /// \brief Release memory occupied by parameters
  void removeParameters(const GCS::SET_pD& theParams);

  /// \brief Solve the set of equations
  /// \return identifier whether solution succeeded
  SolveStatus solve();

  /// \brief Revert solution to initial values
  void undo();

  /// \brief Check the constraint is conflicted with others
  bool isConflicting(const ConstraintID& theConstraint) const;

  /// \brief Check conflicting/redundant constraints and DoF
  void diagnose();

  /// \brief Degrees of freedom
  int dof();

private:
  void collectConflicting();

private:
  typedef std::map<ConstraintID, std::set<GCSConstraintPtr> > ConstraintMap;

  GCS::VEC_pD                  myParameters;     ///< list of unknowns
  ConstraintMap                myConstraints;    ///< list of constraints

  std::shared_ptr<GCS::System> myEquationSystem; ///< set of equations for solving in FreeGCS
  bool                         myDiagnoseBeforeSolve; ///< is the diagnostic necessary

  GCS::SET_I                   myConflictingIDs; ///< list of IDs of conflicting constraints
  /// specifies the conflicting constraints are already collected
  bool                         myConfCollected;

  int                          myDOF;            ///< degrees of freedom
};

typedef std::shared_ptr<PlaneGCSSolver_Solver> SolverPtr;

#endif
