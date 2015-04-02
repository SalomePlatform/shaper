// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Radius.cpp
// Created:     26 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Radius.h"
#include "SketcherPrs_Tools.h"

#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_XYZ.h>
#include <ModelAPI_AttributeDouble.h>

static const gp_Circ MyDefCirc(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), 1);

IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Radius, AIS_RadiusDimension);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Radius, AIS_RadiusDimension);

SketcherPrs_Radius::SketcherPrs_Radius(ModelAPI_Feature* theConstraint, 
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane)
: AIS_RadiusDimension(MyDefCirc), myConstraint(theConstraint), myPlane(thePlane)
{
  myAspect = new Prs3d_DimensionAspect();
  myAspect->MakeArrows3d(false);
  myAspect->MakeText3d(false);
  myAspect->MakeTextShaded(false);
  myAspect->MakeUnitsDisplayed(false);
  myAspect->TextAspect()->SetHeight(MyTextHeight);
  myAspect->ArrowAspect()->SetLength(SketcherPrs_Tools::getArrowSize());
  
  SetDimensionAspect(myAspect);
  SetSelToleranceForText2d(MyTextHeight);
}


void SketcherPrs_Radius::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
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

  // Get circle
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
    (aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr) return;

  std::shared_ptr<ModelAPI_Feature> aCyrcFeature = ModelAPI_Feature::feature(anAttr->object());
  double aRadius = 1;
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr;
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
    std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (aCyrcFeature->data()->attribute(SketchPlugin_Arc::START_ID()));
    aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
  }
  std::shared_ptr<GeomAPI_Pnt> aCenter = myPlane->to3D(aCenterAttr->x(), aCenterAttr->y());
  std::shared_ptr<GeomAPI_Dir> aNormal = myPlane->norm();

  GeomAPI_Circ aCircle(aCenter, aNormal, aRadius);
  //gp_Circ aCircle(gp_Ax2(aCenter->impl<gp_Pnt>(), aNormal->impl<gp_Dir>()), aRadius);
    
  std::shared_ptr<GeomAPI_Pnt> anAnchor = aCircle.project(aFlyoutPnt);
  std::shared_ptr<GeomAPI_XYZ> anAnchorXYZ = anAnchor->xyz();
  anAnchorXYZ = anAnchorXYZ->decreased(aCenter->xyz());
  std::shared_ptr<GeomAPI_Dir> aDeltaDir(new GeomAPI_Dir(anAnchorXYZ));
  const double aDelta = 1e-3;
  anAnchor->setX(anAnchor->x() + aDelta * aDeltaDir->x());
  anAnchor->setY(anAnchor->y() + aDelta * aDeltaDir->y());
  anAnchor->setZ(anAnchor->z() + aDelta * aDeltaDir->z());

  SetMeasuredGeometry(aCircle.impl<gp_Circ>(), anAnchor->impl<gp_Pnt>());
  SetCustomValue(aRadius);

  AIS_RadiusDimension::Compute(thePresentationManager, thePresentation, theMode);
}
