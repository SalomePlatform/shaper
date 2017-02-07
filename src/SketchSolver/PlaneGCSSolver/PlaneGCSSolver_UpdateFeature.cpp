// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_UpdateFeature.cpp
// Created: 17 Feb 2017
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_UpdateFeature.h>
#include <SketchSolver_Constraint.h>

void PlaneGCSSolver_UpdateFeature::attach(SketchSolver_Constraint* theObserver, const std::string& theType)
{
  if (theType == GROUP())
    myObservers.push_back(theObserver);
  else
    myNext->attach(theObserver, theType);
}

void PlaneGCSSolver_UpdateFeature::update(const FeaturePtr& theFeature)
{
  std::list<SketchSolver_Constraint*>::iterator anIt = myObservers.begin();
  for (; anIt != myObservers.end(); ++anIt)
    (*anIt)->notify(theFeature, this);
}
