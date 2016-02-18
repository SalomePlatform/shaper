// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Transformation.cpp
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Transformation.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_MultiTranslation.h>
#include <SketchPlugin_MultiRotation.h>
#include <ModelAPI_AttributeRefList.h>
#include <GeomDataAPI_Point2D.h>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Pnt.hxx>
#include <StdPrs_Point.hxx>



IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Transformation, SketcherPrs_SymbolPrs);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Transformation, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Transformation::SketcherPrs_Transformation(ModelAPI_Feature* theConstraint, 
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                           bool isTranslation) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane), myIsTranslation(isTranslation)
{
}  

bool SketcherPrs_Transformation::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                                  const std::shared_ptr<GeomAPI_Ax3>&/* thePlane*/)
{
  bool aReadyToDisplay = false;

  std::shared_ptr<ModelAPI_Data> aData = theConstraint->data();
  // Get transformated objects list
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB = aData->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (anAttrB.get() == NULL)
    return aReadyToDisplay;

  int aNbB = anAttrB->size();
  aReadyToDisplay = aNbB > 0;

#ifdef DEBUG_HIDE_COPY_ATTRIBUTE
  // additional check
  if (theConstraint->getKind() == SketchPlugin_MultiTranslation::ID()) {
    // If it is translation
    AttributePoint2DPtr aStart = GeomDataAPI_Point2D::getPoint2D(aData,
                                            SketchPlugin_MultiTranslation::START_POINT_ID());
    AttributePoint2DPtr aEnd = GeomDataAPI_Point2D::getPoint2D(aData,
                                            SketchPlugin_MultiTranslation::END_POINT_ID());

    aReadyToDisplay = aStart.get() && aEnd.get() && aStart->isInitialized() && aEnd->isInitialized();
  }
  else if (theConstraint->getKind() == SketchPlugin_MultiRotation::ID()) {
    // if it is rotation
    AttributePoint2DPtr aCenter = GeomDataAPI_Point2D::getPoint2D(aData, SketchPlugin_MultiRotation::CENTER_ID());
    aReadyToDisplay = aCenter.get() && aCenter->isInitialized();
  }
#endif
  return aReadyToDisplay;
}

bool SketcherPrs_Transformation::updatePoints(double theStep) const 
{
  if (!IsReadyToDisplay(myConstraint, myPlane))
    return false;

  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  // Get transformated objects list
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB = aData->reflist(SketchPlugin_Constraint::ENTITY_B());

  int aNbB = anAttrB->size();
  if (aNbB == 0)
  {
#ifdef DEBUG_SENSITIVE_TO_BE_CORRECTED
  //if (!myPntArray.IsNull())
    //  mySPoints.Clear();
#endif
  }

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  myPntArray = new Graphic3d_ArrayOfPoints(aNbB);

  int i;
  ObjectPtr aObj;
  gp_Pnt aP1;
  // Compute points of symbols
  for (i = 0; i < aNbB; i++) {
    aObj = anAttrB->object(i);
    if (SketcherPrs_Tools::getShape(aObj).get() == NULL)
      continue;
    aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->SetVertice(i + 1, aP1);
  }  

  return true;
}

void SketcherPrs_Transformation::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB = aData->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (anAttrB.get() == NULL)
    return;

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);

  //Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  //aGroup->SetPrimitivesAspect(aLineAspect);

  // drawListOfShapes uses myDrawer for attributes definition
  Handle(Prs3d_LineAspect) aLnAspect = new Prs3d_LineAspect(theColor, Aspect_TOL_SOLID, 1);
  myDrawer->SetLineAspect(aLnAspect);

  drawListOfShapes(anAttrB, thePrs);
  if (myConstraint->getKind() == SketchPlugin_MultiTranslation::ID()) {
    // If it is translation
    AttributePoint2DPtr aStart = GeomDataAPI_Point2D::getPoint2D(aData,
                                            SketchPlugin_MultiTranslation::START_POINT_ID());
    AttributePoint2DPtr aEnd = GeomDataAPI_Point2D::getPoint2D(aData,
                                            SketchPlugin_MultiTranslation::END_POINT_ID());

    if (aStart.get() && aEnd.get() && aStart->isInitialized() && aEnd->isInitialized()) {
      // Add start point
      std::shared_ptr<GeomAPI_Pnt> aPnt = myPlane->to3D(aStart->x(), aStart->y());
      Handle(Geom_CartesianPoint) aPoint = new Geom_CartesianPoint(aPnt->impl<gp_Pnt>());
      StdPrs_Point::Add(thePrs, aPoint, myDrawer);

      // Add end point
      aPnt = myPlane->to3D(aEnd->x(), aEnd->y());
      aPoint = new Geom_CartesianPoint(aPnt->impl<gp_Pnt>());
      StdPrs_Point::Add(thePrs, aPoint, myDrawer);
    }
  } else if (myConstraint->getKind() == SketchPlugin_MultiRotation::ID()) {
    // if it is rotation
    AttributePoint2DPtr aCenter = GeomDataAPI_Point2D::getPoint2D(aData, SketchPlugin_MultiRotation::CENTER_ID());
    if (aCenter.get() && aCenter->isInitialized()) {
      // Show center of rotation
      std::shared_ptr<GeomAPI_Pnt> aPnt = myPlane->to3D(aCenter->x(), aCenter->y());
      Handle(Geom_CartesianPoint) aPoint = new Geom_CartesianPoint(aPnt->impl<gp_Pnt>());
      StdPrs_Point::Add(thePrs, aPoint, myDrawer);
    }
  }
}

