// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintBase.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintBase.h"

AISObjectPtr SketchPlugin_ConstraintBase::getAISObject(AISObjectPtr thePrevious)
{
  return thePrevious;
}

const void SketchPlugin_ConstraintBase::addSub(const FeaturePtr& theFeature)
{

}

void SketchPlugin_ConstraintBase::move(const double theDeltaX, const double theDeltaY)
{

}

