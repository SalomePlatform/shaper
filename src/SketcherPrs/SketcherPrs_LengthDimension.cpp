// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "SketcherPrs_LengthDimension.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_DimensionStyle.h"

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintDistanceHorizontal.h>
#include <SketchPlugin_ConstraintDistanceVertical.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_XYZ.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Lin2d.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>


#define OCCT_28850_FIXED

/// Creates an aspect to be shown in length/radius dimension presentations
/// \return an instance of aspect
Handle(Prs3d_DimensionAspect) createDimensionAspect()
{
  Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeArrows3d(false);
  anAspect->MakeText3d(false);
  anAspect->MakeTextShaded(false);
  anAspect->MakeUnitsDisplayed(false);
  anAspect->TextAspect()->SetHeight(SketcherPrs_Tools::getConfigTextHeight());
  anAspect->ArrowAspect()->SetLength(SketcherPrs_Tools::getArrowSize());

  return anAspect;
}

/// Update variable aspect parameters (depending on viewer scale)
/// \param theDimAspect an aspect to be changed
/// \param theDimValue an arrow value
/// \param theTextSize an arrow value
void updateArrows(Handle(Prs3d_DimensionAspect) theDimAspect,
  double theDimValue, double theTextSize, SketcherPrs_Tools::LocationType theLocationType)
{
  if (theLocationType == SketcherPrs_Tools::LOCATION_AUTOMATIC) {
    double anArrowLength = theDimAspect->ArrowAspect()->Length();
     // This is not realy correct way to get viewer scale.
    double aViewerScale = (double) SketcherPrs_Tools::getConfigArrowSize() / anArrowLength;

    if(theTextSize > ((theDimValue - 3 * SketcherPrs_Tools::getArrowSize()) * aViewerScale)) {
      theDimAspect->SetTextHorizontalPosition(Prs3d_DTHP_Left);
      theDimAspect->SetArrowOrientation(Prs3d_DAO_External);
      theDimAspect->SetExtensionSize(
        (theTextSize / aViewerScale + SketcherPrs_Tools::getArrowSize()) / 2.0);
    } else {
      theDimAspect->SetTextHorizontalPosition(Prs3d_DTHP_Center);
      theDimAspect->SetArrowOrientation(Prs3d_DAO_Internal);
    }
  }
  else if (theLocationType == SketcherPrs_Tools::LOCATION_RIGHT ||
           theLocationType == SketcherPrs_Tools::LOCATION_LEFT) {
    theDimAspect->SetTextHorizontalPosition(
      theLocationType == SketcherPrs_Tools::LOCATION_LEFT ? Prs3d_DTHP_Left : Prs3d_DTHP_Right);
    theDimAspect->SetArrowOrientation(Prs3d_DAO_External);

    double anArrowLength = theDimAspect->ArrowAspect()->Length();
    // This is not realy correct way to get viewer scale.
    double aViewerScale = (double) SketcherPrs_Tools::getConfigArrowSize() / anArrowLength;
    theDimAspect->SetExtensionSize(
        (theTextSize / aViewerScale + SketcherPrs_Tools::getArrowSize()) / 2.0);
  }
  theDimAspect->TextAspect()->SetFont(Config_PropManager::string("Visualization",
                                      "dimension_font").c_str());

  theDimAspect->SetArrowTailSize(theDimAspect->ArrowAspect()->Length());
  // The value of vertical aligment is sometimes changed
  theDimAspect->TextAspect()->SetVerticalJustification(Graphic3d_VTA_CENTER);
}


static const gp_Pnt MyDefStart(0,0,0);
static const gp_Pnt MyDefEnd(1,0,0);
static const gp_Pln MyDefPln(gp_Pnt(0,0,0), gp_Dir(0,0,1));

IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_LengthDimension, AIS_LengthDimension);

SketcherPrs_LengthDimension::SketcherPrs_LengthDimension(ModelAPI_Feature* theConstraint,
  SketchPlugin_Sketch* theSketcher)
: AIS_LengthDimension(MyDefStart, MyDefEnd, MyDefPln),
  myConstraint(theConstraint),
  mySketcher(theSketcher),
  myFirstPoint(MyDefStart),
  mySecondPoint(MyDefEnd),
  myPlane(MyDefPln),
  myDistance(1),
  myValue(0., false, "")
{
  SetDimensionAspect(createDimensionAspect());
  myStyleListener = new SketcherPrs_DimensionStyle();

  setDirection(theConstraint, plane());
}

SketcherPrs_LengthDimension::~SketcherPrs_LengthDimension()
{
  delete myStyleListener;
}

bool SketcherPrs_LengthDimension::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  gp_Pnt2d aPnt1, aPnt2;
  return readyToDisplay(theConstraint, thePlane, aPnt1, aPnt2);
}

static bool isEqualPoints(ModelAPI_Feature* theConstraint,
  const gp_Pnt2d& theP1, const gp_Pnt2d& theP2)
{
  bool isEqual = false;
  if (theConstraint->getKind() == SketchPlugin_ConstraintDistanceHorizontal::ID())
    isEqual = Abs(theP1.X() - theP2.X()) < Precision::Confusion();
  else if (theConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID())
    isEqual = Abs(theP1.Y() - theP2.Y()) < Precision::Confusion();
  else {
    isEqual = theP1.SquareDistance(theP2) < Precision::SquareConfusion();
  }
  return isEqual;
}

void SketcherPrs_LengthDimension::Compute(
  const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
  const Handle(Prs3d_Presentation)& thePresentation,
  const Standard_Integer theMode)
{
  if (!plane().get())
    return;
  gp_Pnt2d aP1, aP2;
  bool aReadyToDisplay = readyToDisplay(myConstraint, plane(), aP1, aP2);
  if (aReadyToDisplay) {
    if (isEqualPoints(myConstraint, aP1, aP2)) {
      // adjust points to draw the dimension presentation
      std::shared_ptr<GeomDataAPI_Dir> aDirAttr = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
          myConstraint->attribute(SketchPlugin_ConstraintDistance::DIRECTION_ID()));
      double x = 0.0, y = 0.0;
      if (aDirAttr && aDirAttr->isInitialized()) {
        x = aDirAttr->x();
        y = aDirAttr->y();
        if (x == 0.0 && y == 0.0)
          x = 1.0;
      }
      else if (myConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID())
        y = 1.0;
      else
        x = 1.0;

      gp_XY aDir(x, y);
      aP1.ChangeCoord().Add(aDir * (-Precision::Confusion()));
      aP2.ChangeCoord().Add(aDir * Precision::Confusion());
    }

    myFirstPoint = plane()->to3D(aP1.X(), aP1.Y())->impl<gp_Pnt>();
    mySecondPoint = plane()->to3D(aP2.X(), aP2.Y())->impl<gp_Pnt>();

    myDistance = SketcherPrs_Tools::getFlyoutDistance(myConstraint);
    myPlane = gp_Pln(plane()->impl<gp_Ax3>());

    DataPtr aData = myConstraint->data();
    AttributeDoublePtr anAttributeValue;
    if (myConstraint->getKind() == SketchPlugin_ConstraintDistanceHorizontal::ID() ||
        myConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID())
      anAttributeValue = aData->real(SketchPlugin_ConstraintDistanceAlongDir::DISTANCE_VALUE_ID());
    else
      anAttributeValue = aData->real(SketchPlugin_Constraint::VALUE());
    myValue.init(anAttributeValue);

    setDirection(myConstraint, plane());
  }

  // compute flyout distance
  SetFlyout(myDistance);
  SetMeasuredGeometry(myFirstPoint, mySecondPoint, myPlane);

  // Update variable aspect parameters (depending on viewer scale)
  double aTextSize = 0.0;
  GetValueString(aTextSize);

  std::string aLocationAttribute;
  std::string aConstraintKind = myConstraint->getKind();
  if (aConstraintKind == SketchPlugin_ConstraintLength::ID())
    aLocationAttribute = SketchPlugin_ConstraintLength::LOCATION_TYPE_ID();
  else if (aConstraintKind == SketchPlugin_ConstraintDistance::ID())
    aLocationAttribute = SketchPlugin_ConstraintDistance::LOCATION_TYPE_ID();
  else if (aConstraintKind == SketchPlugin_ConstraintDistanceHorizontal::ID())
    aLocationAttribute = SketchPlugin_ConstraintDistanceHorizontal::LOCATION_TYPE_ID();
  else if (aConstraintKind == SketchPlugin_ConstraintDistanceVertical::ID())
    aLocationAttribute = SketchPlugin_ConstraintDistanceVertical::LOCATION_TYPE_ID();

  AttributeIntegerPtr aLocAttr = std::dynamic_pointer_cast<ModelAPI_AttributeInteger>
    (myConstraint->data()->attribute(aLocationAttribute));
  SketcherPrs_Tools::LocationType aLocationType = aLocAttr->isInitialized() ?
    (SketcherPrs_Tools::LocationType)(aLocAttr->value()) : SketcherPrs_Tools::LOCATION_AUTOMATIC;
  updateArrows(DimensionAspect(), GetValue(), aTextSize, aLocationType);

  // Update text visualization: parameter value or parameter text
  myStyleListener->updateDimensions(this, myValue);

  AIS_LengthDimension::Compute(thePresentationManager, thePresentation, theMode);

  if (!aReadyToDisplay)
    SketcherPrs_Tools::sendEmptyPresentationError(myConstraint,
                              "An empty AIS presentation: SketcherPrs_LengthDimension");
}

bool SketcherPrs_LengthDimension::readyToDisplay(ModelAPI_Feature* theConstraint,
                                                 const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                                 gp_Pnt2d& thePnt1, gp_Pnt2d& thePnt2)
{
  if (!thePlane)
    return false;

  DataPtr aData = theConstraint->data();
  if (theConstraint->getKind() == SketchPlugin_ConstraintLength::ID()) {
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
    thePnt1 = gp_Pnt2d(aStartPoint->x(), aStartPoint->y());
    thePnt2 = gp_Pnt2d(aEndPoint->x(), aEndPoint->y());
    return true;

  } else if (theConstraint->getKind() == SketchPlugin_ConstraintDistance::ID() ||
             theConstraint->getKind() == SketchPlugin_ConstraintDistanceHorizontal::ID() ||
             theConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID()) {
    // The constraint is distance
    std::shared_ptr<GeomDataAPI_Point2D> aPoint_A = SketcherPrs_Tools::getFeaturePoint(
        aData, SketchPlugin_Constraint::ENTITY_A(), thePlane);
    std::shared_ptr<GeomDataAPI_Point2D> aPoint_B = SketcherPrs_Tools::getFeaturePoint(
        aData, SketchPlugin_Constraint::ENTITY_B(), thePlane);

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

    // Get points from these object
    thePnt1 = gp_Pnt2d(aPnt_A->x(), aPnt_A->y());
    thePnt2 = gp_Pnt2d(aPnt_B->x(), aPnt_B->y());
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
  SetSelToleranceForText2d(SketcherPrs_Tools::getArrowSize()/5.);
  AIS_LengthDimension::ComputeSelection(aSelection, aMode);
}

void SketcherPrs_LengthDimension::setDirection(ModelAPI_Feature* theConstraint,
                                               const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
#ifdef OCCT_28850_FIXED
  if (theConstraint->getKind() == SketchPlugin_ConstraintDistanceHorizontal::ID())
    SetDirection(thePlane->dirX()->impl<gp_Dir>(), true);
  else if (theConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID())
    SetDirection(thePlane->dirY()->impl<gp_Dir>(), true);
#endif
}
