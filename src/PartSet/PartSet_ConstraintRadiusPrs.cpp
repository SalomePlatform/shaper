// File:        PartSet_FeaturePrs.h
// Created:     16 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_ConstraintRadiusPrs.h>
#include <PartSet_Tools.h>

#include <PartSet_FeatureCirclePrs.h>
#include <PartSet_FeatureArcPrs.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintRadius.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Dir.h>

#include <GeomAlgoAPI_EdgeBuilder.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_RadiusDimension.hxx>
#include <Precision.hxx>
#include <gp_Circ.hxx>
#include <V3d_View.hxx>

using namespace std;

PartSet_ConstraintRadiusPrs::PartSet_ConstraintRadiusPrs(FeaturePtr theSketch)
: PartSet_FeaturePrs(theSketch)
{
}

std::string PartSet_ConstraintRadiusPrs::getKind()
{
  return SKETCH_CONSTRAINT_RADIUS_KIND;
}

PartSet_SelectionMode PartSet_ConstraintRadiusPrs::setFeature(FeaturePtr theFeature, const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  if (!feature() || theMode != SM_FirstPoint || !theFeature) {
    return aMode;
  }
  std::string aKind = theFeature->getKind();
  if (aKind == SKETCH_CIRCLE_KIND || aKind == SKETCH_ARC_KIND) {
    // set length feature
    boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
    boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
    aRef->setFeature(theFeature);

    double aLength = 50;
    bool isValid;
    if (aKind == SKETCH_CIRCLE_KIND) {
      aLength = PartSet_Tools::featureValue(theFeature, CIRCLE_ATTR_RADIUS, isValid);
    }
    else if (aKind == SKETCH_ARC_KIND) {
      aLength = PartSet_FeatureArcPrs::radius(theFeature);
    }

    PartSet_Tools::setFeatureValue(feature(), aLength, CONSTRAINT_ATTR_VALUE);
    aMode = SM_LastPoint;
  }
  return aMode;
}

PartSet_SelectionMode PartSet_ConstraintRadiusPrs::setPoint(double theX, double theY,
                                                         const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  switch (theMode)
  {
    case SM_LastPoint: {
      boost::shared_ptr<ModelAPI_Data> aData = feature()->data();

      boost::shared_ptr<GeomAPI_Pnt2d> aPoint = boost::shared_ptr<GeomAPI_Pnt2d>
                                                             (new GeomAPI_Pnt2d(theX, theY));

      PartSet_Tools::setFeaturePoint(feature(), theX, theY, SKETCH_CONSTRAINT_ATTR_CIRCLE_POINT);

      //double aDistance = 40;
      //AttributeDoublePtr aFlyoutAttr = 
      //    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE));
      //aFlyoutAttr->setValue(aDistance);

      aMode = SM_LastPoint;
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

  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  if (!anAttr)
    return anAIS;
  FeaturePtr aFeature = anAttr->feature();
  std::string aKind = aFeature ? aFeature->getKind() : "";
  if (aKind != SKETCH_CIRCLE_KIND && aKind != SKETCH_ARC_KIND)
    return anAIS;

  //boost::shared_ptr<ModelAPI_AttributeDouble> aFlyoutAttr = 
  //        boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE));
  //double aFlyout = aFlyoutAttr->value();
  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_VALUE));
  double aValue = aValueAttr->value();

  // an anchor point
  boost::shared_ptr<GeomDataAPI_Point2D> aAnchorAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(theFeature->data()->attribute
                                                        (SKETCH_CONSTRAINT_ATTR_CIRCLE_POINT));
  boost::shared_ptr<GeomAPI_Pnt2d> anAnchor2D = aAnchorAttr->pnt();
  boost::shared_ptr<GeomAPI_Pnt> anAnchor = PartSet_Tools::point3D(anAnchor2D, theSketch);
  gp_Pnt anAnchorPoint = anAnchor->impl<gp_Pnt>();

  std::string aCenterArgument;
  double aRadius;
  if (aKind == SKETCH_CIRCLE_KIND) {
    aCenterArgument = CIRCLE_ATTR_CENTER;
    bool isValid;
    aRadius = PartSet_Tools::featureValue(aFeature, CIRCLE_ATTR_RADIUS, isValid);
  }
  else if (aKind == SKETCH_ARC_KIND) {
    aCenterArgument = ARC_ATTR_CENTER;
    aRadius = PartSet_FeatureArcPrs::radius(aFeature);
  }

  // a circle
  boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFeature->data()->attribute(aCenterArgument));
  boost::shared_ptr<GeomAPI_Pnt2d> aCenter2D = aCenterAttr->pnt();
  boost::shared_ptr<GeomAPI_Pnt> aCenter = PartSet_Tools::point3D(aCenter2D, theSketch);

  boost::shared_ptr<GeomDataAPI_Dir> aNDir = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(theSketch->data()->attribute(SKETCH_ATTR_NORM));
  boost::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));
  const gp_Dir& aDir = aNormal->impl<gp_Dir>();

  gp_Circ aCircle = gp_Circ(gp_Ax2(aCenter->impl<gp_Pnt>(), aDir), aRadius);
  //boost::shared_ptr<GeomAPI_Shape> aShape;
  //aShape = GeomAlgoAPI_EdgeBuilder::line(aCenter, anAnchor);
    //boost::shared_ptr<GeomAPI_Pnt> theStart, boost::shared_ptr<GeomAPI_Pnt> theEnd)

  if (anAIS.IsNull())
  {
    Handle(AIS_RadiusDimension) aDimAIS = new AIS_RadiusDimension(aCircle, anAnchorPoint);
    aDimAIS->SetCustomValue(aValue);
    //Handle(AIS_RadiusDimension) aDimAIS = new AIS_RadiusDimension(aShape->impl<TopoDS_Shape>());

    Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
    anAspect->MakeArrows3d (Standard_False);
    anAspect->MakeText3d(false);
    anAspect->TextAspect()->SetHeight(CONSTRAINT_TEXT_HEIGHT);
    anAspect->MakeTextShaded(false);
    aDimAIS->DimensionAspect()->MakeUnitsDisplayed(false);
    aDimAIS->SetDimensionAspect (anAspect);
    //aDimAIS->SetFlyout(aFlyout);
    aDimAIS->SetSelToleranceForText2d(CONSTRAINT_TEXT_SELECTION_TOLERANCE);

    anAIS = aDimAIS;
  }
  else {
    // update presentation
    Handle(AIS_RadiusDimension) aDimAIS = Handle(AIS_RadiusDimension)::DownCast(anAIS);
    if (!aDimAIS.IsNull()) {
      gp_Pnt anAPoint(anAnchorPoint.X(),anAnchorPoint.Y(),anAnchorPoint.Z());

      aDimAIS->SetMeasuredGeometry(aCircle, anAnchorPoint);
      aDimAIS->SetCustomValue(aValue);
      //aDimAIS->SetMeasuredGeometry(aShape->impl<TopoDS_Shape>());
      //aDimAIS->SetFlyout(aFlyout);
      aDimAIS->Redisplay(Standard_True);
    }
  }
  return anAIS;
}

void PartSet_ConstraintRadiusPrs::projectPointOnFeature(FeaturePtr theFeature, FeaturePtr theSketch,
                                                        gp_Pnt& thePoint, Handle(V3d_View) theView,
                                                        double& theX, double& theY)
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  if (!anAttr)
    return;
  FeaturePtr aFeature = anAttr->feature();
  if (!aFeature)
    return;

  gp_Circ aCircle;
  gp_Pnt anAnchorPoint;
  if (aFeature->getKind() == SKETCH_CIRCLE_KIND) {
    PartSet_FeatureCirclePrs::projectPointOnFeature(aFeature, theSketch, thePoint, theView, theX, theY);
  }
  else if (aFeature->getKind() == SKETCH_ARC_KIND) {
    PartSet_FeatureArcPrs::projectPointOnFeature(aFeature, theSketch, thePoint, theView, theX, theY);
  }
  // TODO: a bug in AIS_RadiusDimension:
  // The anchor point can't be myCirc.Location() - an exception is raised.
  // But we need exactly this case...
  // We want to show a radius dimension starting from the circle centre and 
  // ending at the user-defined point.
  // Also, if anchor point coincides with myP2, the radius dimension is not displayed at all.
  double aDelta = 1/1000.0;
  theX += aDelta;
  theY += aDelta;
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
