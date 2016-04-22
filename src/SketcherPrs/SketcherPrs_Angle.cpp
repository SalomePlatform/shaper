// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Angle.cpp
// Created:     20 August 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Angle.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_DimensionStyleListener.h"

#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Line.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Angle2d.h>
#include <GeomAPI_Lin2d.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>

#include <TopExp.hxx>
#include <BRep_Tool.hxx>

#include <Events_Error.h>

#define PI 3.1415926535897932

IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Angle, AIS_AngleDimension);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Angle, AIS_AngleDimension);

SketcherPrs_Angle::SketcherPrs_Angle(ModelAPI_Feature* theConstraint, 
                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane)
: AIS_AngleDimension(gp_Pnt(0,0,0), gp_Pnt(1,0,0), gp_Pnt(0,1,0)), myConstraint(theConstraint),
  mySketcherPlane(thePlane)
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

  myStyleListener = new SketcherPrs_DimensionStyleListener();
}

SketcherPrs_Angle::~SketcherPrs_Angle()
{
  delete myStyleListener;
}

bool SketcherPrs_Angle::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  gp_Pnt aFirstPoint, aSecondPoint, aCenterPoint;
  return readyToDisplay(theConstraint, thePlane, aFirstPoint, aSecondPoint, aCenterPoint);
}

bool SketcherPrs_Angle::readyToDisplay(ModelAPI_Feature* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                       gp_Pnt& theFirstPoint, gp_Pnt& theSecondPoint,
                                       gp_Pnt& theCenterPoint)
{
  bool aReadyToDisplay = false;

  DataPtr aData = theConstraint->data();

  // Flyout point
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = 
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>
    (aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  if (!aFlyoutAttr->isInitialized())
    return aReadyToDisplay; // can not create a good presentation

  AttributeRefAttrPtr anAttr1 = aData->refattr(SketchPlugin_Constraint::ENTITY_A());
  if (!anAttr1->isInitialized())
    return aReadyToDisplay;

  AttributeRefAttrPtr anAttr2 = aData->refattr(SketchPlugin_Constraint::ENTITY_B());
  if (!anAttr2->isInitialized())
    return aReadyToDisplay;

  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());

  if (!aLineA.get() || !aLineB.get())
    return aReadyToDisplay;

  std::shared_ptr<GeomDataAPI_Point2D> aStartA = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineA->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndA = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineA->attribute(SketchPlugin_Line::END_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartB = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineB->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndB = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineB->attribute(SketchPlugin_Line::END_ID()));

  std::shared_ptr<GeomAPI_Angle2d> anAng;
  if (!aData->attribute(SketchPlugin_ConstraintAngle::ANGLE_REVERSED_FIRST_LINE_ID())->isInitialized() ||
      !aData->attribute(SketchPlugin_ConstraintAngle::ANGLE_REVERSED_SECOND_LINE_ID())->isInitialized())
    anAng = std::shared_ptr<GeomAPI_Angle2d>(new GeomAPI_Angle2d(
        aStartA->pnt(), aEndA->pnt(), aStartB->pnt(), aEndB->pnt()));
  else {
    std::shared_ptr<GeomAPI_Lin2d> aLine1(new GeomAPI_Lin2d(aStartA->pnt(), aEndA->pnt()));
    bool isReversed1 = aData->boolean(SketchPlugin_ConstraintAngle::ANGLE_REVERSED_FIRST_LINE_ID())->value();
    std::shared_ptr<GeomAPI_Lin2d> aLine2(new GeomAPI_Lin2d(aStartB->pnt(), aEndB->pnt()));
    bool isReversed2 = aData->boolean(SketchPlugin_ConstraintAngle::ANGLE_REVERSED_SECOND_LINE_ID())->value();
    anAng = std::shared_ptr<GeomAPI_Angle2d>(new GeomAPI_Angle2d(aLine1, isReversed1, aLine2, isReversed2));
  }

  gp_Pnt2d aFirstPoint = anAng->firstPoint()->impl<gp_Pnt2d>();
  std::shared_ptr<GeomAPI_Pnt> aPoint = thePlane->to3D(aFirstPoint.X(), aFirstPoint.Y());
  theFirstPoint = aPoint->impl<gp_Pnt>();

  gp_Pnt2d aCenterPoint = anAng->center()->impl<gp_Pnt2d>();
  aPoint = thePlane->to3D(aCenterPoint.X(), aCenterPoint.Y());
  theCenterPoint = aPoint->impl<gp_Pnt>();

  gp_Pnt2d aSecondPoint = anAng->secondPoint()->impl<gp_Pnt2d>();
  aPoint = thePlane->to3D(aSecondPoint.X(), aSecondPoint.Y());
  theSecondPoint = aPoint->impl<gp_Pnt>();

  return true;
}


void SketcherPrs_Angle::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                const Handle(Prs3d_Presentation)& thePresentation, 
                                const Standard_Integer theMode)
{
  gp_Pnt aFirstPoint, aSecondPoint, aCenterPoint;
  bool aReadyToDisplay = readyToDisplay(myConstraint, mySketcherPlane, aFirstPoint, aSecondPoint, aCenterPoint);
  if (aReadyToDisplay) {
    myFirstPoint = aFirstPoint;
    mySecondPoint = aSecondPoint;
    myCenterPoint = aCenterPoint;

    DataPtr aData = myConstraint->data();
    AttributeDoublePtr aVal = aData->real(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID());
    myAngle = aVal->value();
    myValue = aVal->text();

    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = 
                                std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                (aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
    std::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = mySketcherPlane->to3D(aFlyoutAttr->x(), aFlyoutAttr->y());
    myFlyOutPoint = aFlyoutPnt->impl<gp_Pnt>();
  }

  DataPtr aData = myConstraint->data();
  std::shared_ptr<ModelAPI_AttributeInteger> aTypeAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeInteger>(aData->attribute(SketchPlugin_ConstraintAngle::TYPE_ID()));
  SketcherPrs_Tools::AngleType anAngleType = (SketcherPrs_Tools::AngleType)(aTypeAttr->value());

  double aDist = -1;
  switch (anAngleType) {
    case SketcherPrs_Tools::ANGLE_DIRECT: {
      SetArrowVisible(Standard_False/*first*/, Standard_True/*second*/);
      SetMeasuredGeometry(myFirstPoint, myCenterPoint, mySecondPoint);
      bool isReversedPlanes = isAnglePlaneReversedToSketchPlane();
      SetAngleReversed(!isReversedPlanes);
    }
    break;
    case SketcherPrs_Tools::ANGLE_COMPLEMENTARY: {
      double anEdge1Length = aCenterPoint.Distance(myFirstPoint);
      //aDist = calculateDistanceToFlyoutPoint();
      gp_Pnt aFirstPoint = aCenterPoint.Translated(
                          gp_Vec(myCenterPoint, myFirstPoint).Normalized() * (-anEdge1Length));
      SetMeasuredGeometry(aFirstPoint, myCenterPoint, mySecondPoint);
    }
    break;
    case SketcherPrs_Tools::ANGLE_BACKWARD: {
      SetArrowVisible(Standard_False/*first*/, Standard_True/*second*/);
      SetMeasuredGeometry(myFirstPoint, myCenterPoint, mySecondPoint);
      bool isReversedPlanes = isAnglePlaneReversedToSketchPlane();
      SetAngleReversed(isReversedPlanes);
    }
    break;
    default:
      break;
  }
  if (aDist < 0) /// it was not calculated yet
    aDist = calculateDistanceToFlyoutPoint();
  SetFlyout(aDist);

  // Angle value is in degrees
  SetCustomValue(myAngle);

  myAspect->SetExtensionSize(myAspect->ArrowAspect()->Length());
  myAspect->SetArrowTailSize(myAspect->ArrowAspect()->Length());

  // Update text visualization: parameter value or parameter text
  myStyleListener->updateDimensions(this, myHasParameters, myValue);

  AIS_AngleDimension::Compute(thePresentationManager, thePresentation, theMode);

  if (!aReadyToDisplay)
    SketcherPrs_Tools::sendEmptyPresentationError(myConstraint,
                              "An empty AIS presentation: SketcherPrs_Angle");
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

bool SketcherPrs_Angle::isAnglePlaneReversedToSketchPlane()
{
  bool aReversed = false;
  if (!mySketcherPlane.get())
    return aReversed;

  gp_Pln aPlane = GetPlane();
  gp_Dir aDir = aPlane.Axis().Direction();
  double aDot = mySketcherPlane->normal()->dot(
                std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z())));
  return aDot < 0;
}

double SketcherPrs_Angle::calculateDistanceToFlyoutPoint()
{
  gp_Dir aBisector((myFirstPoint.XYZ() + mySecondPoint.XYZ()) * 0.5 - myCenterPoint.XYZ());
  //gp_Pnt aFlyPnt(aFlyoutPnt->x(), aFlyoutPnt->y(), aFlyoutPnt->z());
  gp_XYZ aFlyDir = myFlyOutPoint.XYZ() - myCenterPoint.XYZ();
  double aDistance = aFlyDir.Dot(aBisector.XYZ());
  // make a positive distance in order to AIS angle presentation is not reversed
  aDistance = fabs(aDistance);
  return aDistance;
}
