// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_UpdateCoincidence.cpp
// Created: 17 Feb 2017
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_UpdateCoincidence.h>
#include <SketchSolver_Constraint.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintMiddle.h>

void PlaneGCSSolver_UpdateCoincidence::attach(SketchSolver_Constraint* theObserver, const std::string& theType)
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

bool PlaneGCSSolver_UpdateCoincidence::checkCoincidence(
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2)
{
  bool isAccepted = true;

  std::list<std::set<EntityWrapperPtr> >::iterator anIt = myCoincident.begin();
  std::list<std::set<EntityWrapperPtr> >::iterator aFound1 = myCoincident.end();
  std::list<std::set<EntityWrapperPtr> >::iterator aFound2 = myCoincident.end();
  for (; anIt != myCoincident.end(); ++anIt) {
    if (aFound1 == myCoincident.end() && anIt->find(theEntity1) != anIt->end())
      aFound1 = anIt;
    if (aFound2 == myCoincident.end() && anIt->find(theEntity2) != anIt->end())
      aFound2 = anIt;
    if (aFound1 != myCoincident.end() && aFound2 != myCoincident.end())
      break;
  }

  if (aFound1 == myCoincident.end() && aFound2 == myCoincident.end()) {
    // new group of coincidence
    std::set<EntityWrapperPtr> aNewCoinc;
    aNewCoinc.insert(theEntity1);
    aNewCoinc.insert(theEntity2);
    myCoincident.push_back(aNewCoinc);
  } else if (aFound1 == aFound2) // same group => already coincident
    isAccepted = false;
  else if (aFound1 == myCoincident.end())
    aFound2->insert(theEntity1);
  else if (aFound2 == myCoincident.end())
    aFound1->insert(theEntity2);
  else { // merge two groups
    aFound1->insert(aFound2->begin(), aFound2->end());
    myCoincident.erase(aFound2);
  }

  return isAccepted;
}
