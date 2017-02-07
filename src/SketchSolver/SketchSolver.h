// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef SKETCHSOLVER_H
#define SKETCHSOLVER_H

#include <stdlib.h>

/// Tolerance for value of parameters
const double tolerance = 1.e-10;

#define PI 3.1415926535897932

// Types for data entities enumeration
typedef int ConstraintID;

// Predefined values for identifiers
const ConstraintID CID_UNKNOWN  =  0;
const ConstraintID CID_MOVEMENT = -1;

#endif
