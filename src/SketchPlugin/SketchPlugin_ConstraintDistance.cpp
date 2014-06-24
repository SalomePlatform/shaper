// File:    SketchPlugin_ConstraintDistance.cpp
// Created: 23 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintDistance.h"
#include <SketchPlugin_Point.h>

#include <GeomAPI_Lin2D.h>
#include <GeomAPI_Pnt2D.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <AIS_LengthDimension.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

/// Obtain the point object from specified constraint parameter
static boost::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(
            DataPtr             theData,
            const std::string&  theAttribute);


SketchPlugin_ConstraintDistance::SketchPlugin_ConstraintDistance()
{
}

void SketchPlugin_ConstraintDistance::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_VALUE,    ModelAPI_AttributeDouble::type());
  data()->addAttribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT, GeomDataAPI_Point2D::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_B, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintDistance::execute()
{
}

Handle(AIS_InteractiveObject) SketchPlugin_ConstraintDistance::getAISShape(
  Handle_AIS_InteractiveObject thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();

  DataPtr aData = data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint_A = getFeaturePoint(aData, CONSTRAINT_ATTR_ENTITY_A);
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint_B = getFeaturePoint(aData, CONSTRAINT_ATTR_ENTITY_B);
  if (!aPoint_A || !aPoint_B)
    return thePrevious;

  // fly out calculation
  boost::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT));
  boost::shared_ptr<GeomAPI_Pnt2d> aFlyOutPnt = aFlyOutAttr->pnt();

  boost::shared_ptr<GeomAPI_Lin2d> aFeatureLin = 
    boost::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aPoint_A->x(), aPoint_A->y(),
                                                       aPoint_B->x(), aPoint_B->y()));
  boost::shared_ptr<GeomAPI_Pnt2d> aResult = aFeatureLin->project(aFlyOutPnt);
  double aDistance = aFlyOutPnt->distance(aResult);

  if (!aFeatureLin->isRight(aFlyOutPnt))
    aDistance = -aDistance;
  double aFlyout = aDistance;

  //Build dimension here
  boost::shared_ptr<GeomAPI_Pnt> aPoint1 = sketch()->to3D(aPoint_A->x(), aPoint_A->y());
  boost::shared_ptr<GeomAPI_Pnt> aPoint2 = sketch()->to3D(aPoint_B->x(), aPoint_B->y());
  if (aFlyout < 0)
    aPoint1.swap(aPoint2);

  // value calculation
  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_VALUE));
  double aValue = aValueAttr->value();
  if (aValue == 0) { // TODO! the default value
    aValue = aPoint1->distance(aPoint2);
  }

  Handle(AIS_InteractiveObject) anAIS = thePrevious;
  if (anAIS.IsNull())
  {
    Handle(AIS_LengthDimension) aDimAIS = 
      new AIS_LengthDimension(aPoint1->impl<gp_Pnt>(), aPoint2->impl<gp_Pnt>(), aPlane->impl<gp_Pln>());
    aDimAIS->SetCustomValue(aValue);

    Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
    anAspect->MakeArrows3d (Standard_False);
    anAspect->MakeText3d(false);
    anAspect->TextAspect()->SetHeight(CONSTRAINT_TEXT_HEIGHT);
    anAspect->MakeTextShaded(false);
    aDimAIS->DimensionAspect()->MakeUnitsDisplayed(false);
    aDimAIS->SetDimensionAspect (anAspect);
    aDimAIS->SetFlyout(aFlyout);
    aDimAIS->SetSelToleranceForText2d(CONSTRAINT_TEXT_SELECTION_TOLERANCE);

    anAIS = aDimAIS;
  }
  else {
    // update presentation
    Handle(AIS_LengthDimension) aDimAIS = Handle(AIS_LengthDimension)::DownCast(anAIS);
    if (!aDimAIS.IsNull()) {
      aDimAIS->SetMeasuredGeometry(aPoint1->impl<gp_Pnt>(), aPoint2->impl<gp_Pnt>(), aPlane->impl<gp_Pln>());
      aDimAIS->SetCustomValue(aValue);
      aDimAIS->SetFlyout(aFlyout);

      aDimAIS->Redisplay(Standard_True);
    }
  }
  return anAIS;
}


boost::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                       const std::string& theAttribute)
{
  boost::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theData)
    return aPointAttr;

  FeaturePtr aFeature;
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr)
    aFeature = anAttr->feature();

  if (aFeature && aFeature->getKind() == SKETCH_POINT_KIND)
    aPointAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                           (aFeature->data()->attribute(POINT_ATTR_COORD));
  return aPointAttr;
}

