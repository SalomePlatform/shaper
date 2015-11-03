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
  myAspect->TextAspect()->SetHeight(SketcherPrs_Tools::getDefaultTextHeight());
  myAspect->ArrowAspect()->SetLength(SketcherPrs_Tools::getArrowSize());

  SetSelToleranceForText2d(SketcherPrs_Tools::getTextHeight());
  SetDimensionAspect(myAspect);
}


void SketcherPrs_LengthDimension::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                 const Handle(Prs3d_Presentation)& thePresentation, 
                                 const Standard_Integer theMode)
{
  gp_Pnt aPnt1, aPnt2;
  if (!getPoints(aPnt1, aPnt2))
    return;

  // compute flyout distance
  double aD = SketcherPrs_Tools::getFlyoutDistance(myConstraint);
  SetFlyout(SketcherPrs_Tools::getFlyoutDistance(myConstraint));
  SetMeasuredGeometry(aPnt1, aPnt2, myPlane->impl<gp_Ax3>());

  // Update variable aspect parameters (depending on viewer scale)
  myAspect->SetExtensionSize(myAspect->ArrowAspect()->Length());
  myAspect->SetArrowTailSize(myAspect->ArrowAspect()->Length());
  // The value of vertical aligment is sometimes changed
  myAspect->TextAspect()->SetVerticalJustification(Graphic3d_VTA_CENTER);

  AIS_LengthDimension::Compute(thePresentationManager, thePresentation, theMode);
}

bool SketcherPrs_LengthDimension::getPoints(gp_Pnt& thePnt1, gp_Pnt& thePnt2)
{
  DataPtr aData = myConstraint->data();
  if (myConstraint->getKind() == SketchPlugin_ConstraintLength::ID()) {
    // The constraint is length
    std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
      (aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
    if (!anAttr)
      return false;

    FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
    if (!aFeature || aFeature->getKind() != SketchPlugin_Line::ID())
      return false;

    // Get geometry of the object
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
    // The constraint is distance
    std::shared_ptr<GeomDataAPI_Point2D> aPoint_A = SketcherPrs_Tools::getFeaturePoint(
        aData, SketchPlugin_Constraint::ENTITY_A(), myPlane);
    std::shared_ptr<GeomDataAPI_Point2D> aPoint_B = SketcherPrs_Tools::getFeaturePoint(
        aData, SketchPlugin_Constraint::ENTITY_B(), myPlane);

    std::shared_ptr<GeomAPI_Pnt2d> aPnt_A;
    std::shared_ptr<GeomAPI_Pnt2d> aPnt_B;
    if (aPoint_A && aPoint_B) {
      // Both objects are points
      aPnt_A = aPoint_A->pnt();
      aPnt_B = aPoint_B->pnt();
    } else if (!aPoint_A && aPoint_B) {
      // First object is line
      FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(
          aData, SketchPlugin_Constraint::ENTITY_A());
      if (aLine) {
        aPnt_B = aPoint_B->pnt();
        aPnt_A = SketcherPrs_Tools::getProjectionPoint(aLine, aPnt_B);
      }
    } else if (aPoint_A && !aPoint_B) {
      // Second object is line
      FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(
          aData, SketchPlugin_Constraint::ENTITY_B());
      if (aLine) {
        aPnt_A = aPoint_A->pnt();
        aPnt_B = SketcherPrs_Tools::getProjectionPoint(aLine, aPnt_A);
      }
    }
    if (!aPnt_A || !aPnt_B) // Objects not found
      return false;

    // Get points from these objects
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
  // Map the application selection modes to standard ones
  Standard_Integer aMode;
  switch (theMode) {
  case 0: // we should use selection of all objects
    aMode = 0;
    break;
  case SketcherPrs_Tools::Sel_Dimension_All:
    aMode = 0;
    break;
  case SketcherPrs_Tools::Sel_Dimension_Line:
    aMode = 1;
    break;
  case SketcherPrs_Tools::Sel_Dimension_Text:
    aMode = 2;
    break;
  default: {
    // there are own selection modes, so the others should be ignored
    // otherwise, the text selection appears in the viewer
    return;
  }
  }
  SetSelToleranceForText2d(SketcherPrs_Tools::getTextHeight());
  AIS_LengthDimension::ComputeSelection(aSelection, aMode);
}
