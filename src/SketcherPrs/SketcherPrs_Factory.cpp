// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Factory.cpp
// Created:     13 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Factory.h"

#include <SketcherPrs_Coincident.h>
#include <SketcherPrs_Parallel.h>
#include <SketcherPrs_Perpendicular.h>
#include <SketcherPrs_Rigid.h>
#include <SketcherPrs_HVDirection.h>

#define CONSTRAINT_PRS_IMPL(NAME, CLASS) \
AISObjectPtr SketcherPrs_Factory::NAME(SketchPlugin_Constraint* theConstraint, \
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane) \
{ \
  std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject()); \
  Handle(CLASS) aPrs = new CLASS(theConstraint, thePlane); \
  aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs)); \
  return aAISObj; \
}

CONSTRAINT_PRS_IMPL(coincidentConstraint, SketcherPrs_Coincident);
CONSTRAINT_PRS_IMPL(parallelConstraint, SketcherPrs_Parallel);
CONSTRAINT_PRS_IMPL(perpendicularConstraint, SketcherPrs_Perpendicular);
CONSTRAINT_PRS_IMPL(rigidConstraint, SketcherPrs_Rigid)


AISObjectPtr SketcherPrs_Factory::horisontalConstraint(SketchPlugin_Constraint* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{ 
  std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject()); 
  Handle(SketcherPrs_HVDirection) aPrs = new SketcherPrs_HVDirection(theConstraint, thePlane, true); 
  aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs)); 
  return aAISObj; 
}

AISObjectPtr SketcherPrs_Factory::verticalConstraint(SketchPlugin_Constraint* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{ 
  std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject()); 
  Handle(SketcherPrs_HVDirection) aPrs = new SketcherPrs_HVDirection(theConstraint, thePlane, false); 
  aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs)); 
  return aAISObj; 
}
