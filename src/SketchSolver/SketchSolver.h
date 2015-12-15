// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef SKETCHSOLVER_H
#define SKETCHSOLVER_H

#include <stdlib.h>

#if defined SKETCHSOLVER_EXPORTS
#if defined WIN32
#define SKETCHSOLVER_EXPORT              __declspec( dllexport )
#else
#define SKETCHSOLVER_EXPORT
#endif
#else
#if defined WIN32
#define SKETCHSOLVER_EXPORT              __declspec( dllimport )
#else
#define SKETCHSOLVER_EXPORT
#endif
#endif

/// Tolerance for value of parameters
const double tolerance = 1.e-10;

#define PI 3.1415926535897932

// Types for data entities enumeration
typedef size_t GroupID;
typedef size_t ParameterID;
typedef size_t EntityID;
typedef size_t ConstraintID;

// Predefined values for identifiers
const GroupID       GID_UNKNOWN    = 0;
const GroupID       GID_OUTOFGROUP = 1;

const ParameterID   PID_UNKNOWN    = 0;
const EntityID      EID_UNKNOWN    = 0;
const ConstraintID  CID_UNKNOWN    = 0;

#endif
