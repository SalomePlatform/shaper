// File:        PartSet_FeaturePrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_FeatureArcPrs.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Arc.h>

#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>

#include <Precision.hxx>

using namespace std;

PartSet_FeatureArcPrs::PartSet_FeatureArcPrs(FeaturePtr theSketch)
: PartSet_FeaturePrs(theSketch)
{
}

PartSet_SelectionMode PartSet_FeatureArcPrs::setPoint(double theX, double theY,
                                                       const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  switch (theMode)
  {
    case SM_FirstPoint: {
      PartSet_Tools::setFeaturePoint(feature(), theX, theY, ARC_ATTR_CENTER);
      aMode = SM_SecondPoint;
    }
    break;
    case SM_SecondPoint: {
      PartSet_Tools::setFeaturePoint(feature(), theX, theY, ARC_ATTR_START);
      aMode = SM_ThirdPoint;
   }
   break;
   case SM_ThirdPoint: {
      PartSet_Tools::setFeaturePoint(feature(), theX, theY, ARC_ATTR_END);
      aMode = SM_DonePoint;
   }
    break;
    default:
      break;
  }
  return aMode;
}

std::string PartSet_FeatureArcPrs::getAttribute(const PartSet_SelectionMode& theMode) const
{
  std::string aAttribute;
  switch (theMode)
  {
    case SM_FirstPoint:
      aAttribute = ARC_ATTR_CENTER;
    break;
    case SM_SecondPoint:
      aAttribute = ARC_ATTR_START;
    break;
    case SM_ThirdPoint:
      aAttribute = ARC_ATTR_END;
    break;
    default:
    break;
  }
  return aAttribute;
}

PartSet_SelectionMode PartSet_FeatureArcPrs::getNextMode(const std::string& theAttribute) const
{
  PartSet_SelectionMode aMode;

  if (theAttribute == ARC_ATTR_CENTER)
    aMode = SM_SecondPoint;
  else if (theAttribute == ARC_ATTR_START)
    aMode = SM_ThirdPoint;
  else if (theAttribute == ARC_ATTR_END)
    aMode = SM_DonePoint;
  return aMode;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_FeatureArcPrs::featurePoint
                                                     (const PartSet_SelectionMode& theMode)
{
  std::string aPointArg;
  switch (theMode)
  {
    case SM_FirstPoint:
      aPointArg = ARC_ATTR_CENTER;
      break;
    case SM_SecondPoint:
      aPointArg = ARC_ATTR_START;
      break;
    case SM_ThirdPoint:
      aPointArg = ARC_ATTR_END;
      break;
    default:
      break;
  }
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                              (aData->attribute(aPointArg));
  return aPoint;
}
