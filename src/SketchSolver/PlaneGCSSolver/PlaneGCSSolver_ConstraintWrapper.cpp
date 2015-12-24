// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_ConstraintWrapper.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_ConstraintWrapper.h>

#include <math.h>

PlaneGCSSolver_ConstraintWrapper::PlaneGCSSolver_ConstraintWrapper(
    const ConstraintPtr& theOriginal,
    const GCSConstraintPtr& theConstraint,
    const SketchSolver_ConstraintType& theType)
  : myGCSConstraints(1, theConstraint),
    myType(theType),
    myID(CID_UNKNOWN)
{
  myBaseConstraint = theOriginal;
  myValue = 0.0;
}

PlaneGCSSolver_ConstraintWrapper::PlaneGCSSolver_ConstraintWrapper(
    const ConstraintPtr& theOriginal,
    const std::list<GCSConstraintPtr>& theConstraints,
    const SketchSolver_ConstraintType& theType)
  : myGCSConstraints(theConstraints),
    myType(theType),
    myID(CID_UNKNOWN)
{
  myBaseConstraint = theOriginal;
  myValue = 0.0;
}

void PlaneGCSSolver_ConstraintWrapper::setValueParameter(const ParameterWrapperPtr& theValue)
{
  myValueParam = theValue;
  myValue = myValueParam->value();
}

void PlaneGCSSolver_ConstraintWrapper::setValue(const double& theValue)
{
  myValue = theValue;
  myValueParam->setValue(theValue);
}


void PlaneGCSSolver_ConstraintWrapper::setGroup(const GroupID& theGroup)
{
  myGroup = theGroup;
  std::list<EntityWrapperPtr>::iterator aSubsIt = myConstrained.begin();
  for (; aSubsIt != myConstrained.end(); ++aSubsIt)
    (*aSubsIt)->setGroup(theGroup);
}

bool PlaneGCSSolver_ConstraintWrapper::isUsed(FeaturePtr theFeature) const
{
  std::list<EntityWrapperPtr>::const_iterator anIt = myConstrained.begin();
  for (; anIt != myConstrained.end(); ++anIt)
    if ((*anIt)->isUsed(theFeature))
      return true;
  return false;
}

bool PlaneGCSSolver_ConstraintWrapper::isUsed(AttributePtr theAttribute) const
{
  std::list<EntityWrapperPtr>::const_iterator anIt = myConstrained.begin();
  for (; anIt != myConstrained.end(); ++anIt)
    if ((*anIt)->isUsed(theAttribute))
      return true;
  return false;
}

bool PlaneGCSSolver_ConstraintWrapper::isEqual(const ConstraintWrapperPtr& theOther)
{
  if (type() != theOther->type())
    return false;

  // Verify equality of values
  if (fabs(myValue - theOther->value()) > tolerance)
    return false;

  // Verify equality of entities
  const std::list<EntityWrapperPtr>& anOtherSubs = theOther->entities();
  if (myConstrained.size() != anOtherSubs.size())
    return false;
  std::list<EntityWrapperPtr>::const_iterator aMySubsIt = myConstrained.begin();
  std::list<EntityWrapperPtr>::const_iterator anOtherSubsIt = anOtherSubs.begin();
  for (; aMySubsIt != myConstrained.end(); ++aMySubsIt, ++anOtherSubsIt)
    if (!(*aMySubsIt)->isEqual(*anOtherSubsIt))
      return false;
  return true;
}

bool PlaneGCSSolver_ConstraintWrapper::update(const ConstraintWrapperPtr& theOther)
{
  bool isUpdated = false;

  std::list<EntityWrapperPtr> aMySubs = entities();
  std::list<EntityWrapperPtr> anOtherSubs = theOther->entities();
  std::list<EntityWrapperPtr>::const_iterator aMySubsIt = aMySubs.begin();
  std::list<EntityWrapperPtr>::const_iterator anOtherSubsIt = anOtherSubs.begin();
  for (; aMySubsIt != aMySubs.end() && anOtherSubsIt != anOtherSubs.end();
       ++aMySubsIt, ++anOtherSubsIt)
     isUpdated = (*aMySubsIt)->update(*anOtherSubsIt) || isUpdated;

  if (fabs(value() - theOther->value()) > tolerance) {
    myValue = theOther->value();
    isUpdated = true;
  }
  return isUpdated;
}
