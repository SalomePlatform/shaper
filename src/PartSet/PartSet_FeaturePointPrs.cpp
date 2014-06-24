// File:        PartSet_FeaturePrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_FeaturePointPrs.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Point.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>

#include <Precision.hxx>

using namespace std;

PartSet_FeaturePointPrs::PartSet_FeaturePointPrs(FeaturePtr theSketch)
: PartSet_FeaturePrs(theSketch)
{
}

std::string PartSet_FeaturePointPrs::getKind()
{
  return SKETCH_POINT_KIND;
}

PartSet_SelectionMode PartSet_FeaturePointPrs::setPoint(double theX, double theY,
                                                       const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  switch (theMode)
  {
    case SM_FirstPoint: {
      PartSet_Tools::setFeaturePoint(feature(), theX, theY, POINT_ATTR_COORD);
      aMode = SM_DonePoint;
    }
    break;
    default:
      break;
  }
  return aMode;
}

std::string PartSet_FeaturePointPrs::getAttribute(const PartSet_SelectionMode& theMode) const
{
  std::string aAttribute;
  switch (theMode)
  {
    case SM_FirstPoint:
      aAttribute = POINT_ATTR_COORD;
    break;
    default:
    break;
  }
  return aAttribute;
}

PartSet_SelectionMode PartSet_FeaturePointPrs::getNextMode(const std::string& theAttribute) const
{
  PartSet_SelectionMode aMode;

  if (theAttribute == POINT_ATTR_COORD)
    aMode = SM_DonePoint;
  return aMode;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_FeaturePointPrs::findPoint(FeaturePtr theFeature,
                                                                          double theX, double theY)
{
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D;
  if (!theFeature || theFeature->getKind() != getKind())
    return aPoint2D;

  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(POINT_ATTR_COORD));
  if (fabs(aPoint->x() - theX) < Precision::Confusion() && fabs(aPoint->y() - theY) < Precision::Confusion() )
    aPoint2D = aPoint;

  return aPoint2D;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_FeaturePointPrs::featurePoint
                                                     (const PartSet_SelectionMode& theMode)
{
  std::string aPointArg;
  switch (theMode)
  {
    case SM_FirstPoint:
      aPointArg = POINT_ATTR_COORD;
      break;
    default:
      break;
  }
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                              (aData->attribute(aPointArg));
  return aPoint;
}
