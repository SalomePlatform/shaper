// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Defs.h
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_Defs_H_
#define PlaneGCSSolver_Defs_H_

#include <SketchSolver.h>

#include <Constraints.h>
#include <Geo.h>
#include <memory>

typedef std::shared_ptr<GCS::Point>       GCSPointPtr;
typedef std::shared_ptr<GCS::Curve>       GCSCurvePtr;
typedef std::shared_ptr<GCS::Constraint>  GCSConstraintPtr;

#endif
