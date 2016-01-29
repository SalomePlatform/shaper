// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_ConstraintType.h
// Created: 8 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef SolveSpaceSolver_ConstraintType_H_
#define SolveSpaceSolver_ConstraintType_H_

#include <SketchSolver_IConstraintWrapper.h>
#include <SolveSpaceSolver_Solver.h>

namespace ConstraintType
{
  /// \brief Convert constraint type from SketchSolver to SolveSpace
  static int toSolveSpace(SketchSolver_ConstraintType theType)
  {
    switch (theType) {
    case CONSTRAINT_PT_PT_COINCIDENT:   return SLVS_C_POINTS_COINCIDENT;
    case CONSTRAINT_PT_ON_LINE:         return SLVS_C_PT_ON_LINE;
    case CONSTRAINT_PT_ON_CIRCLE:       return SLVS_C_PT_ON_CIRCLE;
    case CONSTRAINT_MIDDLE_POINT:       return SLVS_C_AT_MIDPOINT;
    case CONSTRAINT_PT_PT_DISTANCE:     return SLVS_C_PT_PT_DISTANCE;
    case CONSTRAINT_PT_LINE_DISTANCE:   return SLVS_C_PT_LINE_DISTANCE;
    case CONSTRAINT_ANGLE:              return SLVS_C_ANGLE;
    case CONSTRAINT_RADIUS:             return SLVS_C_DIAMETER;
    case CONSTRAINT_FIXED:              return SLVS_C_WHERE_DRAGGED;
    case CONSTRAINT_HORIZONTAL:         return SLVS_C_HORIZONTAL;
    case CONSTRAINT_VERTICAL:           return SLVS_C_VERTICAL;
    case CONSTRAINT_PARALLEL:           return SLVS_C_PARALLEL;
    case CONSTRAINT_PERPENDICULAR:      return SLVS_C_PERPENDICULAR;
    case CONSTRAINT_SYMMETRIC:          return SLVS_C_SYMMETRIC_LINE;
    case CONSTRAINT_EQUAL_LINES:        return SLVS_C_EQUAL_LENGTH_LINES;
    case CONSTRAINT_EQUAL_LINE_ARC:     return SLVS_C_EQUAL_LINE_ARC_LEN;
    case CONSTRAINT_EQUAL_RADIUS:       return SLVS_C_EQUAL_RADIUS;
    case CONSTRAINT_TANGENT_ARC_LINE:   return SLVS_C_ARC_LINE_TANGENT;
    case CONSTRAINT_TANGENT_ARC_ARC:    return SLVS_C_CURVE_CURVE_TANGENT;
    case CONSTRAINT_MULTI_ROTATION:     return SLVS_C_MULTI_ROTATION;
    case CONSTRAINT_MULTI_TRANSLATION:  return SLVS_C_MULTI_TRANSLATION;
    default: break;
    }
    return SLVS_C_UNKNOWN;
  }

  /// \brief Convert constraint type from SolveSpace to SketchSolver
  static SketchSolver_ConstraintType fromSolveSpace(int theType)
  {
    switch (theType) {
    case SLVS_C_POINTS_COINCIDENT:    return CONSTRAINT_PT_PT_COINCIDENT;
    case SLVS_C_PT_ON_LINE:           return CONSTRAINT_PT_ON_LINE;
    case SLVS_C_PT_ON_CIRCLE:         return CONSTRAINT_PT_ON_CIRCLE;
    case SLVS_C_AT_MIDPOINT:          return CONSTRAINT_MIDDLE_POINT;
    case SLVS_C_PT_PT_DISTANCE:       return CONSTRAINT_PT_PT_DISTANCE;
    case SLVS_C_PT_LINE_DISTANCE:     return CONSTRAINT_PT_LINE_DISTANCE;
    case SLVS_C_EQUAL_LENGTH_LINES:   return CONSTRAINT_EQUAL_LINES;
    case SLVS_C_EQUAL_LINE_ARC_LEN:   return CONSTRAINT_EQUAL_LINE_ARC;
    case SLVS_C_SYMMETRIC_LINE:       return CONSTRAINT_SYMMETRIC;
    case SLVS_C_HORIZONTAL:           return CONSTRAINT_HORIZONTAL;
    case SLVS_C_VERTICAL:             return CONSTRAINT_VERTICAL;
    case SLVS_C_DIAMETER:             return CONSTRAINT_RADIUS;
    case SLVS_C_ANGLE:                return CONSTRAINT_ANGLE;
    case SLVS_C_PARALLEL:             return CONSTRAINT_PARALLEL;
    case SLVS_C_PERPENDICULAR:        return CONSTRAINT_PERPENDICULAR;
    case SLVS_C_ARC_LINE_TANGENT:     return CONSTRAINT_TANGENT_ARC_LINE;
    case SLVS_C_EQUAL_RADIUS:         return CONSTRAINT_EQUAL_RADIUS;
    case SLVS_C_WHERE_DRAGGED:        return CONSTRAINT_FIXED;
    case SLVS_C_CURVE_CURVE_TANGENT:  return CONSTRAINT_TANGENT_ARC_ARC;
    case SLVS_C_MULTI_ROTATION:       return CONSTRAINT_MULTI_ROTATION;
    case SLVS_C_MULTI_TRANSLATION:    return CONSTRAINT_MULTI_TRANSLATION;
    default: break;
    }
    return CONSTRAINT_UNKNOWN;
  }
}


#endif
