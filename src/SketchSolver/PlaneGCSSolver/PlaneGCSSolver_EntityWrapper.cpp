// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_EntityWrapper.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_EntityWrapper.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>

PlaneGCSSolver_EntityWrapper::PlaneGCSSolver_EntityWrapper(
    const FeaturePtr theFeature, const GCSCurvePtr theEntity)
  : myEntity(theEntity),
    myID(EID_UNKNOWN)
{
  myBaseFeature = theFeature;

  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(myEntity);
  if (aLine) myType = ENTITY_LINE;
  else {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(myEntity);
    if (anArc) myType = ENTITY_ARC;
    else {
      std::shared_ptr<GCS::Circle> aCircle = std::dynamic_pointer_cast<GCS::Circle>(myEntity);
      if (aCircle) myType = ENTITY_CIRCLE;
    }
  }

  // empty entity, probably this is a SketchPlugin_Point or SketchPlugin_Sketch
  if (theFeature->getKind() == SketchPlugin_Point::ID())
    myType = ENTITY_POINT;
  else if (theFeature->getKind() == SketchPlugin_Sketch::ID())
    myType = ENTITY_SKETCH;
}

void PlaneGCSSolver_EntityWrapper::setGroup(const GroupID& theGroup)
{
  myGroup = theGroup;
  std::list<EntityWrapperPtr>::iterator aSubsIt = mySubEntities.begin();
  for (; aSubsIt != mySubEntities.end(); ++aSubsIt)
    (*aSubsIt)->setGroup(theGroup);
  std::list<ParameterWrapperPtr>::iterator aPIt = myParameters.begin();
  for (; aPIt != myParameters.end(); ++aPIt)
    (*aPIt)->setGroup(theGroup);
}

bool PlaneGCSSolver_EntityWrapper::isUsed(FeaturePtr theFeature) const
{
  if (isBase(theFeature))
    return true;

  std::list<EntityWrapperPtr>::const_iterator anIt = mySubEntities.begin();
  for (; anIt != mySubEntities.end(); ++anIt)
    if ((*anIt)->isUsed(theFeature))
      return true;
  return false;
}

bool PlaneGCSSolver_EntityWrapper::isUsed(AttributePtr theAttribute) const
{
  if (isBase(theAttribute))
    return true;

  std::list<EntityWrapperPtr>::const_iterator anIt = mySubEntities.begin();
  for (; anIt != mySubEntities.end(); ++anIt)
    if ((*anIt)->isUsed(theAttribute))
      return true;
  return false;
}

bool PlaneGCSSolver_EntityWrapper::isEqual(const EntityWrapperPtr& theOther)
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

bool PlaneGCSSolver_EntityWrapper::update(const EntityWrapperPtr& theOther)
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
