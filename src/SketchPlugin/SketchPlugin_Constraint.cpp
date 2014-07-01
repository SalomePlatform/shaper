// File:    SketchPlugin_Constraint.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_Constraint.h"

boost::shared_ptr<GeomAPI_AISObject> SketchPlugin_Constraint::getAISObject(
                              boost::shared_ptr<GeomAPI_AISObject> thePrevious)
{ 
  return thePrevious;
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_Constraint::preview()
{
  return getPreview();
}

