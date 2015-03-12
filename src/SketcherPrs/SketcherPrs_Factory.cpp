// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Factory.cpp
// Created:     13 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Factory.h"

#include <SketcherPrs_Coincident.h>
#include <SketcherPrs_Parallel.h>
#include <SketcherPrs_Perpendicular.h>


AISObjectPtr SketcherPrs_Factory::coincidentConstraint(SketchPlugin_Constraint* theConstraint, 
                                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
  Handle(SketcherPrs_Coincident) aPrs = new SketcherPrs_Coincident(theConstraint, thePlane);
  aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs));
  return aAISObj;
}


AISObjectPtr SketcherPrs_Factory::parallelConstraint(SketchPlugin_Constraint* theConstraint, 
                                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
  Handle(SketcherPrs_Parallel) aPrs = new SketcherPrs_Parallel(theConstraint, thePlane);
  aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs));
  return aAISObj;
}

AISObjectPtr SketcherPrs_Factory::perpendicularConstraint(SketchPlugin_Constraint* theConstraint, 
                                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
  Handle(SketcherPrs_Perpendicular) aPrs = new SketcherPrs_Perpendicular(theConstraint, thePlane);
  aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs));
  return aAISObj;
}
