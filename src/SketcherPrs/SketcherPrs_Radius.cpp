// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "SketcherPrs_Radius.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_DimensionStyleListener.h"

#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_XYZ.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>

#include <gp_Circ.hxx>

/// Creates an aspect to be shown in length/radius dimension presentations
/// \return an instance of aspect
extern Handle(Prs3d_DimensionAspect) createDimensionAspect();

/// Update variable aspect parameters (depending on viewer scale)
/// \param theDimAspect an aspect to be changed
/// \param theDimValue an arrow value
/// \param theTextSize an arrow value
extern void updateArrows(Handle(Prs3d_DimensionAspect) theDimAspect,
  double theDimValue, double theTextSize, SketcherPrs_Tools::LocationType theLocationType);


static const gp_Circ MyDefCirc(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), 1);

IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Radius, AIS_RadiusDimension);

SketcherPrs_Radius::SketcherPrs_Radius(ModelAPI_Feature* theConstraint,
  SketchPlugin_Sketch* theSketcher)
: AIS_RadiusDimension(MyDefCirc), myConstraint(theConstraint), mySketcher(theSketcher),
  myCircle(MyDefCirc),
  myAnchorPoint(gp_Pnt(0, 0, 2)),
  myValue(1, false, "")
{
  SetDimensionAspect(createDimensionAspect());
  myStyleListener = new SketcherPrs_DimensionStyleListener();
}

SketcherPrs_Radius::~SketcherPrs_Radius()
{
  delete myStyleListener;
}

bool SketcherPrs_Radius::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                          const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  gp_Circ aCircle;
  gp_Pnt anAnchorPoint;
  return readyToDisplay(theConstraint, thePlane, aCircle, anAnchorPoint);
}

bool SketcherPrs_Radius::readyToDisplay(ModelAPI_Feature* theConstraint,
                                        const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                        gp_Circ& theCircle, gp_Pnt& theAnchorPoint)
{
  bool aReadyToDisplay = false;

  DataPtr aData = theConstraint->data();

  // Flyout point
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>
    (aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  if (!aFlyoutAttr->isInitialized()) {
    return aReadyToDisplay; // can not create a good presentation
  }

  // Get circle
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
    (aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr)
    return aReadyToDisplay;

  std::shared_ptr<ModelAPI_Feature> aCyrcFeature = ModelAPI_Feature::feature(anAttr->object());
  //theRadius = 1;
  double aRadius = 1;
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr;
  // it is possible that circle result becomes zero, in this case the presentation should disappear
  // for example, it happens when circle radius is set to zero
  if (!aCyrcFeature)
    return aReadyToDisplay;
  if (aCyrcFeature->getKind() == SketchPlugin_Circle::ID()) { // circle
    aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aCyrcFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID()));
    AttributeDoublePtr aCircRadius =
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aCyrcFeature->data()->attribute(SketchPlugin_Circle::RADIUS_ID()));
    aRadius = aCircRadius->value();
  } else { // arc
    aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aCyrcFeature->data()->attribute(SketchPlugin_Arc::CENTER_ID()));
    //std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
    //  std::dynamic_pointer_cast<GeomDataAPI_Point2D>
    //  (aCyrcFeature->data()->attribute(SketchPlugin_Arc::START_ID()));
    //theRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
    AttributeDoublePtr aCircRadius =
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aCyrcFeature->data()->attribute(SketchPlugin_Arc::RADIUS_ID()));
    aRadius = aCircRadius->value();
  }
  std::shared_ptr<GeomAPI_Pnt> aCenter = thePlane->to3D(aCenterAttr->x(), aCenterAttr->y());
  std::shared_ptr<GeomAPI_Dir> aNormal = thePlane->normal();

  GeomAPI_Circ aCircle(aCenter, aNormal, aRadius);
  std::shared_ptr<GeomAPI_Pnt> anAnchor =
    SketcherPrs_Tools::getAnchorPoint(theConstraint, thePlane);

  theCircle = aCircle.impl<gp_Circ>();
  theAnchorPoint = anAnchor->impl<gp_Pnt>();

  aReadyToDisplay = theAnchorPoint.Distance(theCircle.Location()) > 1e-7;

  return aReadyToDisplay;
}

void SketcherPrs_Radius::Compute(
  const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
  const Handle(Prs3d_Presentation)& thePresentation,
  const Standard_Integer theMode)
{
  if (!plane().get())
    return;
  gp_Circ aCircle;
  gp_Pnt anAnchorPoint;
  bool aReadyToDisplay = readyToDisplay(myConstraint, plane(), aCircle, anAnchorPoint);
  if (aReadyToDisplay) {
    myCircle = aCircle;
    myAnchorPoint = anAnchorPoint;

    DataPtr aData = myConstraint->data();
    AttributeDoublePtr anAttributeValue = aData->real(SketchPlugin_Constraint::VALUE());
    myValue.init(anAttributeValue);
  }

  SetMeasuredGeometry(myCircle, myAnchorPoint);
  myStyleListener->updateDimensions(this, myValue);

  // Update variable aspect parameters (depending on viewer scale)
  double aTextSize = 0.0;
  GetValueString(aTextSize);

  AttributeIntegerPtr aLocAttr = std::dynamic_pointer_cast<ModelAPI_AttributeInteger>
    (myConstraint->data()->attribute(SketchPlugin_ConstraintRadius::LOCATION_TYPE_ID()));
  SketcherPrs_Tools::LocationType aLocationType = aLocAttr->isInitialized() ?
    (SketcherPrs_Tools::LocationType)(aLocAttr->value()) : SketcherPrs_Tools::LOCATION_AUTOMATIC;
  updateArrows(DimensionAspect(), GetValue(), aTextSize, aLocationType);

  AIS_RadiusDimension::Compute(thePresentationManager, thePresentation, theMode);

  if (!aReadyToDisplay)
    SketcherPrs_Tools::sendEmptyPresentationError(myConstraint,
                              "An empty AIS presentation: SketcherPrs_Radius");
}

void SketcherPrs_Radius::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
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
  AIS_RadiusDimension::ComputeSelection(aSelection, aMode);
}
