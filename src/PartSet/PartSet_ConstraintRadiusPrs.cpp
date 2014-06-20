// File:        PartSet_FeaturePrs.h
// Created:     16 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_ConstraintRadiusPrs.h>
#include <PartSet_Tools.h>

#include <PartSet_FeatureLinePrs.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintRadius.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Lin2d.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>

#include <AIS_InteractiveObject.hxx>
#include <Precision.hxx>

using namespace std;

PartSet_ConstraintRadiusPrs::PartSet_ConstraintRadiusPrs(FeaturePtr theSketch)
: PartSet_FeaturePrs(theSketch)
{
}

std::string PartSet_ConstraintRadiusPrs::getKind()
{
  return SKETCH_CONSTRAINT_RADIUS_KIND;
}

bool PartSet_ConstraintRadiusPrs::setFeature(FeaturePtr theFeature, const PartSet_SelectionMode& theMode)
{
  bool aResult = false;
  if (feature() && theMode == SM_FirstPoint &&
      (theFeature && theFeature->getKind() == SKETCH_CIRCLE_KIND ||
       theFeature && theFeature->getKind() == SKETCH_ARC_KIND)) {
    // set length feature
    boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
    boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
    aRef->setFeature(theFeature);

    // set length value
    /*aData = theFeature->data();
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 =
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));

    double aLenght = aPoint1->pnt()->distance(aPoint2->pnt());
    */
    double aLenght = 50;
    PartSet_Tools::setFeatureValue(feature(), aLenght, CONSTRAINT_ATTR_VALUE);
    aResult = true;
  }
  return aResult;
}

PartSet_SelectionMode PartSet_ConstraintRadiusPrs::setPoint(double theX, double theY,
                                                         const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  switch (theMode)
  {
    case SM_SecondPoint: {
      boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
      /*boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
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
        */
      double aDistance = 40;
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

Handle(AIS_InteractiveObject) PartSet_ConstraintRadiusPrs::createPresentation(FeaturePtr theFeature,
                                                       FeaturePtr theSketch,
                                                       Handle(AIS_InteractiveObject) thePreviuos)
{
  Handle(AIS_InteractiveObject) anAIS = thePreviuos;
  if (!theFeature || !theSketch)
    return anAIS;
  /*
  boost::shared_ptr<ModelAPI_Data> aData = theSketch->data();
  boost::shared_ptr<GeomDataAPI_Point> anOrigin = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(aData->attribute(SKETCH_ATTR_ORIGIN));
  boost::shared_ptr<GeomDataAPI_Dir> aNormal = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_NORM));
  gp_Pln aPlane(aNormal->x(), aNormal->y(), aNormal->z(), 0);

  aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  if (!anAttr)
    return thePreviuos;
  FeaturePtr aFeature = anAttr->feature();
  if (!aFeature || aFeature->getKind() != SKETCH_LINE_KIND)
    return thePreviuos;

  boost::shared_ptr<ModelAPI_AttributeDouble> aFlyoutAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE));
  double aFlyout = aFlyoutAttr->value();

  aData = aFeature->data();
  if (!aData->isValid())
    return thePreviuos;

  boost::shared_ptr<GeomDataAPI_Point2D> aPointStart =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
  boost::shared_ptr<GeomDataAPI_Point2D> aPointEnd =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));

  gp_Pnt aPoint1, aPoint2;
  PartSet_Tools::convertTo3D(aPointStart->x(), aPointStart->y(), theSketch, aPoint1);
  PartSet_Tools::convertTo3D(aPointEnd->x(), aPointEnd->y(), theSketch, aPoint2);

  //Build dimension here
  gp_Pnt aP1 = aPoint1;
  gp_Pnt aP2 = aPoint2;
  if (aFlyout < 0) {
    aP1 = aPoint2;
    aP2 = aPoint1;
  }

  Handle(AIS_InteractiveObject) anAIS = thePreviuos;
  if (anAIS.IsNull())
  {
    Handle(AIS_LengthDimension) aDimAIS = new AIS_LengthDimension (aP1, aP2, aPlane);

    Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
    anAspect->MakeArrows3d (Standard_False);
    anAspect->MakeText3d(false);
    anAspect->TextAspect()->SetHeight(CONSTRAINT_TEXT_HEIGHT);
    anAspect->MakeTextShaded(false);
    aDimAIS->DimensionAspect()->MakeUnitsDisplayed(false);
    //if (isUnitsDisplayed)
    //{
    //  aDimAIS->SetDisplayUnits (aDimDlg->GetUnits ());
    //}
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
      aDimAIS->SetFlyout(aFlyout);

      aDimAIS->Redisplay(Standard_True);
    }
  }
*/
  return anAIS;
}

std::string PartSet_ConstraintRadiusPrs::getAttribute(const PartSet_SelectionMode& theMode) const
{
  return "";
}

PartSet_SelectionMode PartSet_ConstraintRadiusPrs::getNextMode(const std::string& theAttribute) const
{
  return SM_FirstPoint;
}

void PartSet_ConstraintRadiusPrs::move(double theDeltaX, double theDeltaY)
{
}

double PartSet_ConstraintRadiusPrs::distanceToPoint(FeaturePtr theFeature,
                                                 double theX, double theY)
{
  return 0;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_ConstraintRadiusPrs::findPoint(FeaturePtr theFeature,
                                                                           double theX, double theY)
{
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D;
  return aPoint2D;
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_ConstraintRadiusPrs::featurePoint
                                                     (const PartSet_SelectionMode& theMode)
{
  return boost::shared_ptr<GeomDataAPI_Point2D>();
}
