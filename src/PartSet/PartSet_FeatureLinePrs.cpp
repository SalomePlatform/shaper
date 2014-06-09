// File:        PartSet_FeaturePrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_FeatureLinePrs.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Constraint.h>

#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>

#include <Precision.hxx>
#include <V3d_View.hxx>

using namespace std;

PartSet_FeatureLinePrs::PartSet_FeatureLinePrs(FeaturePtr theSketch)
: PartSet_FeaturePrs(theSketch)
{
}

void PartSet_FeatureLinePrs::initFeature(FeaturePtr theFeature)
{
  if (feature() && theFeature)
  {
    // use the last point of the previous feature as the first of the new one
    boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
    boost::shared_ptr<GeomDataAPI_Point2D> anInitPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                                  (aData->attribute(LINE_ATTR_END));
    PartSet_Tools::setFeaturePoint(feature(), anInitPoint->x(), anInitPoint->y(), LINE_ATTR_START);
    PartSet_Tools::setFeaturePoint(feature(), anInitPoint->x(), anInitPoint->y(), LINE_ATTR_END);

    aData = feature()->data();
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                                 (aData->attribute(LINE_ATTR_START));
    PartSet_Tools::createConstraint(sketch(), anInitPoint, aPoint);
  }
}

PartSet_SelectionMode PartSet_FeatureLinePrs::setPoint(double theX, double theY,
                                                       const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  switch (theMode)
  {
    case SM_FirstPoint: {
      PartSet_Tools::setFeaturePoint(feature(), theX, theY, LINE_ATTR_START);
      PartSet_Tools::setFeaturePoint(feature(), theX, theY, LINE_ATTR_END);
      aMode = SM_SecondPoint;
    }
    break;
    case SM_SecondPoint: {
      PartSet_Tools::setFeaturePoint(feature(), theX, theY, LINE_ATTR_END);
      aMode = SM_DonePoint;
   }
    break;
    default:
      break;
  }
  return aMode;
}

std::string PartSet_FeatureLinePrs::getAttribute(const PartSet_SelectionMode& theMode) const
{
  std::string aAttribute;
  switch (theMode)
  {
    case SM_FirstPoint:
      aAttribute = LINE_ATTR_START;
    break;
    case SM_SecondPoint:
      aAttribute = LINE_ATTR_END;
    break;
    default:
    break;
  }
  return aAttribute;
}

PartSet_SelectionMode PartSet_FeatureLinePrs::getNextMode(const std::string& theAttribute) const
{
  PartSet_SelectionMode aMode;

  if (theAttribute == LINE_ATTR_START)
    aMode = SM_SecondPoint;
  else if (theAttribute == LINE_ATTR_END)
    aMode = SM_DonePoint;
  return aMode;
}

void PartSet_FeatureLinePrs::projectPointOnLine(FeaturePtr theFeature,
                                                const PartSet_SelectionMode& theMode,
                                                const gp_Pnt& thePoint, Handle(V3d_View) theView,
                                                double& theX, double& theY)
{
  if (theFeature) {
    double X0, X1, X2, X3;
    double Y0, Y1, Y2, Y3;
    PartSet_Tools::getLinePoint(theFeature, LINE_ATTR_START, X2, Y2);
    PartSet_Tools::getLinePoint(theFeature, LINE_ATTR_END, X3, Y3);
    PartSet_Tools::convertTo2D(thePoint, sketch(), theView, X1, Y1);

    switch (theMode) {
      case SM_FirstPoint:
        PartSet_Tools::projectPointOnLine(X2, Y2, X3, Y3, X1, Y1, theX, theY);
      break;
      case SM_SecondPoint: {
        PartSet_Tools::getLinePoint(feature(), LINE_ATTR_START, X0, Y0);
        PartSet_Tools::intersectLines(X0, Y0, X1, Y1, X2, Y2, X3, Y3, theX, theY);
      }
      break;
      default:
      break;
    }
  }
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_FeatureLinePrs::featurePoint
                                                     (const PartSet_SelectionMode& theMode)
{
  std::string aPointArg;
  switch (theMode)
  {
    case SM_FirstPoint:
      aPointArg = LINE_ATTR_START;
      break;
    case SM_SecondPoint:
      aPointArg = LINE_ATTR_END;
      break;
    default:
      break;
  }
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                              (aData->attribute(aPointArg));
  return aPoint;
}
