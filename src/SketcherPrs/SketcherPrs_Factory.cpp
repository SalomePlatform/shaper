// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Factory.cpp
// Created:     13 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Factory.h"

#include "SketcherPrs_Coincident.h"
#include "SketcherPrs_Parallel.h"
#include "SketcherPrs_Perpendicular.h"
#include "SketcherPrs_Rigid.h"
#include "SketcherPrs_HVDirection.h"
#include "SketcherPrs_Equal.h"
#include "SketcherPrs_Tangent.h"
#include "SketcherPrs_Radius.h"
#include "SketcherPrs_LengthDimension.h"

#define CONSTRAINT_PRS_IMPL(NAME, CLASS) \
AISObjectPtr SketcherPrs_Factory::NAME(ModelAPI_Feature* theConstraint, \
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
CONSTRAINT_PRS_IMPL(rigidConstraint, SketcherPrs_Rigid);
CONSTRAINT_PRS_IMPL(equalConstraint, SketcherPrs_Equal);
CONSTRAINT_PRS_IMPL(tangentConstraint, SketcherPrs_Tangent);
CONSTRAINT_PRS_IMPL(radiusConstraint, SketcherPrs_Radius);
CONSTRAINT_PRS_IMPL(lengthDimensionConstraint, SketcherPrs_LengthDimension);


AISObjectPtr SketcherPrs_Factory::horisontalConstraint(ModelAPI_Feature* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{ 
  std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject()); 
  Handle(SketcherPrs_HVDirection) aPrs = new SketcherPrs_HVDirection(theConstraint, thePlane, true); 
  aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs)); 
  return aAISObj; 
}

AISObjectPtr SketcherPrs_Factory::verticalConstraint(ModelAPI_Feature* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{ 
  std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject()); 
  Handle(SketcherPrs_HVDirection) aPrs = new SketcherPrs_HVDirection(theConstraint, thePlane, false); 
  aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs)); 
  return aAISObj; 
}
