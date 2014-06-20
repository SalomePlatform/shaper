// File:        PartSet_FeaturePrs.h
// Created:     16 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_FeatureDistancePrs.h>
#include <PartSet_Tools.h>

#include <PartSet_FeatureLinePrs.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_ConstraintDistance.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Lin2d.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>

#include <Precision.hxx>

using namespace std;

PartSet_FeatureDistancePrs::PartSet_FeatureDistancePrs(FeaturePtr theSketch)
: PartSet_FeaturePrs(theSketch)
{
}

std::string PartSet_FeatureDistancePrs::getKind()
{
  return SKETCH_CONSTRAINT_DISTANCE_KIND;
}

bool PartSet_FeatureDistancePrs::setFeature(FeaturePtr theFeature, const PartSet_SelectionMode& theMode)
{
  bool aResult = false;
  if (feature() && theFeature && theFeature->getKind() == SKETCH_LINE_KIND && theMode == SM_FirstPoint)
  {
    // set length feature
    boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
    boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
    aRef->setFeature(theFeature);

    // set length value
    aData = theFeature->data();
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 =
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));

    double aLenght = aPoint1->pnt()->distance(aPoint2->pnt());
    PartSet_Tools::setFeatureValue(feature(), aLenght, CONSTRAINT_ATTR_VALUE);
    aResult = true;
  }
  return aResult;
}

PartSet_SelectionMode PartSet_FeatureDistancePrs::setPoint(double theX, double theY,
                                                         const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  switch (theMode)
  {
    case SM_SecondPoint: {
      boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
      boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
              boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
      FeaturePtr aFeature;
      if (anAttr) {
        aFeature = anAttr->feature();
        if (aFeature->getKind() != SKETCH_LINE_KIND) {
          aFeature = FeaturePtr();
        }
      }
      boost::shared_ptr<GeomAPI_Pnt2d> aPoint = boost::shared_ptr<GeomAPI_Pnt2d>
                                                             (new GeomAPI_Pnt2d(theX, theY));
      boost::shared_ptr<GeomAPI_Lin2d> aFeatureLin = PartSet_FeatureLinePrs::createLin2d(aFeature);
      boost::shared_ptr<GeomAPI_Pnt2d> aResult = aFeatureLin->project(aPoint);
      double aDistance = aPoint->distance(aResult);

      double aStartX, aStartY;
      PartSet_FeatureLinePrs::getLinePoint(aFeature, LINE_ATTR_START, aStartX, aStartY);

      if (!aFeatureLin->isRight(aPoint))
        aDistance = -aDistance;

      AttributeDoublePtr aFlyoutAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE));
      aFlyoutAttr->setValue(aDistance);

      aMode = SM_DonePoint;
    }
    break;
    default:
      break;
  }
  return aMode;
}

std::string PartSet_FeatureDistancePrs::getAttribute(const PartSet_SelectionMode& theMode) const
{
  return "";
}

PartSet_SelectionMode PartSet_FeatureDistancePrs::getNextMode(const std::string& theAttribute) const
{
  return SM_FirstPoint;
}

void PartSet_FeatureDistancePrs::move(double theDeltaX, double theDeltaY)
{
}

double PartSet_FeatureDistancePrs::distanceToPoint(FeaturePtr theFeature,
                                                 double theX, double theY)
{
  return 0;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_FeatureDistancePrs::findPoint(FeaturePtr theFeature,
                                                                           double theX, double theY)
{
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D;
  return aPoint2D;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_FeatureDistancePrs::featurePoint
                                                     (const PartSet_SelectionMode& theMode)
{
  return boost::shared_ptr<GeomDataAPI_Point2D>();
}
