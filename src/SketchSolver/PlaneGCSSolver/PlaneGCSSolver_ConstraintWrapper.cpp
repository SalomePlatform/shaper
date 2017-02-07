// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_ConstraintWrapper.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_ConstraintWrapper.h>

PlaneGCSSolver_ConstraintWrapper::PlaneGCSSolver_ConstraintWrapper(
    const GCSConstraintPtr& theConstraint,
    const SketchSolver_ConstraintType& theType)
  : myGCSConstraints(1, theConstraint),
    myType(theType)
{
  myID = CID_UNKNOWN;
}

PlaneGCSSolver_ConstraintWrapper::PlaneGCSSolver_ConstraintWrapper(
    const std::list<GCSConstraintPtr>& theConstraints,
    const SketchSolver_ConstraintType& theType)
  : myGCSConstraints(theConstraints),
    myType(theType)
{
  myID = CID_UNKNOWN;
}

void PlaneGCSSolver_ConstraintWrapper::setId(const ConstraintID& theID)
{
  myID = theID;
  std::list<GCSConstraintPtr>::iterator anIt = myGCSConstraints.begin();
  for (; anIt != myGCSConstraints.end(); ++anIt)
    (*anIt)->setTag((int)theID);
}

void PlaneGCSSolver_ConstraintWrapper::setValueParameter(const ScalarWrapperPtr& theValue)
{
  myValueParam = theValue;
}

void PlaneGCSSolver_ConstraintWrapper::setValue(const double& theValue)
{
  myValueParam->setValue(theValue);
}

double PlaneGCSSolver_ConstraintWrapper::value() const
{
  return myValueParam->value();
}
