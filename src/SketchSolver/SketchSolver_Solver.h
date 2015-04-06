// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Solver.h
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Solver_H_
#define SketchSolver_Solver_H_

#include "SketchSolver.h"

// Need to be defined before including SolveSpace to avoid additional dependances on Windows platform
#if defined(WIN32) && !defined(HAVE_C99_INTEGER_TYPES)
typedef unsigned int UINT32;
#else
#include <stdint.h>
#endif
#include <string.h>
#include <slvs.h>

#include <vector>

#define SLVS_RESULT_EMPTY_SET -1

// Unknown constraint (for error reporting)
#define SLVS_C_UNKNOWN 0
// Fillet constraint identifier
#define SLVS_C_FILLET 100100
// Unknown entity
#define SLVS_E_UNKNOWN 0
// Unknown group
#define SLVS_G_UNKNOWN 0

/**
 * The main class that performs the high-level operations for connection to the SolveSpace.
 */
class SketchSolver_Solver
{
 public:
  SketchSolver_Solver();
  ~SketchSolver_Solver();

  /** \brief Initialize the ID of the group
   */
  inline void setGroupID(Slvs_hGroup theGroupID)
  {
    myGroupID = theGroupID;
  }

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
  int solve();

  /** \brief Updates the list of parameters by calculated values
   *  \param[in,out] theParameters parameters to be updated
   *  \return \c true if parameters are updated correctly
   */
  bool getResult(std::vector<Slvs_Param>& theParameters);

 private:
  Slvs_hGroup myGroupID;         ///< identifier of the group to be solved
  Slvs_System myEquationsSystem;  ///< set of equations for solving in SolveSpace
};

#endif
