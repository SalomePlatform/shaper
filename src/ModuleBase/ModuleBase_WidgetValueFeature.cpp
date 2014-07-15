// File:        ModuleBase_WidgetValueFeature.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetValueFeature.h>

#include <GeomAPI_Pnt2d.h>

ModuleBase_WidgetValueFeature::ModuleBase_WidgetValueFeature()
{
}

ModuleBase_WidgetValueFeature::~ModuleBase_WidgetValueFeature()
{
}

void ModuleBase_WidgetValueFeature::setResult(const ResultPtr& theFeature)
{
  myResult = theFeature;
}

const ResultPtr& ModuleBase_WidgetValueFeature::result() const
{
  return myResult;
}

void ModuleBase_WidgetValueFeature::setPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  myPoint = thePoint;
}

const boost::shared_ptr<GeomAPI_Pnt2d>& ModuleBase_WidgetValueFeature::point() const
{
  return myPoint;
}
