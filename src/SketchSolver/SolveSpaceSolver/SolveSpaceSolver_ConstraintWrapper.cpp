// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_ConstraintWrapper.cpp
// Created: 2 Dec 2015
// Author:  Artem ZHIDKOV

#include <SolveSpaceSolver_ConstraintWrapper.h>
#include <SolveSpaceSolver_ConstraintType.h>
#include <math.h>

SolveSpaceSolver_ConstraintWrapper::SolveSpaceSolver_ConstraintWrapper(
    const ConstraintPtr&    theOriginal,
    const Slvs_Constraint& theConstraint)
  : mySlvsConstraint(theConstraint)
{
  myBaseConstraint = theOriginal;
  myValue = mySlvsConstraint.valA;
}

ConstraintID SolveSpaceSolver_ConstraintWrapper::id() const
{
  return (ConstraintID)mySlvsConstraint.h;
}

void SolveSpaceSolver_ConstraintWrapper::setGroup(const GroupID& theGroup)
{
  mySlvsConstraint.group = (Slvs_hGroup)theGroup;
  std::list<EntityWrapperPtr>::iterator aSubsIt = myConstrained.begin();
  for (; aSubsIt != myConstrained.end(); ++aSubsIt)
    (*aSubsIt)->setGroup(theGroup);
}

SketchSolver_ConstraintType SolveSpaceSolver_ConstraintWrapper::type() const
{
  return ConstraintType::fromSolveSpace(mySlvsConstraint.type);
}

void SolveSpaceSolver_ConstraintWrapper::setValue(const double& theValue)
{
  double aValue = theValue;
  if (type() == CONSTRAINT_RADIUS)
    aValue *= 2.0; // NOTE: SolveSpace uses constraint DIAMETER

  SketchSolver_IConstraintWrapper::setValue(aValue);
}

bool SolveSpaceSolver_ConstraintWrapper::isUsed(FeaturePtr theFeature) const
{
  std::list<EntityWrapperPtr>::const_iterator anIt = myConstrained.begin();
  for (; anIt != myConstrained.end(); ++anIt)
    if ((*anIt)->isUsed(theFeature))
      return true;
  return false;
}

bool SolveSpaceSolver_ConstraintWrapper::isUsed(AttributePtr theAttribute) const
{
  std::list<EntityWrapperPtr>::const_iterator anIt = myConstrained.begin();
  for (; anIt != myConstrained.end(); ++anIt)
    if ((*anIt)->isUsed(theAttribute))
      return true;
  return false;
}

bool SolveSpaceSolver_ConstraintWrapper::isEqual(const ConstraintWrapperPtr& theOther)
{
  const Slvs_Constraint anOtherConstraint = 
    std::dynamic_pointer_cast<SolveSpaceSolver_ConstraintWrapper>(theOther)->constraint();
  if (mySlvsConstraint.type != anOtherConstraint.type)
    return false;

  // Verify SolveSpace entities. If they are equal, no need additional checking of parameters.
  if (mySlvsConstraint.group   == anOtherConstraint.group   &&
      mySlvsConstraint.ptA     == anOtherConstraint.ptA     &&
      mySlvsConstraint.ptB     == anOtherConstraint.ptB     &&
      mySlvsConstraint.entityA == anOtherConstraint.entityA &&
      mySlvsConstraint.entityB == anOtherConstraint.entityB &&
      mySlvsConstraint.entityC == anOtherConstraint.entityC &&
      mySlvsConstraint.entityD == anOtherConstraint.entityD &&
      fabs(mySlvsConstraint.valA - anOtherConstraint.valA) < tolerance) {
    return true;
  }

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

bool SolveSpaceSolver_ConstraintWrapper::update(const ConstraintWrapperPtr& theOther)
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
