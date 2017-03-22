// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Defs.h
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_Defs_H_
#define PlaneGCSSolver_Defs_H_

#include <Constraints.h>
#include <Geo.h>
#include <memory>

typedef std::shared_ptr<GCS::Point>       GCSPointPtr;
typedef std::shared_ptr<GCS::Curve>       GCSCurvePtr;
typedef std::shared_ptr<GCS::Constraint>  GCSConstraintPtr;

// Tolerance for value of parameters
const double tolerance = 1.e-10;

#define PI 3.1415926535897932

// Types for data entities enumeration
typedef int ConstraintID;

// Predefined values for identifiers
const ConstraintID CID_UNKNOWN  =  0;
const ConstraintID CID_MOVEMENT = -1;

/// Types of entities
enum SketchSolver_EntityType {
  ENTITY_UNKNOWN = 0,
  ENTITY_SCALAR,
  ENTITY_ANGLE,
  ENTITY_POINT,
  ENTITY_LINE,
  ENTITY_CIRCLE,
  ENTITY_ARC
};

/// Types of constraints
enum SketchSolver_ConstraintType {
  CONSTRAINT_UNKNOWN = 0,
  CONSTRAINT_COINCIDENCE,      // base coincidence if we don't know exact type yet
  CONSTRAINT_PT_PT_COINCIDENT,
  CONSTRAINT_PT_ON_LINE,
  CONSTRAINT_PT_ON_CIRCLE,
  CONSTRAINT_MIDDLE_POINT,
  CONSTRAINT_DISTANCE,         // base distance if we don't know the measured objects yet
  CONSTRAINT_PT_PT_DISTANCE,
  CONSTRAINT_PT_LINE_DISTANCE,
  CONSTRAINT_RADIUS,
  CONSTRAINT_ANGLE,
  CONSTRAINT_FIXED,
  CONSTRAINT_HORIZONTAL,
  CONSTRAINT_VERTICAL,
  CONSTRAINT_PARALLEL,
  CONSTRAINT_PERPENDICULAR,
  CONSTRAINT_SYMMETRIC,
  CONSTRAINT_EQUAL,           // base equality if we don't know the measured objects yet
  CONSTRAINT_EQUAL_LINES,
  CONSTRAINT_EQUAL_LINE_ARC,
  CONSTRAINT_EQUAL_RADIUS,
  CONSTRAINT_TANGENT,         // base tangency if we don't know the measured objects yet
  CONSTRAINT_TANGENT_CIRCLE_LINE,
  CONSTRAINT_TANGENT_CIRCLE_CIRCLE,
  CONSTRAINT_COLLINEAR,
  CONSTRAINT_MULTI_TRANSLATION,
  CONSTRAINT_MULTI_ROTATION
};

#endif
