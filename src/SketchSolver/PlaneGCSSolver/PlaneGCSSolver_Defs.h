// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
const ConstraintID CID_FICTIVE  = 99;

/// Types of entities
enum SketchSolver_EntityType {
  ENTITY_UNKNOWN = 0,
  ENTITY_BOOLEAN,
  ENTITY_SCALAR,
  ENTITY_SCALAR_ARRAY,
  ENTITY_ANGLE,
  ENTITY_POINT,
  ENTITY_POINT_ARRAY,
  ENTITY_LINE,
  ENTITY_CIRCLE,
  ENTITY_ARC,
  ENTITY_ELLIPSE,
  ENTITY_ELLIPTIC_ARC,
  ENTITY_BSPLINE
};

/// Types of constraints
enum SketchSolver_ConstraintType {
  CONSTRAINT_UNKNOWN = 0,
  CONSTRAINT_COINCIDENCE,      // base coincidence if we don't know exact type yet
  CONSTRAINT_PT_PT_COINCIDENT,
  CONSTRAINT_PT_ON_CURVE,
  CONSTRAINT_MIDDLE_POINT,
  CONSTRAINT_DISTANCE,         // base distance if we don't know the measured objects yet
  CONSTRAINT_PT_PT_DISTANCE,
  CONSTRAINT_PT_LINE_DISTANCE,
  CONSTRAINT_HORIZONTAL_DISTANCE,
  CONSTRAINT_VERTICAL_DISTANCE,
  CONSTRAINT_RADIUS,
  CONSTRAINT_ANGLE,
  CONSTRAINT_FIXED,
  CONSTRAINT_HORIZONTAL,
  CONSTRAINT_VERTICAL,
  CONSTRAINT_PARALLEL,
  CONSTRAINT_PERPENDICULAR,
  CONSTRAINT_PERPENDICULAR_CURVES,
  CONSTRAINT_SYMMETRIC,
  CONSTRAINT_EQUAL,           // base equality if we don't know the measured objects yet
  CONSTRAINT_EQUAL_LINES,
  CONSTRAINT_EQUAL_LINE_ARC,
  CONSTRAINT_EQUAL_RADIUS,
  CONSTRAINT_EQUAL_ELLIPSES,
  CONSTRAINT_TANGENT,         // base tangency if we don't know the measured objects yet
  CONSTRAINT_TANGENT_CIRCLE_LINE,
  CONSTRAINT_TANGENT_CURVE_CURVE,
  CONSTRAINT_COLLINEAR,
  CONSTRAINT_MULTI_TRANSLATION,
  CONSTRAINT_MULTI_ROTATION
};

#endif
