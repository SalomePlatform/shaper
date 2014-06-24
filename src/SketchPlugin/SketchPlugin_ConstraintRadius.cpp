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

  data()->addAttribute(CONSTRAINT_ATTR_FLYOUT_VALUE, ModelAPI_AttributeDouble::type());
  data()->addAttribute(SKETCH_CONSTRAINT_ATTR_CIRCLE_POINT, GeomDataAPI_Point2D::type());
}

void SketchPlugin_ConstraintRadius::execute()
{
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
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SKETCH_CONSTRAINT_ATTR_CIRCLE_POINT));
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

