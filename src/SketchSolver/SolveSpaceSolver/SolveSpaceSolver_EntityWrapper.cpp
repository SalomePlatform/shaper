// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_EntityWrapper.cpp
// Created: 2 Dec 2015
// Author:  Artem ZHIDKOV

#include <SolveSpaceSolver_EntityWrapper.h>

SolveSpaceSolver_EntityWrapper::SolveSpaceSolver_EntityWrapper(
    const FeaturePtr theFeature, const Slvs_Entity& theEntity)
  : myEntity(theEntity)
{
  myBaseFeature = theFeature;
}

SolveSpaceSolver_EntityWrapper::SolveSpaceSolver_EntityWrapper(
    const AttributePtr theAttribute, const Slvs_Entity& theEntity)
  : myEntity(theEntity)
{
  myBaseAttribute = theAttribute;
}

SolveSpaceSolver_EntityWrapper::SolveSpaceSolver_EntityWrapper(
    const FeaturePtr thePointFeature,
    const AttributePtr thePointAttribute,
    const Slvs_Entity& theEntity)
  : myEntity(theEntity)
{
  myBaseFeature = thePointFeature;
  myBaseAttribute = thePointAttribute;
}


EntityID SolveSpaceSolver_EntityWrapper::id() const
{
  return (EntityID)myEntity.h;
}

void SolveSpaceSolver_EntityWrapper::setGroup(const GroupID& theGroup)
{
  myEntity.group = (Slvs_hGroup)theGroup;
  std::list<EntityWrapperPtr>::iterator aSubsIt = mySubEntities.begin();
  for (; aSubsIt != mySubEntities.end(); ++aSubsIt)
    (*aSubsIt)->setGroup(theGroup);
  std::list<ParameterWrapperPtr>::iterator aPIt = myParameters.begin();
  for (; aPIt != myParameters.end(); ++aPIt)
    (*aPIt)->setGroup(theGroup);
}

SketchSolver_EntityType SolveSpaceSolver_EntityWrapper::type() const
{
  switch (myEntity.type) {
  case SLVS_E_POINT_IN_3D:
  case SLVS_E_POINT_IN_2D:    return ENTITY_POINT;
  case SLVS_E_LINE_SEGMENT:   return ENTITY_LINE;
  case SLVS_E_CIRCLE:         return ENTITY_CIRCLE;
  case SLVS_E_ARC_OF_CIRCLE:  return ENTITY_ARC;
  case SLVS_E_NORMAL_IN_3D:
  case SLVS_E_NORMAL_IN_2D:   return ENTITY_NORMAL;
  case SLVS_E_DISTANCE:       return ENTITY_SCALAR;
  case SLVS_E_WORKPLANE:      return ENTITY_SKETCH;
  default: break;
  }
  return ENTITY_UNKNOWN;
}

bool SolveSpaceSolver_EntityWrapper::isUsed(FeaturePtr theFeature) const
{
  if (isBase(theFeature))
    return true;

  std::list<EntityWrapperPtr>::const_iterator anIt = mySubEntities.begin();
  for (; anIt != mySubEntities.end(); ++anIt)
    if ((*anIt)->isUsed(theFeature))
      return true;
  return false;
}

bool SolveSpaceSolver_EntityWrapper::isUsed(AttributePtr theAttribute) const
{
  if (isBase(theAttribute))
    return true;

  std::list<EntityWrapperPtr>::const_iterator anIt = mySubEntities.begin();
  for (; anIt != mySubEntities.end(); ++anIt)
    if ((*anIt)->isUsed(theAttribute))
      return true;
  return false;
}

bool SolveSpaceSolver_EntityWrapper::isEqual(const EntityWrapperPtr& theOther)
{
  if (type() != theOther->type())
    return false;

  // Verify Equality of sub-entities
  const std::list<EntityWrapperPtr>& anOtherSubs = theOther->subEntities();
  if (mySubEntities.size() != anOtherSubs.size())
    return false;
  std::list<EntityWrapperPtr>::const_iterator aMySubsIt = mySubEntities.begin();
  std::list<EntityWrapperPtr>::const_iterator anOtherSubsIt = anOtherSubs.begin();
  for (; aMySubsIt != mySubEntities.end(); ++aMySubsIt, ++anOtherSubsIt)
    if (!(*aMySubsIt)->isEqual(*anOtherSubsIt))
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

bool SolveSpaceSolver_EntityWrapper::update(const EntityWrapperPtr& theOther)
{
  bool isUpdated = false;

  std::list<EntityWrapperPtr> aMySubs = subEntities();
  std::list<EntityWrapperPtr> anOtherSubs = theOther->subEntities();
  std::list<EntityWrapperPtr>::const_iterator aMySubsIt = aMySubs.begin();
  std::list<EntityWrapperPtr>::const_iterator anOtherSubsIt = anOtherSubs.begin();
  for (; aMySubsIt != aMySubs.end() && anOtherSubsIt != anOtherSubs.end();
       ++aMySubsIt, ++anOtherSubsIt)
     isUpdated = (*aMySubsIt)->update(*anOtherSubsIt) || isUpdated;

  std::list<ParameterWrapperPtr> aMyParams = parameters();
  std::list<ParameterWrapperPtr> anOtherParams = theOther->parameters();
  std::list<ParameterWrapperPtr>::const_iterator aMyParIt = aMyParams.begin();
  std::list<ParameterWrapperPtr>::const_iterator anOtherParIt = anOtherParams.begin();
  for (; aMyParIt != aMyParams.end() && anOtherParIt != anOtherParams.end();
      ++aMyParIt, ++anOtherParIt)
    isUpdated = (*aMyParIt)->update(*anOtherParIt);
  return isUpdated;
}
