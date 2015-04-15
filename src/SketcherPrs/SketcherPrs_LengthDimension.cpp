// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_LengthDimension.cpp
// Created:     27 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_LengthDimension.h"
#include "SketcherPrs_Tools.h"

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_XYZ.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Lin2d.h>

#include <ModelAPI_Data.h>


static const gp_Pnt MyDefStart(0,0,0);
static const gp_Pnt MyDefEnd(1,0,0);
static const gp_Pln MyDefPln(gp_Pnt(0,0,0), gp_Dir(0,0,1));

IMPLEMENT_STANDARD_HANDLE(SketcherPrs_LengthDimension, AIS_LengthDimension);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_LengthDimension, AIS_LengthDimension);

SketcherPrs_LengthDimension::SketcherPrs_LengthDimension(ModelAPI_Feature* theConstraint, 
                        const std::shared_ptr<GeomAPI_Ax3>& thePlane)
: AIS_LengthDimension(MyDefStart, MyDefEnd, MyDefPln), 
myConstraint(theConstraint), myPlane(thePlane)
{
  myAspect = new Prs3d_DimensionAspect();
  myAspect->MakeArrows3d(false);
  myAspect->MakeText3d(false);
  myAspect->MakeTextShaded(false);
  myAspect->MakeUnitsDisplayed(false);
  myAspect->TextAspect()->SetHeight(MyTextHeight);
  myAspect->ArrowAspect()->SetLength(SketcherPrs_Tools::getArrowSize());

  SetSelToleranceForText2d(MyTextHeight);
  SetDimensionAspect(myAspect);
}


void SketcherPrs_LengthDimension::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                 const Handle(Prs3d_Presentation)& thePresentation, 
                                 const Standard_Integer theMode)
{
  gp_Pnt aPnt1, aPnt2;
  if (!getPoints(aPnt1, aPnt2))
    return;
  //DataPtr aData = myConstraint->data();

  //AttributePtr aFlyOutAttribute = aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
  //if (!aFlyOutAttribute->isInitialized()) {
  //  return; // not possible to show length because points are not defined
  //}
  //std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = 
  //  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFlyOutAttribute);
  //std::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = myPlane->to3D(aFlyOutAttr->x(), aFlyOutAttr->y());
  //gp_Pnt aFlyPnt = aFlyoutPnt->impl<gp_Pnt>();

  //double aDistance = aPnt1.Distance(aPnt2);

  //double aFlyout = 0;
  //double aDist = 0.0;
  //if (aDistance < Precision::Confusion())
  //  aDist = aPnt1.Distance(aFlyPnt);
  //else {
  //  gp_Lin aLine(aPnt1, gp_Vec(aPnt1, aPnt2));
  //  aDist = aLine.Distance(aFlyPnt);
  //}

  //gp_XYZ aLineDir = aPnt2.XYZ().Subtracted(aPnt1.XYZ());
  //gp_XYZ aFOutDir = aFlyPnt.XYZ().Subtracted(aPnt1.XYZ());
  //gp_XYZ aNorm = myPlane->norm()->xyz()->impl<gp_XYZ>();
  //if (aLineDir.Crossed(aFOutDir).Dot(aNorm) < 0)
  //  aDist = -aDist;
  //aFlyout = aDist;

  //SetFlyout(aFlyout);
  SetFlyout(SketcherPrs_Tools::getFlyoutDistance(myConstraint, myPlane));
  SetMeasuredGeometry(aPnt1, aPnt2, myPlane->impl<gp_Ax3>());
  AIS_LengthDimension::Compute(thePresentationManager, thePresentation, theMode);
}

bool SketcherPrs_LengthDimension::getPoints(gp_Pnt& thePnt1, gp_Pnt& thePnt2) const
{
  DataPtr aData = myConstraint->data();
  if (myConstraint->getKind() == SketchPlugin_ConstraintLength::ID()) {
    std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
      (aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
    if (!anAttr)
      return false;

    FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
    if (!aFeature || aFeature->getKind() != SketchPlugin_Line::ID())
      return false;

    DataPtr aLineData = aFeature->data();
    std::shared_ptr<GeomDataAPI_Point2D> aStartPoint = 
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (aLineData->attribute(SketchPlugin_Line::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEndPoint = 
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (aLineData->attribute(SketchPlugin_Line::END_ID()));
    thePnt1 = myPlane->to3D(aStartPoint->x(), aStartPoint->y())->impl<gp_Pnt>();
    thePnt2 = myPlane->to3D(aEndPoint->x(), aEndPoint->y())->impl<gp_Pnt>();
    return true;

  } else if (myConstraint->getKind() == SketchPlugin_ConstraintDistance::ID()) {
    std::shared_ptr<GeomDataAPI_Point2D> aPoint_A = SketcherPrs_Tools::getFeaturePoint(
        aData, SketchPlugin_Constraint::ENTITY_A());
    std::shared_ptr<GeomDataAPI_Point2D> aPoint_B = SketcherPrs_Tools::getFeaturePoint(
        aData, SketchPlugin_Constraint::ENTITY_B());

    std::shared_ptr<GeomAPI_Pnt2d> aPnt_A;
    std::shared_ptr<GeomAPI_Pnt2d> aPnt_B;
    if (aPoint_A && aPoint_B) {
      aPnt_A = aPoint_A->pnt();
      aPnt_B = aPoint_B->pnt();
    } else if (!aPoint_A && aPoint_B) {
      FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(
          aData, SketchPlugin_Constraint::ENTITY_A());
      if (aLine) {
        aPnt_B = aPoint_B->pnt();
        aPnt_A = SketcherPrs_Tools::getProjectionPoint(aLine, aPnt_B);
      }
    } else if (aPoint_A && !aPoint_B) {
      FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(
          aData, SketchPlugin_Constraint::ENTITY_B());
      if (aLine) {
        aPnt_A = aPoint_A->pnt();
        aPnt_B = SketcherPrs_Tools::getProjectionPoint(aLine, aPnt_A);
      }
    }
    if (!aPnt_A || !aPnt_B)
      return false;

    std::shared_ptr<GeomAPI_Pnt> aPoint1 = myPlane->to3D(aPnt_A->x(), aPnt_A->y());
    std::shared_ptr<GeomAPI_Pnt> aPoint2 = myPlane->to3D(aPnt_B->x(), aPnt_B->y());
    thePnt1 = aPoint1->impl<gp_Pnt>();
    thePnt2 = aPoint2->impl<gp_Pnt>();
    return true;
  }
  return false;
}



void SketcherPrs_LengthDimension::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                                   const Standard_Integer theMode)
{
  Standard_Integer aMode;
  switch (theMode) {
  case SketcherPrs_Tools::Sel_Dimension_All:
    aMode = 0;
    break;
  case SketcherPrs_Tools::Sel_Dimension_Line:
    aMode = 1;
    break;
  case SketcherPrs_Tools::Sel_Dimension_Text:
    aMode = 2;
    break;
  default:
    aMode = theMode;
  }
  AIS_LengthDimension::ComputeSelection(aSelection, aMode);
}
