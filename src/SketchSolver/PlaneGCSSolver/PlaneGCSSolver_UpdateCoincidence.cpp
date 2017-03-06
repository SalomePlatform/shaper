// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_UpdateCoincidence.cpp
// Created: 17 Feb 2017
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_UpdateCoincidence.h>
#include <SketchSolver_Constraint.h>

#include <SketchPlugin_ConstraintCoincidence.h>
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
      theFeature->getKind() == SketchPlugin_ConstraintMiddle::ID()) {
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

  std::list<std::set<EntityWrapperPtr> >::iterator anIt = myCoincident.begin();
  std::list<std::set<EntityWrapperPtr> >::iterator
      aFound[2] = {myCoincident.end(), myCoincident.end()};

  for (; anIt != myCoincident.end(); ++anIt) {
    if (aFound[0] == myCoincident.end() && anIt->find(theEntity1) != anIt->end())
      aFound[0] = anIt;
    if (aFound[1] == myCoincident.end() && anIt->find(theEntity2) != anIt->end())
      aFound[1] = anIt;
    if (aFound[0] != myCoincident.end() && aFound[1] != myCoincident.end())
      break;
  }

  if (aFound[0] == myCoincident.end() && aFound[1] == myCoincident.end()) {
    // new group of coincidence
    std::set<EntityWrapperPtr> aNewCoinc;
    aNewCoinc.insert(theEntity1);
    aNewCoinc.insert(theEntity2);
    myCoincident.push_back(aNewCoinc);
  } else if (aFound[0] == aFound[1]) // same group => already coincident
    isAccepted = false;
  else {
    if (theEntity1->type() == ENTITY_POINT && theEntity2->type() == ENTITY_POINT &&
       (theEntity1->isExternal() || theEntity2->isExternal())) {
      bool hasExternal = false;
      for (int i = 0; i < 2 && !hasExternal; ++i) {
        if (aFound[i] != myCoincident.end()) {
          if (theEntity1->isExternal())
            hasExternal = hasAnotherExternalPoint(*aFound[i], theEntity1);
          if (!hasExternal && theEntity2->isExternal())
            hasExternal = hasAnotherExternalPoint(*aFound[i], theEntity2);
        }
      }
      if (hasExternal)
        isAccepted = false;
    }

    if (aFound[0] == myCoincident.end())
      aFound[1]->insert(theEntity1);
    else if (aFound[1] == myCoincident.end())
      aFound[0]->insert(theEntity2);
    else { // merge two groups
      aFound[0]->insert(aFound[1]->begin(), aFound[1]->end());
      myCoincident.erase(aFound[1]);
    }
  }

  return isAccepted;
}
