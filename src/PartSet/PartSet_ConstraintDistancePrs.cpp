// File:        PartSet_FeaturePrs.h
// Created:     16 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_ConstraintDistancePrs.h>
#include <PartSet_Tools.h>

#include <PartSet_FeatureLinePrs.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_ConstraintDistance.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pln.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_LengthDimension.hxx>

#include <Precision.hxx>
#include <gp_Pln.hxx>

using namespace std;

PartSet_ConstraintDistancePrs::PartSet_ConstraintDistancePrs(FeaturePtr theSketch)
: PartSet_FeaturePrs(theSketch)
{
}

std::string PartSet_ConstraintDistancePrs::getKind()
{
  return SKETCH_CONSTRAINT_DISTANCE_KIND;
}

PartSet_SelectionMode PartSet_ConstraintDistancePrs::setFeature(FeaturePtr theFeature, const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  if (!feature() || !theFeature)
    return aMode;

  std::string anArgument;
  if (theMode == SM_FirstPoint) {
    anArgument = CONSTRAINT_ATTR_ENTITY_A;
    aMode = SM_SecondPoint;
  }
  else if (theMode == SM_SecondPoint) {
    anArgument = CONSTRAINT_ATTR_ENTITY_B;
    aMode = SM_LastPoint;
  }
  if (theFeature->getKind() == SKETCH_POINT_KIND)
  {
    // set length feature
    boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
    boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(anArgument));
    aRef->setFeature(theFeature);
  }

  if (theMode == SM_SecondPoint) {
    // set length value
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint_A = getFeaturePoint(feature(), CONSTRAINT_ATTR_ENTITY_A);
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint_B = getFeaturePoint(feature(), CONSTRAINT_ATTR_ENTITY_B);

    if (aPoint_A && aPoint_B) {
      double aLenght = aPoint_A->pnt()->distance(aPoint_B->pnt());
      PartSet_Tools::setFeatureValue(feature(), aLenght, CONSTRAINT_ATTR_VALUE);
    }
  }

  return aMode;
}

PartSet_SelectionMode PartSet_ConstraintDistancePrs::setPoint(double theX, double theY,
                                                         const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  switch (theMode)
  {
    case SM_LastPoint: {
      boost::shared_ptr<GeomDataAPI_Point2D> aPoint_A = getFeaturePoint(feature(),
                                                                        CONSTRAINT_ATTR_ENTITY_A);
      boost::shared_ptr<GeomDataAPI_Point2D> aPoint_B = getFeaturePoint(feature(),
                                                                        CONSTRAINT_ATTR_ENTITY_B);

      boost::shared_ptr<GeomAPI_Pnt2d> aPoint = boost::shared_ptr<GeomAPI_Pnt2d>
                                                             (new GeomAPI_Pnt2d(theX, theY));
      boost::shared_ptr<GeomAPI_Lin2d> aFeatureLin = boost::shared_ptr<GeomAPI_Lin2d>
                                        (new GeomAPI_Lin2d(aPoint_A->x(), aPoint_A->y(),
                                                           aPoint_B->x(), aPoint_B->y()));
      boost::shared_ptr<GeomAPI_Pnt2d> aResult = aFeatureLin->project(aPoint);
      double aDistance = aPoint->distance(aResult);

      if (!aFeatureLin->isRight(aPoint))
        aDistance = -aDistance;

      AttributeDoublePtr aFlyoutAttr = boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>
                                      (feature()->data()->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE));
      aFlyoutAttr->setValue(aDistance);

      aMode = SM_DonePoint;
    }
    break;
    default:
      break;
  }
  return aMode;
}

Handle(AIS_InteractiveObject) PartSet_ConstraintDistancePrs::createPresentation(FeaturePtr theFeature,
                                                       FeaturePtr theSketch,
                                                       Handle(AIS_InteractiveObject) thePreviuos)
{
  Handle(AIS_InteractiveObject) anAIS = thePreviuos;
  if (!theFeature || !theSketch)
    return anAIS;
  boost::shared_ptr<GeomAPI_Pln> aGPlane = PartSet_Tools::sketchPlane(theSketch);
  gp_Pln aPlane = aGPlane->impl<gp_Pln>();

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint_A = getFeaturePoint(theFeature, CONSTRAINT_ATTR_ENTITY_A);
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint_B = getFeaturePoint(theFeature, CONSTRAINT_ATTR_ENTITY_B);
  if (!aPoint_A || !aPoint_B)
    return anAIS;

  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeDouble> aFlyoutAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE));
  double aFlyout = aFlyoutAttr->value();

  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_VALUE));
  double aValue = aValueAttr->value();

  gp_Pnt aPoint1, aPoint2;
  PartSet_Tools::convertTo3D(aPoint_A->x(), aPoint_A->y(), theSketch, aPoint1);
  PartSet_Tools::convertTo3D(aPoint_B->x(), aPoint_B->y(), theSketch, aPoint2);

  //Build dimension here
  gp_Pnt aP1 = aPoint1;
  gp_Pnt aP2 = aPoint2;
  if (aFlyout < 0) {
    aP1 = aPoint2;
    aP2 = aPoint1;
  }

  if (anAIS.IsNull())
  {
    Handle(AIS_LengthDimension) aDimAIS = new AIS_LengthDimension(aP1, aP2, aPlane);
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
      aDimAIS->SetMeasuredGeometry(aPoint1, aPoint2, aPlane);
      aDimAIS->SetCustomValue(aValue);
      aDimAIS->SetFlyout(aFlyout);

      aDimAIS->Redisplay(Standard_True);
    }
  }
  return anAIS;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_ConstraintDistancePrs::getFeaturePoint
                                                               (FeaturePtr theFeature,
                                                                const std::string& theAttribute)
{
  boost::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  FeaturePtr aFeature = PartSet_Tools::feature(theFeature, theAttribute, SKETCH_POINT_KIND);
  if (aFeature)
    aPointAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                           (aFeature->data()->attribute(POINT_ATTR_COORD));
  return aPointAttr;
}

std::string PartSet_ConstraintDistancePrs::getAttribute(const PartSet_SelectionMode& theMode) const
{
  return "";
}

PartSet_SelectionMode PartSet_ConstraintDistancePrs::getNextMode(const std::string& theAttribute) const
{
  return SM_FirstPoint;
}

double PartSet_ConstraintDistancePrs::distanceToPoint(FeaturePtr theFeature,
                                                 double theX, double theY)
{
  return 0;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_ConstraintDistancePrs::findPoint(FeaturePtr theFeature,
                                                                           double theX, double theY)
{
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D;
  return aPoint2D;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_ConstraintDistancePrs::featurePoint
                                                     (const PartSet_SelectionMode& theMode)
{
  return boost::shared_ptr<GeomDataAPI_Point2D>();
}
