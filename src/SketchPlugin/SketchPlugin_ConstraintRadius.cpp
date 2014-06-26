// File:    SketchPlugin_ConstraintRadius.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintRadius.h"

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Point.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_RadiusDimension.hxx>

SketchPlugin_ConstraintRadius::SketchPlugin_ConstraintRadius()
{
}

void SketchPlugin_ConstraintRadius::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_VALUE,    ModelAPI_AttributeDouble::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT, GeomDataAPI_Point2D::type());
}

void SketchPlugin_ConstraintRadius::execute()
{
  if (data()->attribute(CONSTRAINT_ATTR_ENTITY_A)->isInitialized() &&
      !data()->attribute(CONSTRAINT_ATTR_VALUE)->isInitialized()) {

    boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(data()->attribute(CONSTRAINT_ATTR_ENTITY_A));
    FeaturePtr aFeature = aRef->feature();
    if (aFeature) {
      double aRadius = 0;
      boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();
      if (aFeature->getKind() == SKETCH_CIRCLE_KIND) {
        AttributeDoublePtr anAttribute = boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>
                                            (aData->attribute(CIRCLE_ATTR_RADIUS));
        if (anAttribute)
          aRadius = anAttribute->value();
      }
      else if (aFeature->getKind() == SKETCH_ARC_KIND) {
        boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_CENTER));
        boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_START));
        if (aCenterAttr && aStartAttr)
          aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
      }
      boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(CONSTRAINT_ATTR_VALUE));
      aValueAttr->setValue(aRadius);
    }
  }
}

Handle(AIS_InteractiveObject) SketchPlugin_ConstraintRadius::getAISShape(
  Handle_AIS_InteractiveObject thePrevious)
{
  Handle(AIS_InteractiveObject) anAIS = thePrevious;
  if (!sketch())
    return anAIS;

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  if (!anAttr)
    return anAIS;
  FeaturePtr aFeature = anAttr->feature();
  std::string aKind = aFeature ? aFeature->getKind() : "";
  if (aKind != SKETCH_CIRCLE_KIND && aKind != SKETCH_ARC_KIND)
    return anAIS;

  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_VALUE));
  double aValue = aValueAttr->value();

  // an anchor point
  boost::shared_ptr<GeomDataAPI_Point2D> aAnchorAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT));
  if (!aAnchorAttr->isInitialized())
    return anAIS;
  boost::shared_ptr<GeomAPI_Pnt2d> anAnchor2D = aAnchorAttr->pnt();
  boost::shared_ptr<GeomAPI_Pnt> anAnchor = sketch()->to3D(anAnchor2D->x(), anAnchor2D->y());

  aData = aFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr;
  double aRadius;
  if (aKind == SKETCH_CIRCLE_KIND) {
    aCenterAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CIRCLE_ATTR_CENTER));
    AttributeDoublePtr aCircRadius = 
      boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CIRCLE_ATTR_RADIUS));
    aRadius = aCircRadius->value();
  }
  else if (aKind == SKETCH_ARC_KIND) {
    aCenterAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_CENTER));
    boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_START));
    aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
  }

  // a circle
  boost::shared_ptr<GeomAPI_Pnt> aCenter = sketch()->to3D(aCenterAttr->x(), aCenterAttr->y());

  boost::shared_ptr<GeomDataAPI_Dir> aNDir = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(sketch()->data()->attribute(SKETCH_ATTR_NORM));
  boost::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();

  boost::shared_ptr<GeomAPI_Circ> aCircle(new GeomAPI_Circ(aCenter, aNormal, aRadius));

  anAnchor = aCircle->project(anAnchor);
  // TODO: a bug in AIS_RadiusDimension:
  // The anchor point can't be myCirc.Location() - an exception is raised.
  // But we need exactly this case...
  // We want to show a radius dimension starting from the circle centre and 
  // ending at the user-defined point.
  // Also, if anchor point coincides with myP2, the radius dimension is not displayed at all.
  gp_Pnt anAPnt = anAnchor->impl<gp_Pnt>();
  double aDelta = 1/1000.0;
  if (anAnchor->x() != 0)
    anAnchor->setX(anAnchor->x() + aDelta);
  if (anAnchor->y() != 0)
    anAnchor->setY(anAnchor->y() + aDelta);
  if (anAnchor->z() != 0)
    anAnchor->setZ(anAnchor->z() + aDelta);

  if (anAIS.IsNull())
  {
    Handle(AIS_RadiusDimension) aDimAIS = 
      new AIS_RadiusDimension(aCircle->impl<gp_Circ>(), anAnchor->impl<gp_Pnt>());
    aDimAIS->SetCustomValue(aValue);

    Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
    anAspect->MakeArrows3d (Standard_False);
    anAspect->MakeText3d(false);
    anAspect->TextAspect()->SetHeight(CONSTRAINT_TEXT_HEIGHT);
    anAspect->MakeTextShaded(false);
    aDimAIS->DimensionAspect()->MakeUnitsDisplayed(false);
    aDimAIS->SetDimensionAspect (anAspect);
    aDimAIS->SetSelToleranceForText2d(CONSTRAINT_TEXT_SELECTION_TOLERANCE);

    anAIS = aDimAIS;
  }
  else
  {
    // update presentation
    Handle(AIS_RadiusDimension) aDimAIS = Handle(AIS_RadiusDimension)::DownCast(anAIS);
    if (!aDimAIS.IsNull())
    {
      aDimAIS->SetMeasuredGeometry(aCircle->impl<gp_Circ>(), anAnchor->impl<gp_Pnt>());
      aDimAIS->SetCustomValue(aValue);
      aDimAIS->Redisplay(Standard_True);
    }
  }
  return anAIS;
}

void SketchPlugin_ConstraintRadius::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);
}
