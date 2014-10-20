// File:        PartSet_Tools.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Tools.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Document.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Pnt.h>

#include <GeomAPI_Dir.h>
#include <GeomAPI_XYZ.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_Constraint.h>

#include <ModuleBase_ViewerPrs.h>

#include <V3d_View.hxx>
#include <gp_Pln.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <Geom_Line.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

const double PRECISION_TOLERANCE = 0.000001;

gp_Pnt PartSet_Tools::convertClickToPoint(QPoint thePoint, Handle(V3d_View) theView)
{
  if (theView.IsNull())
    return gp_Pnt();

  V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
  theView->Eye(XEye, YEye, ZEye);

  theView->At(XAt, YAt, ZAt);
  gp_Pnt EyePoint(XEye, YEye, ZEye);
  gp_Pnt AtPoint(XAt, YAt, ZAt);

  gp_Vec EyeVector(EyePoint, AtPoint);
  gp_Dir EyeDir(EyeVector);

  gp_Pln PlaneOfTheView = gp_Pln(AtPoint, EyeDir);
  Standard_Real X, Y, Z;
  theView->Convert(thePoint.x(), thePoint.y(), X, Y, Z);
  gp_Pnt ConvertedPoint(X, Y, Z);

  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView, ConvertedPoint);
  gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(), ConvertedPointOnPlane.Y(),
                                     PlaneOfTheView);
  return ResultPoint;
}

void PartSet_Tools::convertTo2D(const gp_Pnt& thePoint, FeaturePtr theSketch,
Handle(V3d_View) theView,
                                double& theX, double& theY)
{
  if (!theSketch)
    return;

  AttributeDoublePtr anAttr;
  boost::shared_ptr<ModelAPI_Data> aData = theSketch->data();

  boost::shared_ptr<GeomDataAPI_Point> anOrigin = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));

  boost::shared_ptr<GeomDataAPI_Dir> aX = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> anY = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRY_ID()));

  gp_Pnt anOriginPnt(anOrigin->x(), anOrigin->y(), anOrigin->z());
  gp_Vec aVec(anOriginPnt, thePoint);

  if (!theView.IsNull()) {
    V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
    theView->Eye(XEye, YEye, ZEye);

    theView->At(XAt, YAt, ZAt);
    gp_Pnt EyePoint(XEye, YEye, ZEye);
    gp_Pnt AtPoint(XAt, YAt, ZAt);

    gp_Vec anEyeVec(EyePoint, AtPoint);
    anEyeVec.Normalize();

    boost::shared_ptr<GeomDataAPI_Dir> aNormal = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    gp_Vec aNormalVec(aNormal->x(), aNormal->y(), aNormal->z());

    double aDen = anEyeVec * aNormalVec;
    double aLVec = aDen != 0 ? aVec * aNormalVec / aDen : DBL_MAX;

    gp_Vec aDeltaVec = anEyeVec * aLVec;
    aVec = aVec - aDeltaVec;
  }
  theX = aVec.X() * aX->x() + aVec.Y() * aX->y() + aVec.Z() * aX->z();
  theY = aVec.X() * anY->x() + aVec.Y() * anY->y() + aVec.Z() * anY->z();
}

void PartSet_Tools::convertTo3D(const double theX, const double theY, FeaturePtr theSketch,
                                gp_Pnt& thePoint)
{
  if (!theSketch)
    return;

  boost::shared_ptr<ModelAPI_Data> aData = theSketch->data();

  boost::shared_ptr<GeomDataAPI_Point> aC = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aX = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aY = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRY_ID()));

  boost::shared_ptr<GeomAPI_XYZ> aSum = aC->pnt()->xyz()->added(aX->dir()->xyz()->multiplied(theX))
      ->added(aY->dir()->xyz()->multiplied(theY));

  boost::shared_ptr<GeomAPI_Pnt> aPoint = boost::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aSum));
  thePoint = gp_Pnt(aPoint->x(), aPoint->y(), aPoint->z());
}

FeaturePtr PartSet_Tools::nearestFeature(QPoint thePoint, Handle_V3d_View theView,
                                         FeaturePtr theSketch,
                                         const std::list<ModuleBase_ViewerPrs>& theFeatures)
{
  double aX, anY;
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(thePoint, theView);
  PartSet_Tools::convertTo2D(aPoint, theSketch, theView, aX, anY);

  FeaturePtr aFeature;
  std::list<ModuleBase_ViewerPrs>::const_iterator anIt = theFeatures.begin(), aLast = theFeatures
      .end();

  FeaturePtr aDeltaFeature;
  double aMinDelta = -1;
  ModuleBase_ViewerPrs aPrs;
  for (; anIt != aLast; anIt++) {
    aPrs = *anIt;
    if (!aPrs.object())
      continue;
    boost::shared_ptr<SketchPlugin_Feature> aSketchFeature = boost::dynamic_pointer_cast<
        SketchPlugin_Feature>(aPrs.object());
    if (!aSketchFeature)
      continue;
    double aDelta = aSketchFeature->distanceToPoint(
        boost::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, anY)));
    if (aMinDelta < 0 || aMinDelta > aDelta) {
      aMinDelta = aDelta;
      // TODO aDeltaFeature = aPrs.result();
    }
  }
  return aDeltaFeature;
}

boost::shared_ptr<ModelAPI_Document> PartSet_Tools::document()
{
  return ModelAPI_Session::get()->moduleDocument();
}

void PartSet_Tools::setFeaturePoint(FeaturePtr theFeature, double theX, double theY,
                                    const std::string& theAttribute)
{
  if (!theFeature)
    return;
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(theAttribute));
  if (aPoint)
    aPoint->setValue(theX, theY);
}

void PartSet_Tools::setFeatureValue(FeaturePtr theFeature, double theValue,
                                    const std::string& theAttribute)
{
  if (!theFeature)
    return;
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  AttributeDoublePtr anAttribute = boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(theAttribute));
  if (anAttribute)
    anAttribute->setValue(theValue);
}

double PartSet_Tools::featureValue(FeaturePtr theFeature, const std::string& theAttribute,
                                   bool& isValid)
{
  isValid = false;
  double aValue;
  if (theFeature) {
    boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
    AttributeDoublePtr anAttribute = boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        aData->attribute(theAttribute));
    if (anAttribute) {
      aValue = anAttribute->value();
      isValid = true;
    }
  }
  return aValue;
}

FeaturePtr PartSet_Tools::feature(FeaturePtr theFeature, const std::string& theAttribute,
                                  const std::string& theKind)
{
  FeaturePtr aFeature;
  if (!theFeature)
    return aFeature;

  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(theAttribute));
  if (anAttr) {
    aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->object());
    if (!theKind.empty() && aFeature && aFeature->getKind() != theKind) {
      aFeature = FeaturePtr();
    }
  }
  return aFeature;
}

void PartSet_Tools::createConstraint(CompositeFeaturePtr theSketch,
                                     boost::shared_ptr<GeomDataAPI_Point2D> thePoint1,
                                     boost::shared_ptr<GeomDataAPI_Point2D> thePoint2)
{
  FeaturePtr aFeature;
  if (theSketch) {
    aFeature = theSketch->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  } else {
    boost::shared_ptr<ModelAPI_Document> aDoc = document();
    aFeature = aDoc->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  }

  boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();

  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef1 = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRef1->setAttr(thePoint1);

  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef2 = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRef2->setAttr(thePoint2);

  if (aFeature)  // TODO: generate an error if feature was not created
    aFeature->execute();
}

void PartSet_Tools::setConstraints(CompositeFeaturePtr theSketch, FeaturePtr theFeature,
                                   const std::string& theAttribute, double theClickedX,
                                   double theClickedY)
{
  // find a feature point by the selection mode
  //boost::shared_ptr<GeomDataAPI_Point2D> aPoint = featurePoint(theMode);
  boost::shared_ptr<GeomDataAPI_Point2D> aFeaturePoint = boost::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(theFeature->data()->attribute(theAttribute));
  if (!aFeaturePoint)
    return;

  // get all sketch features. If the point with the given coordinates belong to any sketch feature,
  // the constraint is created between the feature point and the found sketch point
  boost::shared_ptr<ModelAPI_Data> aData = theSketch->data();
  boost::shared_ptr<ModelAPI_AttributeRefList> aRefList = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(aData->attribute(SketchPlugin_Sketch::FEATURES_ID()));

  std::list<ObjectPtr> aFeatures = aRefList->list();
  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  std::list<boost::shared_ptr<ModelAPI_Attribute> > anAttiributes;
  boost::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = boost::shared_ptr<GeomAPI_Pnt2d>(
      new GeomAPI_Pnt2d(theClickedX, theClickedY));
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
    // find the given point in the feature attributes
    anAttiributes = aFeature->data()->attributes(GeomDataAPI_Point2D::type());
    std::list<boost::shared_ptr<ModelAPI_Attribute> >::const_iterator anIt = anAttiributes.begin(),
        aLast = anAttiributes.end();
    boost::shared_ptr<GeomDataAPI_Point2D> aFPoint;
    for (; anIt != aLast && !aFPoint; anIt++) {
      boost::shared_ptr<GeomDataAPI_Point2D> aCurPoint = boost::dynamic_pointer_cast<
          GeomDataAPI_Point2D>(*anIt);
      if (aCurPoint && aCurPoint->pnt()->distance(aClickedPoint) < Precision::Confusion())
        aFPoint = aCurPoint;
    }
    if (aFPoint)
      PartSet_Tools::createConstraint(theSketch, aFPoint, aFeaturePoint);
  }
}

boost::shared_ptr<GeomAPI_Pln> PartSet_Tools::sketchPlane(CompositeFeaturePtr theSketch)
{
  boost::shared_ptr<GeomAPI_Pln> aPlane;
  double aA, aB, aC, aD;

  boost::shared_ptr<ModelAPI_Data> aData = theSketch->data();
  boost::shared_ptr<GeomDataAPI_Point> anOrigin = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aNormal = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::NORM_ID()));
  aA = aNormal->x();
  aB = aNormal->y();
  aC = aNormal->z();
  aD = 0;

  aPlane = boost::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(aA, aB, aC, aD));
  return aPlane;
}

boost::shared_ptr<GeomAPI_Pnt> PartSet_Tools::point3D(boost::shared_ptr<GeomAPI_Pnt2d> thePoint2D,
                                                      CompositeFeaturePtr theSketch)
{
  boost::shared_ptr<GeomAPI_Pnt> aPoint;
  if (!theSketch || !thePoint2D)
    return aPoint;

  boost::shared_ptr<GeomDataAPI_Point> aC = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
      theSketch->data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aX = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      theSketch->data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  boost::shared_ptr<GeomDataAPI_Dir> aY = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      theSketch->data()->attribute(SketchPlugin_Sketch::DIRY_ID()));

  return thePoint2D->to3D(aC->pnt(), aX->dir(), aY->dir());
}

bool PartSet_Tools::isConstraintFeature(const std::string& theKind)
{
  return theKind == SketchPlugin_ConstraintDistance::ID()
      || theKind == SketchPlugin_ConstraintLength::ID()
      || theKind == SketchPlugin_ConstraintRadius::ID()
      || theKind == SketchPlugin_ConstraintRigid::ID();
}
