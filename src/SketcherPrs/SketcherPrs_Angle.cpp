// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SketcherPrs_Angle.h"
#include "SketcherPrs_DimensionStyle.h"
#include "SketcherPrs_Tools.h"

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

/// Update variable aspect parameters (depending on viewer scale)
/// \param theDimAspect an aspect to be changed
/// \param theDimValue an arrow value
/// \param theTextSize an arrow value
extern void updateArrows(Handle(Prs3d_DimensionAspect) theDimAspect,
  double theDimValue, double theTextSize, SketcherPrs_Tools::LocationType theLocationType);

#define PI 3.1415926535897932

//#ifndef WNT
//  #define COMPILATION_CORRECTION
//#endif

IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Angle, AIS_AngleDimension);

SketcherPrs_Angle::SketcherPrs_Angle(ModelAPI_Feature* theConstraint,
  SketchPlugin_Sketch* theSketcher)
: AIS_AngleDimension(gp_Pnt(0,0,0), gp_Pnt(1,0,0), gp_Pnt(0,1,0)), myConstraint(theConstraint),
  mySketcher(theSketcher),
  myFirstPoint(gp_Pnt(0,0,0)), myCenterPoint(gp_Pnt(1,0,0)), mySecondPoint(gp_Pnt(0,1,0)),
  myValue(90., false, ""), myFlyOutPoint(0, 0.5, 0)
{
  myAspect = new Prs3d_DimensionAspect();
  myAspect->MakeArrows3d(false);
  myAspect->MakeText3d(false);
  myAspect->MakeTextShaded(false);
  myAspect->MakeUnitsDisplayed(false);
  myAspect->TextAspect()->SetHeight(SketcherPrs_Tools::getConfigTextHeight());
  myAspect->ArrowAspect()->SetLength(SketcherPrs_Tools::getArrowSize());

  SetDimensionAspect(myAspect);

  myStyleListener = new SketcherPrs_DimensionStyle();
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
  if (!thePlane)
    return aReadyToDisplay;

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

  FeaturePtr aLineA =
    SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLineB =
    SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());

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
  bool isFirstPnt = aData->attribute(
    SketchPlugin_ConstraintAngle::ANGLE_REVERSED_FIRST_LINE_ID())->isInitialized();
  bool isSecondPnt = aData->attribute(
    SketchPlugin_ConstraintAngle::ANGLE_REVERSED_SECOND_LINE_ID())->isInitialized();
  if (!isFirstPnt || !isSecondPnt)
    anAng = std::shared_ptr<GeomAPI_Angle2d>(new GeomAPI_Angle2d(
        aStartA->pnt(), aEndA->pnt(), aStartB->pnt(), aEndB->pnt()));
  else {
    std::shared_ptr<GeomAPI_Lin2d> aLine1(new GeomAPI_Lin2d(aStartA->pnt(), aEndA->pnt()));
    bool isReversed1 =
      aData->boolean(SketchPlugin_ConstraintAngle::ANGLE_REVERSED_FIRST_LINE_ID())->value();
    std::shared_ptr<GeomAPI_Lin2d> aLine2(new GeomAPI_Lin2d(aStartB->pnt(), aEndB->pnt()));
    bool isReversed2 =
      aData->boolean(SketchPlugin_ConstraintAngle::ANGLE_REVERSED_SECOND_LINE_ID())->value();

    if (aData->integer(SketchPlugin_ConstraintAngle::TYPE_ID())->value() ==
        (int)SketcherPrs_Tools::ANGLE_COMPLEMENTARY)
      isReversed1 = !isReversed1;

    anAng = std::shared_ptr<GeomAPI_Angle2d>(
      new GeomAPI_Angle2d(aLine1, isReversed1, aLine2, isReversed2));
  }

  gp_Pnt2d aCenterPoint = anAng->center()->impl<gp_Pnt2d>();
  gp_Pnt2d aFirstPoint, aSecondPoint;
  if (anAng->angleRadian() > 0.0) {
    aFirstPoint = anAng->firstPoint()->impl<gp_Pnt2d>();
    aSecondPoint = anAng->secondPoint()->impl<gp_Pnt2d>();
  }
  else {
    aFirstPoint = anAng->secondPoint()->impl<gp_Pnt2d>();
    aSecondPoint = anAng->firstPoint()->impl<gp_Pnt2d>();
  }

  if (aData->integer(SketchPlugin_ConstraintAngle::TYPE_ID())->value() ==
      (int)SketcherPrs_Tools::ANGLE_BACKWARD)
    std::swap(aFirstPoint, aSecondPoint);

  std::shared_ptr<GeomAPI_Pnt> aPoint = thePlane->to3D(aFirstPoint.X(), aFirstPoint.Y());
  theFirstPoint = aPoint->impl<gp_Pnt>();

  aPoint = thePlane->to3D(aCenterPoint.X(), aCenterPoint.Y());
  theCenterPoint = aPoint->impl<gp_Pnt>();

  aPoint = thePlane->to3D(aSecondPoint.X(), aSecondPoint.Y());
  theSecondPoint = aPoint->impl<gp_Pnt>();

  return true;
}


void SketcherPrs_Angle::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                const Handle(Prs3d_Presentation)& thePresentation,
                                const Standard_Integer theMode)
{
  if (!plane().get())
    return;

  DataPtr aData = myConstraint->data();

  gp_Pnt aFirstPoint, aSecondPoint, aCenterPoint;
  bool aReadyToDisplay = readyToDisplay(myConstraint, plane(),
                                        aFirstPoint, aSecondPoint, aCenterPoint);
  if (aReadyToDisplay) {
    myFirstPoint = aFirstPoint;
    mySecondPoint = aSecondPoint;
    myCenterPoint = aCenterPoint;

    AttributeDoublePtr anAttributeValue =
      aData->real(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID());
    myValue.init(anAttributeValue);

    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
                                std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                (aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
    std::shared_ptr<GeomAPI_Pnt> aFlyoutPnt =
      plane()->to3D(aFlyoutAttr->x(), aFlyoutAttr->y());
    myFlyOutPoint = aFlyoutPnt->impl<gp_Pnt>();
  }

  double aDist = -1;
#ifndef COMPILATION_CORRECTION
  SetArrowsVisibility(AIS_TOAV_Both);
#endif
  SetMeasuredGeometry(myFirstPoint, myCenterPoint, mySecondPoint);
#ifndef COMPILATION_CORRECTION
  bool isReversedPlanes = isAnglePlaneReversedToSketchPlane();
  SetType(!isReversedPlanes ? AIS_TOA_Exterior : AIS_TOA_Interior);
#endif
  if (aDist < 0) /// it was not calculated yet
    aDist = calculateDistanceToFlyoutPoint();
  SetFlyout(aDist);

  // Update text visualization: parameter value or parameter text
  myStyleListener->updateDimensions(this, myValue);

  double aTextSize = 0.0;
  GetValueString(aTextSize);
  AttributeIntegerPtr aLocAttr = std::dynamic_pointer_cast<ModelAPI_AttributeInteger>
    (myConstraint->data()->attribute(SketchPlugin_ConstraintAngle::LOCATION_TYPE_ID()));
  SketcherPrs_Tools::LocationType aLocationType = aLocAttr->isInitialized() ?
    (SketcherPrs_Tools::LocationType)(aLocAttr->value()) : SketcherPrs_Tools::LOCATION_AUTOMATIC;

  double aRadius = myCenterPoint.Translated(
    gp_Vec(myCenterPoint, myFirstPoint).Normalized()*aDist).Distance(myCenterPoint);
  double anAngleValue = myValue.myDoubleValue;
  double anAngleCircleLength = aRadius * anAngleValue * PI / 180.;

  updateArrows(myAspect, anAngleCircleLength, aTextSize, aLocationType);

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
  SetSelToleranceForText2d(SketcherPrs_Tools::getArrowSize()/5.);
  AIS_AngleDimension::ComputeSelection(aSelection, aMode);
}

bool SketcherPrs_Angle::isAnglePlaneReversedToSketchPlane()
{
  bool aReversed = false;
  if (!plane().get())
    return aReversed;

  gp_Pln aPlane = GetPlane();
  gp_Dir aDir = aPlane.Axis().Direction();
  double aDot = plane()->normal()->dot(
                std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z())));
  return aDot < 0;
}

double SketcherPrs_Angle::calculateDistanceToFlyoutPoint()
{
  return myFlyOutPoint.Distance(myCenterPoint);
}
