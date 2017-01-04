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

typedef std::map<GCS::Constraint*, SketchSolver_ConstraintType> ConstraintMap;

/**
 * The main class that performs the high-level operations for connection to the PlaneGCS.
 */
class PlaneGCSSolver_Solver : public SketchSolver_ISolver
{
public:
  PlaneGCSSolver_Solver();
  ~PlaneGCSSolver_Solver();

  /// \brief Clear system of equations
  void clear();

  /// \brief Add constraint to the system of equations
  void addConstraint(GCSConstraintPtr theConstraint,
                     const SketchSolver_ConstraintType theType);

  /// \brief Remove constraint from the system of equations
  void removeConstraint(GCSConstraintPtr theConstraint);

  /// \brief Initialize list of unknowns
  void setParameters(const GCS::VEC_pD& theParams)
  { myParameters = theParams; }

  /// \brief Set list of IDs of tangent constraints
  ///
  /// Workaround to avoid incorrect report about redundant constraints
  /// if an arc is already smoothly connected to a line.
  void setTangent(const GCS::SET_I& theTangentIDs)
  { myTangent = theTangentIDs; }

  /** \brief Solve the set of equations
   *  \return identifier whether solution succeeded
   */
  virtual SketchSolver_SolveStatus solve();

  /// \brief Prepare for solving. Store initial values of parameters for undo
  virtual void prepare()
  { /* do nothing */ }

  /// \brief Revert solution to initial values
  virtual void undo();

  /// \brief Check the constraint is conflicted with others
  virtual bool isConflicting(const ConstraintID& theConstraint) const;

  /// \brief Degrees of freedom
  virtual int dof() const;

private:
  void collectConflicting();

  /// \brief Remove constraint from the system of equations
  void removeConstraint(GCS::Constraint* theConstraint);

  /// \brief Remove redundant tangent constraints and try to solve the system again
  GCS::SolveStatus solveWithoutTangent();

  /// \brief Check the entities under the tangent constraint are smoothly connected
  bool isTangentTruth(int theTagID) const;
  /// \brief Check the entities under the tangent constraint are smoothly connected
  bool isTangentTruth(GCS::Constraint* theTangent) const;

private:
  GCS::VEC_pD                  myParameters;     ///< list of unknowns

  /// list of constraints already processed by the system
  ConstraintMap                myConstraints;
  std::shared_ptr<GCS::System> myEquationSystem; ///< set of equations for solving in FreeGCS

  GCS::SET_I                   myConflictingIDs; ///< list of IDs of conflicting constraints

  /// specifies the conflicting constraints are already collected
  bool                         myConfCollected;

  /// list of tangent IDs to check incorrect redundant constraints
  GCS::SET_I                   myTangent;
};

#endif
