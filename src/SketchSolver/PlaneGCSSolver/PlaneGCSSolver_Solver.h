// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Solver.h
// Created: 14 Dec 2014
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_Solver_H_
#define PlaneGCSSolver_Solver_H_

#include <SketchSolver_ISolver.h>
#include <PlaneGCSSolver_Defs.h>

////// Need to be defined before including SolveSpace to avoid additional dependences on Windows platform
////#if defined(WIN32) && !defined(HAVE_C99_INTEGER_TYPES)
////typedef unsigned int UINT32;
////#else
////#include <stdint.h>
////#endif
////#include <string.h>
#include <GCS.h>

////#include <vector>

/**
 * The main class that performs the high-level operations for connection to the PlaneGCS.
 */
class PlaneGCSSolver_Solver : public SketchSolver_ISolver
{
public:
  PlaneGCSSolver_Solver() {}
  ~PlaneGCSSolver_Solver();

  /// \brief Clear system of equations
  void clear();

  /// \brief Add constraint to the system of equations
  void addConstraint(GCSConstraintPtr theConstraint);

  /// \brief Remove constraint from the system of equations
  void removeConstraint(GCSConstraintPtr theConstraint);

  /// \brief Initialize list of unknowns
  void setParameters(const GCS::VEC_pD& theParams)
  { myParameters = theParams; }

  /** \brief Solve the set of equations
   *  \return identifier whether solution succeeded
   */
  virtual SketchSolver_SolveStatus solve();

private:
  GCS::VEC_pD                myParameters;     ///< list of unknowns
  std::set<GCS::Constraint*> myConstraints;    ///< list of constraints already processed by the system
  GCS::System                myEquationSystem; ///< set of equations for solving in FreeGCS
};

#endif
