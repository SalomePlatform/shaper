// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_ScalarWrapper.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_ParameterWrapper.h>



PlaneGCSSolver_ScalarWrapper::PlaneGCSSolver_ScalarWrapper(
    const AttributePtr              theAttribute,
    const ParameterWrapperPtr       theParam)
{
  myBaseAttribute = theAttribute;
  myParameters.assign(1, theParam);
}

void PlaneGCSSolver_ScalarWrapper::setGroup(const GroupID& theGroup)
{
  myGroup = theGroup;
  myParameters.front()->setGroup(theGroup);
}

double* PlaneGCSSolver_ScalarWrapper::scalar() const
{
  std::shared_ptr<PlaneGCSSolver_ParameterWrapper> aParam =
      std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(myParameters.front());
  return aParam->parameter();
}

bool PlaneGCSSolver_ScalarWrapper::isUsed(AttributePtr theAttribute) const
{
  return isBase(theAttribute);
}

bool PlaneGCSSolver_ScalarWrapper::isEqual(const EntityWrapperPtr& theOther)
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

bool PlaneGCSSolver_ScalarWrapper::update(const EntityWrapperPtr& theOther)
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
