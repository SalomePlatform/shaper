// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_PointWrapper.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_PointWrapper.h>

PlaneGCSSolver_PointWrapper::PlaneGCSSolver_PointWrapper(
    const AttributePtr theAttribute, const GCSPointPtr thePoint)
  : myPoint(thePoint),
    myID(EID_UNKNOWN)
{
  myBaseAttribute = theAttribute;
}

void PlaneGCSSolver_PointWrapper::setGroup(const GroupID& theGroup)
{
  myGroup = theGroup;
  std::list<EntityWrapperPtr>::iterator aSubsIt = mySubEntities.begin();
  for (; aSubsIt != mySubEntities.end(); ++aSubsIt)
    (*aSubsIt)->setGroup(theGroup);
  std::list<ParameterWrapperPtr>::iterator aPIt = myParameters.begin();
  for (; aPIt != myParameters.end(); ++aPIt)
    (*aPIt)->setGroup(theGroup);
}

bool PlaneGCSSolver_PointWrapper::isUsed(AttributePtr theAttribute) const
{
  return isBase(theAttribute);
}

bool PlaneGCSSolver_PointWrapper::isEqual(const EntityWrapperPtr& theOther)
{
  if (type() != theOther->type())
    return false;

  // Verify equality of parameters
  const std::list<ParameterWrapperPtr>& anOtherParams = theOther->parameters();
  if (myParameters.size() != anOtherParams.size())
    return false;
  std::list<ParameterWrapperPtr>::const_iterator aMyIt = myParameters.begin();
  std::list<ParameterWrapperPtr>::const_iterator anOtherIt = anOtherParams.begin();
  for (; aMyIt != myParameters.end(); ++aMyIt, ++anOtherIt)
    if (!(*aMyIt)->isEqual(*anOtherIt))
      return false;
  return true;
}

bool PlaneGCSSolver_PointWrapper::update(const EntityWrapperPtr& theOther)
{
  bool isUpdated = false;
  std::list<ParameterWrapperPtr> aMyParams = parameters();
  std::list<ParameterWrapperPtr> anOtherParams = theOther->parameters();
  std::list<ParameterWrapperPtr>::const_iterator aMyParIt = aMyParams.begin();
  std::list<ParameterWrapperPtr>::const_iterator anOtherParIt = anOtherParams.begin();
  for (; aMyParIt != aMyParams.end() && anOtherParIt != anOtherParams.end();
      ++aMyParIt, ++anOtherParIt)
    isUpdated = (*aMyParIt)->update(*anOtherParIt);
  return isUpdated;
}
