// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_UpdateCoincidence.cpp
// Created: 17 Feb 2017
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_UpdateCoincidence.h>
#include <PlaneGCSSolver_EntityWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <SketchSolver_Constraint.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintMiddle.h>

void PlaneGCSSolver_UpdateCoincidence::attach(SketchSolver_Constraint* theObserver,
                                              const std::string& theType)
{
  if (theType == GROUP()) {
    std::list<SketchSolver_Constraint*>::iterator aPlaceToAdd = myObservers.end();
    // point-point coincidence is placed first
    if (theObserver->getType() == CONSTRAINT_PT_PT_COINCIDENT) {
      for (aPlaceToAdd = myObservers.begin(); aPlaceToAdd != myObservers.end(); ++aPlaceToAdd)
        if ((*aPlaceToAdd)->getType() != CONSTRAINT_PT_PT_COINCIDENT)
          break;
    }
    myObservers.insert(aPlaceToAdd, theObserver);
  } else
    myNext->attach(theObserver, theType);
}

void PlaneGCSSolver_UpdateCoincidence::update(const FeaturePtr& theFeature)
{
  if (theFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID() ||
      theFeature->getKind() == SketchPlugin_ConstraintMiddle::ID() ||
      theFeature->getKind() == SketchPlugin_ConstraintCollinear::ID()) {
    myCoincident.clear();
    // notify listeners and stop procesing
    std::list<SketchSolver_Constraint*>::iterator anIt = myObservers.begin();
    for (; anIt != myObservers.end(); ++anIt)
      (*anIt)->notify(theFeature, this);
  } else
    myNext->update(theFeature);
}

static bool hasAnotherExternalPoint(const std::set<EntityWrapperPtr>& theCoincidences,
                                    const EntityWrapperPtr& thePoint)
{
  std::set<EntityWrapperPtr>::const_iterator anIt = theCoincidences.begin();
  for (; anIt != theCoincidences.end(); ++anIt)
    if ((*anIt)->type() == ENTITY_POINT && (*anIt)->isExternal() && *anIt != thePoint)
      return true;
  return false;
}

bool PlaneGCSSolver_UpdateCoincidence::checkCoincidence(
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2)
{
  bool isAccepted = true;

  std::list<CoincidentEntities>::iterator anIt = myCoincident.begin();
  std::list<CoincidentEntities>::iterator
      aFound[2] = {myCoincident.end(), myCoincident.end()};

  for (; anIt != myCoincident.end(); ++anIt) {
    if (anIt->isExist(theEntity1))
      aFound[0] = anIt;
    if (anIt->isExist(theEntity2))
      aFound[1] = anIt;
    if (aFound[0] != myCoincident.end() && aFound[1] != myCoincident.end())
      break;
  }

  if (aFound[0] == myCoincident.end() && aFound[1] == myCoincident.end()) {
    // new group of coincidence
    myCoincident.push_back(CoincidentEntities(theEntity1, theEntity2));
  } else if (aFound[0] == aFound[1]) // same group => already coincident
    isAccepted = false;
  else {
    if (aFound[0] == myCoincident.end())
      isAccepted = aFound[1]->isNewCoincidence(theEntity2, theEntity1);
    else if (aFound[1] == myCoincident.end())
      isAccepted = aFound[0]->isNewCoincidence(theEntity1, theEntity2);
    else { // merge two groups
      isAccepted = aFound[0]->isNewCoincidence(theEntity1, *(aFound[1]), theEntity2);
      myCoincident.erase(aFound[1]);
    }
  }

  return isAccepted;
}

bool PlaneGCSSolver_UpdateCoincidence::isPointOnEntity(
    const EntityWrapperPtr& thePoint,
    const EntityWrapperPtr& theEntity)
{
  std::list<CoincidentEntities>::iterator anIt = myCoincident.begin();
  for (; anIt != myCoincident.end(); ++anIt)
    if (anIt->isExist(thePoint))
      break;

  if (anIt == myCoincident.end())
    return false;

  if (anIt->isExist(theEntity))
    return true;

  if (theEntity->type() == ENTITY_LINE) {
    std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(
        std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(theEntity)->entity());
    return anIt->isExist(aLine->p1) || anIt->isExist(aLine->p2);
  }
  return false;
}





PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::CoincidentEntities(
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2)
{
  if (theEntity1->isExternal() && theEntity2->isExternal()) {
    myExternalAndConnected[theEntity1] = std::set<EntityWrapperPtr>();
    myExternalAndConnected[theEntity2] = std::set<EntityWrapperPtr>();
  } else if (theEntity1->isExternal())
    myExternalAndConnected[theEntity1].insert(theEntity2);
  else if (theEntity2->isExternal())
    myExternalAndConnected[theEntity2].insert(theEntity1);
  else {
    std::set<EntityWrapperPtr> aGroup;
    aGroup.insert(theEntity1);
    aGroup.insert(theEntity2);
    myExternalAndConnected[EntityWrapperPtr()] = aGroup;
  }
}

bool PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::hasExternal() const
{
  return myExternalAndConnected.size() != 1 ||
         myExternalAndConnected.find(EntityWrapperPtr()) == myExternalAndConnected.end();
}

bool PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::isExist(
    const EntityWrapperPtr& theEntity) const
{
  std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> >::const_iterator
      anIt = myExternalAndConnected.begin();
  for (; anIt != myExternalAndConnected.end(); ++anIt)
    if (anIt->first == theEntity ||
        anIt->second.find(theEntity) != anIt->second.end())
      return true;
  return false;
}

static bool isEqual(const GCS::Point& thePoint1, const GCS::Point& thePoint2)
{
  return thePoint1.x == thePoint2.x && thePoint1.y == thePoint2.y;
}

bool PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::isExist(
    const GCS::Point& thePoint) const
{
  std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> >::const_iterator
      anIt = myExternalAndConnected.begin();
  for (; anIt != myExternalAndConnected.end(); ++anIt) {
    if (anIt->first && anIt->first->type() == ENTITY_POINT) {
      const GCSPointPtr& aPoint =
          std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->first)->point();
      if (isEqual(*aPoint, thePoint))
        return true;
    }

    std::set<EntityWrapperPtr>::const_iterator anEntIt = anIt->second.begin();
    for (; anEntIt != anIt->second.end(); ++anEntIt)
      if ((*anEntIt)->type() == ENTITY_POINT) {
        const GCSPointPtr& aPoint =
            std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(*anEntIt)->point();
        if (isEqual(*aPoint, thePoint))
          return true;
      }
  }
  return false;
}

bool PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::isNewCoincidence(
    const EntityWrapperPtr& theEntityExist,
    const EntityWrapperPtr& theOtherEntity)
{
  if (theOtherEntity->isExternal()) {
    if (hasExternal()) {
      if (myExternalAndConnected.find(theOtherEntity) == myExternalAndConnected.end())
        myExternalAndConnected[theOtherEntity] = std::set<EntityWrapperPtr>();
      return false;
    } else {
      myExternalAndConnected[theOtherEntity] = myExternalAndConnected[EntityWrapperPtr()];
      myExternalAndConnected.erase(EntityWrapperPtr());
      return true;
    }
  }

  if (theEntityExist->isExternal()) {
    myExternalAndConnected[theEntityExist].insert(theOtherEntity);
    return true;
  }

  std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> >::iterator
      anIt = myExternalAndConnected.begin();
  for (; anIt != myExternalAndConnected.end(); ++anIt)
    if (anIt->second.find(theEntityExist) != anIt->second.end()) {
      anIt->second.insert(theOtherEntity);
      break;
    }
  return true;
}

bool PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::isNewCoincidence(
    const EntityWrapperPtr&   theEntityExist,
    const CoincidentEntities& theOtherGroup,
    const EntityWrapperPtr&   theEntityInOtherGroup)
{
  bool hasExt[2] = {hasExternal(), theOtherGroup.hasExternal()};
  if (hasExt[0] && hasExt[1]) {
    myExternalAndConnected.insert(theOtherGroup.myExternalAndConnected.begin(),
                                  theOtherGroup.myExternalAndConnected.end());
    return false;
  } else if (!hasExt[0] && !hasExt[1]) {
    std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> >::const_iterator
        aFound = theOtherGroup.myExternalAndConnected.find(EntityWrapperPtr());

    myExternalAndConnected[EntityWrapperPtr()].insert(
        aFound->second.begin(), aFound->second.end());
    return true;
  } else {
    std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> > aSource, aDest;
    EntityWrapperPtr aTarget;
    if (hasExt[0]) {
      aDest = myExternalAndConnected;
      aSource = theOtherGroup.myExternalAndConnected;
      aTarget = theEntityExist;
    } else {
      aSource = myExternalAndConnected;
      aDest = theOtherGroup.myExternalAndConnected;
      aTarget = theEntityInOtherGroup;
    }

    std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> >::const_iterator
        aFound = aSource.find(EntityWrapperPtr());

    std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> >::iterator anIt = aDest.begin();
    for (; anIt != aDest.end(); ++anIt)
      if (anIt->first == aTarget ||
          anIt->second.find(aTarget) != anIt->second.end()) {
        anIt->second.insert(aFound->second.begin(), aFound->second.end());
        break;
      }
    return true;
  }
  // impossible case
  return false;
}
