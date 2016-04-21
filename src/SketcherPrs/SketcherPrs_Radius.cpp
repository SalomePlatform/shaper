// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Radius.cpp
// Created:     26 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Radius.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_DimensionStyleListener.h"

#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>

#include <Events_Error.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_XYZ.h>
#include <ModelAPI_AttributeDouble.h>

#include <gp_Circ.hxx>

static const gp_Circ MyDefCirc(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), 1);

IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Radius, AIS_RadiusDimension);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Radius, AIS_RadiusDimension);

SketcherPrs_Radius::SketcherPrs_Radius(ModelAPI_Feature* theConstraint, 
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane)
: AIS_RadiusDimension(MyDefCirc), myConstraint(theConstraint), mySketcherPlane(thePlane)
{
  SetDimensionAspect(SketcherPrs_Tools::createDimensionAspect());
  SetSelToleranceForText2d(SketcherPrs_Tools::getDefaultTextHeight());

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
  double aRadius;
  return readyToDisplay(theConstraint, thePlane, aCircle, anAnchorPoint, aRadius);
}

bool SketcherPrs_Radius::readyToDisplay(ModelAPI_Feature* theConstraint,
                                        const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                        gp_Circ& theCircle, gp_Pnt& theAnchorPoint,
                                        double& theRadius)
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
  theRadius = 1;
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
    theRadius = aCircRadius->value();
  } else { // arc
    aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aCyrcFeature->data()->attribute(SketchPlugin_Arc::CENTER_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (aCyrcFeature->data()->attribute(SketchPlugin_Arc::START_ID()));
    theRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
  }
  std::shared_ptr<GeomAPI_Pnt> aCenter = thePlane->to3D(aCenterAttr->x(), aCenterAttr->y());
  std::shared_ptr<GeomAPI_Dir> aNormal = thePlane->normal();

  GeomAPI_Circ aCircle(aCenter, aNormal, theRadius);
  std::shared_ptr<GeomAPI_Pnt> anAnchor = SketcherPrs_Tools::getAnchorPoint(theConstraint, thePlane);

  theCircle = aCircle.impl<gp_Circ>();
  theAnchorPoint = anAnchor->impl<gp_Pnt>();

  aReadyToDisplay = theAnchorPoint.Distance(theCircle.Location()) > 1e-7;

  return aReadyToDisplay;
}

void SketcherPrs_Radius::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                 const Handle(Prs3d_Presentation)& thePresentation, 
                                 const Standard_Integer theMode)
{
  gp_Circ aCircle;
  gp_Pnt anAnchorPoint;
  double aRadius;
  bool aReadyToDisplay = readyToDisplay(myConstraint, mySketcherPlane, aCircle, anAnchorPoint, aRadius);
  if (aReadyToDisplay) {
    myCircle = aCircle;
    myAnchorPoint = anAnchorPoint;
    myRadius = aRadius;

    AttributeDoublePtr anAttributeValue = myConstraint->data()->real(SketchPlugin_Constraint::VALUE());
    myHasParameters = anAttributeValue->usedParameters().size() > 0;
    myValue = anAttributeValue->text();
  }

  SetMeasuredGeometry(myCircle, myAnchorPoint);
  SetCustomValue(myRadius);

  // Update variable aspect parameters (depending on viewer scale)
  double aTextSize = 0.0;
  GetValueString(aTextSize);
  SketcherPrs_Tools::updateArrows(DimensionAspect(), GetValue(), aTextSize);

  myStyleListener->updateDimensions(this, myHasParameters, myValue);
  
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
  AIS_RadiusDimension::ComputeSelection(aSelection, aMode);
}
