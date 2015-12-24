// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ISolver.h
// Created: 30 Nov 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ISolver_H_
#define SketchSolver_ISolver_H_

#include <SketchSolver.h>

#include <memory>

/// The result of constraints solution
enum SketchSolver_SolveStatus {
  STATUS_OK,
  STATUS_INCONSISTENT,
  STATUS_EMPTYSET,
  STATUS_FAILED, // set if no one other status is applicable
  STATUS_UNKNOWN // set for newly created groups
};


/**
 *  Interface providing operations to solve sketches.
 */
class SketchSolver_ISolver
{
public:
  virtual ~SketchSolver_ISolver() {}

  /// \brief Changes the ID of the group to solve
  void setGroup(const GroupID& theGroupID)
  { myGroup = theGroupID; }

  /// \brief Set or unset the flag which allows to find all failed constraints
  void calculateFailedConstraints(bool theSic)
  { myFindFaileds = theSic; }

  /// \brief Prepare for solving. Store initial values of parameters for undo
  virtual void prepare() = 0;

  /// \brief Solve the set of equations
  /// \return identifier whether solution succeeded
  virtual SketchSolver_SolveStatus solve() = 0;

  /// \brief Revert solution to initial values
  virtual void undo() = 0;

protected:
  GroupID myGroup;       ///< ID of the group to be solved
  bool    myFindFaileds; ///< flag to find conflicting or inappropriate constraints
};

typedef std::shared_ptr<SketchSolver_ISolver> SolverPtr;

#endif
