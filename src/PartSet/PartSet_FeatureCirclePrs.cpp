// File:        PartSet_FeaturePrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_FeatureCirclePrs.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Circle.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>

#include <Precision.hxx>

using namespace std;

PartSet_FeatureCirclePrs::PartSet_FeatureCirclePrs(FeaturePtr theSketch)
: PartSet_FeaturePrs(theSketch)
{
}

std::string PartSet_FeatureCirclePrs::getKind()
{
  return SKETCH_CIRCLE_KIND;
}

PartSet_SelectionMode PartSet_FeatureCirclePrs::setPoint(double theX, double theY,
                                                         const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  switch (theMode)
  {
    case SM_FirstPoint: {
      PartSet_Tools::setFeaturePoint(feature(), theX, theY, CIRCLE_ATTR_CENTER);
      aMode = SM_SecondPoint;
    }
    break;
    case SM_SecondPoint: {
      boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
      boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                                  (aData->attribute(CIRCLE_ATTR_CENTER));
      boost::shared_ptr<GeomAPI_Pnt2d> aCoordPoint(new GeomAPI_Pnt2d(theX, theY));
      double aRadius = aCoordPoint->distance(aPoint->pnt());
      PartSet_Tools::setFeatureValue(feature(), aRadius, CIRCLE_ATTR_RADIUS);

      aMode = SM_DonePoint;
   }
    break;
    default:
      break;
  }
  return aMode;
}

std::string PartSet_FeatureCirclePrs::getAttribute(const PartSet_SelectionMode& theMode) const
{
  std::string aAttribute;
  switch (theMode)
  {
    case SM_FirstPoint:
      aAttribute = CIRCLE_ATTR_CENTER;
    break;
    case SM_SecondPoint:
      aAttribute = CIRCLE_ATTR_RADIUS;
    break;
    default:
    break;
  }
  return aAttribute;
}

PartSet_SelectionMode PartSet_FeatureCirclePrs::getNextMode(const std::string& theAttribute) const
{
  PartSet_SelectionMode aMode;

  if (theAttribute == CIRCLE_ATTR_CENTER)
    aMode = SM_SecondPoint;
  else if (theAttribute == CIRCLE_ATTR_RADIUS)
    aMode = SM_DonePoint;
  return aMode;
}

double PartSet_FeatureCirclePrs::distanceToPoint(FeaturePtr theFeature,
                                                 double theX, double theY)
{
  double aDelta = 0;
  if (!theFeature || theFeature->getKind() != getKind())
    return aDelta;

  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CIRCLE_ATTR_CENTER));

  boost::shared_ptr<GeomAPI_Pnt2d> aPoint2d(new GeomAPI_Pnt2d(theX, theY));
  return aPoint->pnt()->distance(aPoint2d);
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_FeatureCirclePrs::findPoint(FeaturePtr theFeature,
                                                                           double theX, double theY)
{
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D;
  if (!theFeature || theFeature->getKind() != getKind())
    return aPoint2D;

  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CIRCLE_ATTR_CENTER));
  if (fabs(aPoint->x() - theX) < Precision::Confusion() && fabs(aPoint->y() - theY) < Precision::Confusion() )
    aPoint2D = aPoint;

  return aPoint2D;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_FeatureCirclePrs::featurePoint
                                                     (const PartSet_SelectionMode& theMode)
{
  std::string aPointArg;
  switch (theMode)
  {
    case SM_FirstPoint:
      aPointArg = CIRCLE_ATTR_CENTER;
      break;
    default:
      break;
  }
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                              (aData->attribute(aPointArg));
  return aPoint;
}
