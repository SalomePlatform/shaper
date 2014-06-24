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
      boost::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = 
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(feature()->data()->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT));
      aFlyOutAttr->setValue(boost::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(theX, theY)));

      boost::shared_ptr<GeomAPI_Pnt2d> aFlyOutPnt = aFlyOutAttr->pnt();

      aMode = SM_DonePoint;
    }
    break;
    default:
      break;
  }
  return aMode;
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

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_ConstraintDistancePrs::featurePoint
                                                     (const PartSet_SelectionMode& theMode)
{
  return boost::shared_ptr<GeomDataAPI_Point2D>();
}
