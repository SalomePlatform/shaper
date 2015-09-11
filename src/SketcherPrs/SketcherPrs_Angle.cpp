// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Angle.cpp
// Created:     20 August 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Angle.h"
#include "SketcherPrs_Tools.h"

#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_Constraint.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeDouble.h>

#include <TopExp.hxx>
#include <BRep_Tool.hxx>

#define PI 3.1415926535897932

IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Angle, AIS_AngleDimension);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Angle, AIS_AngleDimension);


SketcherPrs_Angle::SketcherPrs_Angle(ModelAPI_Feature* theConstraint, 
                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane)
: AIS_AngleDimension(gp_Pnt(0,0,0), gp_Pnt(1,0,0), gp_Pnt(0,1,0)), myConstraint(theConstraint), myPlane(thePlane)
{
  myAspect = new Prs3d_DimensionAspect();
  myAspect->MakeArrows3d(false);
  myAspect->MakeText3d(false);
  myAspect->MakeTextShaded(false);
  myAspect->MakeUnitsDisplayed(false);
  myAspect->TextAspect()->SetHeight(SketcherPrs_Tools::getDefaultTextHeight());
  myAspect->ArrowAspect()->SetLength(SketcherPrs_Tools::getArrowSize());
  
  SetDimensionAspect(myAspect);
  SetSelToleranceForText2d(SketcherPrs_Tools::getDefaultTextHeight());
}

void SketcherPrs_Angle::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                const Handle(Prs3d_Presentation)& thePresentation, 
                                const Standard_Integer theMode)
{
  DataPtr aData = myConstraint->data();

  // Flyout point
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = 
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>
    (aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  if (!aFlyoutAttr->isInitialized())
    return; // can not create a good presentation

  std::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = myPlane->to3D(aFlyoutAttr->x(), aFlyoutAttr->y());

  AttributeRefAttrPtr anAttr1 = aData->refattr(SketchPlugin_Constraint::ENTITY_A());
  if (!anAttr1->isInitialized())
    return;

  AttributeRefAttrPtr anAttr2 = aData->refattr(SketchPlugin_Constraint::ENTITY_B());
  if (!anAttr2->isInitialized())
    return;

  // Get angle edges
  ObjectPtr aObj1 = anAttr1->object();
  ObjectPtr aObj2 = anAttr2->object();

  std::shared_ptr<GeomAPI_Shape> aShape1 = SketcherPrs_Tools::getShape(aObj1);
  std::shared_ptr<GeomAPI_Shape> aShape2 = SketcherPrs_Tools::getShape(aObj2);

  if (!aShape1 && !aShape2)
    return;

  TopoDS_Shape aTEdge1 = aShape1->impl<TopoDS_Shape>();
  TopoDS_Shape aTEdge2 = aShape2->impl<TopoDS_Shape>();

  TopoDS_Edge aEdge1 = TopoDS::Edge(aTEdge1);
  TopoDS_Edge aEdge2 = TopoDS::Edge(aTEdge2);
  SetMeasuredGeometry(aEdge1, aEdge2);

  const gp_Pnt& aCenter = CenterPoint();
  const gp_Pnt& aFirst = FirstPoint();
  const gp_Pnt& aSecond = SecondPoint();

  gp_Dir aBisector((aFirst.XYZ() + aSecond.XYZ()) * 0.5 - aCenter.XYZ());

  gp_Pnt aFlyPnt(aFlyoutPnt->x(), aFlyoutPnt->y(), aFlyoutPnt->z());
  gp_XYZ aFlyDir = aFlyPnt.XYZ() - aCenter.XYZ();
  double aDist = aFlyDir.Dot(aBisector.XYZ());
  SetFlyout(aDist);

  // Angle value is in degrees
  AttributeDoublePtr aVal = aData->real(SketchPlugin_Constraint::VALUE());
  SetCustomValue(aVal->value() * PI / 180.0);

  myAspect->SetExtensionSize(myAspect->ArrowAspect()->Length());
  myAspect->SetArrowTailSize(myAspect->ArrowAspect()->Length());

  AIS_AngleDimension::Compute(thePresentationManager, thePresentation, theMode);
}

void SketcherPrs_Angle::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                                   const Standard_Integer theMode)
{
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
  AIS_AngleDimension::ComputeSelection(aSelection, aMode);
}
