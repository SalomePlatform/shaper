// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_Solver.h
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#ifndef SolveSpaceSolver_Solver_H_
#define SolveSpaceSolver_Solver_H_

#include <SketchSolver_ISolver.h>

// Need to be defined before including SolveSpace to avoid additional dependences on Windows platform
#if defined(WIN32) && !defined(HAVE_C99_INTEGER_TYPES)
typedef unsigned int UINT32;
#else
#include <stdint.h>
#endif
#include <string.h>
#include <slvs.h>

#include <vector>

// Unknown constraint (for error reporting)
#define SLVS_C_UNKNOWN 0
// Fillet constraint identifier
#define SLVS_C_FILLET            100100
// Multi-rotation constraint identifier
#define SLVS_C_MULTI_ROTATION    100101
// Multi-translation constraint identifier
#define SLVS_C_MULTI_TRANSLATION 100102
// Unknown entity
#define SLVS_E_UNKNOWN 0
// Unknown group
#define SLVS_G_UNKNOWN 0
// Group ID to store external objects
#define SLVS_G_OUTOFGROUP 1

/** \class SolveSpaceSolver_Solver
 *  \ingroup Plugins
 *  \brief Performs high-level operations to solve sketch in SolveSpace.
 */
class SolveSpaceSolver_Solver : public SketchSolver_ISolver
{
public:
  SolveSpaceSolver_Solver();
  virtual ~SolveSpaceSolver_Solver();

  /** \brief Change array of parameters
   *  \param[in] theParameters pointer to the array of parameters
   *  \param[in] theSize       size of this array
   */
  void setParameters(Slvs_Param* theParameters, int theSize);

  /** \brief Change array of entities
   *  \param[in] theEntities pointer to the array of entities
   *  \param[in] theSize     size of this array
   */
  void setEntities(Slvs_Entity* theEntities, int theSize);

  /** \brief Change array of constraints
   *  \param[in] theConstraints pointer to the array of constraints
   *  \param[in] theSize        size of this array
   */
  void setConstraints(Slvs_Constraint* theConstraints, int theSize);

  /** \brief Store the parameters of the point which was moved by user.
   *         The solver will watch this items to be constant
   *  \param[in] theDragged list of parameters (not more than 4) which should not be changed during solving
   */
  void setDraggedParameters(const Slvs_hParam* theDragged);

  /** \brief Solve the set of equations
   *  \return identifier whether solution succeeded
   */
  virtual SketchSolver_SolveStatus solve();

  /// \brief Prepare for solving. Store initial values of parameters for undo
  virtual void prepare();

  /// \brief Revert solution to initial values
  virtual void undo();

  /// \brief Check the constraint is conflicted with others
  virtual bool isConflicting(const ConstraintID& theConstraint) const;

private:
  /// \brief Check whether degenerated arcs exist
  bool hasDegeneratedArcs() const;

private:
  Slvs_System myEquationsSystem; ///< set of equations for solving in SolveSpace
  Slvs_Param* myParamsCopy;      ///< copy of parameters
};

#endif
