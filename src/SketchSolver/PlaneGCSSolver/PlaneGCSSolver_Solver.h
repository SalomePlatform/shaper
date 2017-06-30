// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
  /// \brief Add parameters created elsewhere
  void addParameters(const GCS::SET_pD& theParams);
  /// \brief Release memory occupied by parameters
  void removeParameters(const GCS::SET_pD& theParams);

  /// \brief Preliminary initialization of solver (useful for moving a feature).
  ///        When called, the solve() method does not reinitialize a set of constraints.
  void initialize();

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

  /// \brief Add fictive constraint if the sketch contains temporary constraints only
  void addFictiveConstraintIfNecessary();
  /// \brief Remove previously added fictive constraint
  void removeFictiveConstraint();

private:
  typedef std::map<ConstraintID, std::set<GCSConstraintPtr> > ConstraintMap;

  GCS::VEC_pD                  myParameters;     ///< list of unknowns
  ConstraintMap                myConstraints;    ///< list of constraints

  std::shared_ptr<GCS::System> myEquationSystem; ///< set of equations for solving in FreeGCS
  bool                         myDiagnoseBeforeSolve; ///< is the diagnostic necessary
  bool                         myInitilized;     ///< is the system already initialized

  GCS::SET_I                   myConflictingIDs; ///< list of IDs of conflicting constraints
  /// specifies the conflicting constraints are already collected
  bool                         myConfCollected;

  int                          myDOF;            ///< degrees of freedom

  GCS::Constraint*             myFictiveConstraint;
};

typedef std::shared_ptr<PlaneGCSSolver_Solver> SolverPtr;

#endif
