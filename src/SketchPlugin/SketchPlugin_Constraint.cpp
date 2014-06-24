// File:    SketchPlugin_Constraint.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_Constraint.h"

#include <AIS_InteractiveObject.hxx>

Handle_AIS_InteractiveObject SketchPlugin_Constraint::getAISShape(Handle_AIS_InteractiveObject thePrevious)
{ 
  return thePrevious;
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_Constraint::preview()
{
  return getPreview();
}

