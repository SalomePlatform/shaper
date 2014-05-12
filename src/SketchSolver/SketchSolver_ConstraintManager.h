// File:    SketchSolver_ConstraintManager.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintManager_Headerfile
#define SketchSolver_ConstraintManager_Headerfile

#include "SketchSolver.h"

/** \class   SketchSolver_ConstraintManager
 *  \ingroup DataModel
 *  \brief   Transforms the Constraint feature into the format understandable by SolveSpace library.
 *
 *  Constraints created for SolveSpace library will be divided into the groups.
 *  The division order based on connectedness of the features by the constraints.
 *  The groups may be fused or separated according to the new constraints.
 */
class SketchSolver_ConstraintManager
{
private:
  class SketchSolver_ConstraintGroup;
};

class SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup
{
};

#endif
