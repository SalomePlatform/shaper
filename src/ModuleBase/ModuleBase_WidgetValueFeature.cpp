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

void ModuleBase_WidgetValueFeature::setObject(const ObjectPtr& theFeature)
{
  myResult = theFeature;
}

const ObjectPtr& ModuleBase_WidgetValueFeature::object() const
{
  return myResult;
}

void ModuleBase_WidgetValueFeature::setPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  myPoint = thePoint;
}

const std::shared_ptr<GeomAPI_Pnt2d>& ModuleBase_WidgetValueFeature::point() const
{
  return myPoint;
}
