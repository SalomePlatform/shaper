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

double SketchPlugin_ConstraintBase::distanceToPoint(
    const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  return 0;
}
