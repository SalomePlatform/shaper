// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Tools.cpp
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Tools.h>
#include <PartSet_Module.h>
#include <PartSet_SketcherMgr.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketcherPrs_Tools.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Vertex.h>

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
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ViewerPrs.h>

#include <V3d_View.hxx>
#include <gp_Pln.hxx>
#include <gp_Circ.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <Geom_Line.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <AIS_InteractiveObject.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>

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
  std::shared_ptr<ModelAPI_Data> aData = theSketch->data();

  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));

  std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_XYZ> anY = aNorm->xyz()->cross(aX->xyz());

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

    std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
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

std::shared_ptr<GeomAPI_Pnt2d> PartSet_Tools::convertTo2D(FeaturePtr theSketch, 
                                                    const std::shared_ptr<GeomAPI_Pnt>& thePnt)
{
  std::shared_ptr<GeomAPI_Pnt2d> aRes;
  if (theSketch->getKind() != SketchPlugin_Sketch::ID())
    return aRes;
  std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      theSketch->data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      theSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      theSketch->data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));
  return thePnt->to2D(aC->pnt(), aX->dir(), aY);
}


std::shared_ptr<GeomAPI_Pnt> PartSet_Tools::convertTo3D(const double theX, const double theY, FeaturePtr theSketch)
{
  std::shared_ptr<ModelAPI_Data> aData = theSketch->data();

  std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));

  std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = 
    std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(theX, theY));

  return aPnt2d->to3D(aC->pnt(), aX->dir(), aY);
}

std::shared_ptr<ModelAPI_Document> PartSet_Tools::document()
{
  return ModelAPI_Session::get()->moduleDocument();
}

/*std::shared_ptr<GeomDataAPI_Point2D> PartSet_Tools::getFeaturePoint(FeaturePtr theFeature,
                                                                      double theX, double theY)
{
  std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = std::shared_ptr<GeomAPI_Pnt2d>(
                                                                 new GeomAPI_Pnt2d(theX, theY));
  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttiributes =
                                    theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<std::shared_ptr<ModelAPI_Attribute> >::const_iterator anIt = anAttiributes.begin(),
                                                                    aLast = anAttiributes.end();
  std::shared_ptr<GeomDataAPI_Point2D> aFPoint;
  for (; anIt != aLast && !aFPoint; anIt++) {
    std::shared_ptr<GeomDataAPI_Point2D> aCurPoint = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(*anIt);
    if (aCurPoint && aCurPoint->pnt()->distance(aClickedPoint) < Precision::Confusion())
      aFPoint = aCurPoint;
  }

  return aFPoint;
}*/

void PartSet_Tools::setFeatureValue(FeaturePtr theFeature, double theValue,
                                    const std::string& theAttribute)
{
  if (!theFeature)
    return;
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  AttributeDoublePtr anAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(theAttribute));
  if (anAttribute)
    anAttribute->setValue(theValue);
}

double PartSet_Tools::featureValue(FeaturePtr theFeature, const std::string& theAttribute,
                                   bool& isValid)
{
  isValid = false;
  double aValue = 0;
  if (theFeature) {
    std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
    AttributeDoublePtr anAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
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

  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(theAttribute));
  if (anAttr) {
    aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->object());
    if (!theKind.empty() && aFeature && aFeature->getKind() != theKind) {
      aFeature = FeaturePtr();
    }
  }
  return aFeature;
}

void PartSet_Tools::createConstraint(CompositeFeaturePtr theSketch,
                                     std::shared_ptr<GeomDataAPI_Point2D> thePoint1,
                                     std::shared_ptr<GeomDataAPI_Point2D> thePoint2)
{
  FeaturePtr aFeature;
  if (theSketch) {
    aFeature = theSketch->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  } else {
    std::shared_ptr<ModelAPI_Document> aDoc = document();
    aFeature = aDoc->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  }

  std::shared_ptr<ModelAPI_Data> aData = aFeature->data();

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef1 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRef1->setAttr(thePoint1);

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef2 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRef2->setAttr(thePoint2);

  if (aFeature)  // TODO: generate an error if feature was not created
    aFeature->execute();
}

/*std::shared_ptr<GeomDataAPI_Point2D> PartSet_Tools::
  findAttributePoint(CompositeFeaturePtr theSketch, double theX, double theY,
  double theTolerance, const QList<FeaturePtr>& theIgnore)
{
  std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = std::shared_ptr<GeomAPI_Pnt2d>(
      new GeomAPI_Pnt2d(theX, theY));

  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttiributes;
  for (int i = 0; i < theSketch->numberOfSubs(); i++) {
    FeaturePtr aFeature = theSketch->subFeature(i);
    if (!theIgnore.contains(aFeature)) {
      anAttiributes = aFeature->data()->attributes(GeomDataAPI_Point2D::typeId());

      std::list<std::shared_ptr<ModelAPI_Attribute> >::const_iterator anIt;
      for (anIt = anAttiributes.cbegin(); anIt != anAttiributes.cend(); ++anIt) {
        std::shared_ptr<GeomDataAPI_Point2D> aCurPoint = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);
        double x = aCurPoint->x();
        double y = aCurPoint->y();
        if (aCurPoint && (aCurPoint->pnt()->distance(aClickedPoint) < theTolerance)) {
          return aCurPoint;
        }
      }
    }
  }
  return std::shared_ptr<GeomDataAPI_Point2D>();
}*/


void PartSet_Tools::setConstraints(CompositeFeaturePtr theSketch, FeaturePtr theFeature,
                                   const std::string& theAttribute, double theClickedX,
                                   double theClickedY)
{
  // find a feature point by the selection mode
  //std::shared_ptr<GeomDataAPI_Point2D> aPoint = featurePoint(theMode);
  std::shared_ptr<GeomDataAPI_Point2D> aFeaturePoint = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(theFeature->data()->attribute(theAttribute));
  if (!aFeaturePoint)
    return;

  // get all sketch features. If the point with the given coordinates belong to any sketch feature,
  // the constraint is created between the feature point and the found sketch point
  std::shared_ptr<ModelAPI_Data> aData = theSketch->data();
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(aData->attribute(SketchPlugin_Sketch::FEATURES_ID()));

  std::list<ObjectPtr> aFeatures = aRefList->list();
  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttiributes;
  std::shared_ptr<GeomAPI_Pnt2d> aClickedPoint = std::shared_ptr<GeomAPI_Pnt2d>(
      new GeomAPI_Pnt2d(theClickedX, theClickedY));
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
    if (!aFeature.get() || theFeature == aFeature)
      continue;
    // find the given point in the feature attributes
    anAttiributes = aFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
    std::list<std::shared_ptr<ModelAPI_Attribute> >::const_iterator anIt = anAttiributes.begin(),
        aLast = anAttiributes.end();
    std::shared_ptr<GeomDataAPI_Point2D> aFPoint;
    for (; anIt != aLast && !aFPoint; anIt++) {
      std::shared_ptr<GeomDataAPI_Point2D> aCurPoint = 
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);
      if (aCurPoint && (aCurPoint->pnt()->distance(aClickedPoint) < Precision::Confusion())) {
        aFPoint = aCurPoint;
        break;
      }
    }
    if (aFPoint)
      PartSet_Tools::createConstraint(theSketch, aFPoint, aFeaturePoint);
  }
}

std::shared_ptr<GeomAPI_Pln> PartSet_Tools::sketchPlane(CompositeFeaturePtr theSketch)
{
  std::shared_ptr<GeomAPI_Pln> aPlane;

  std::shared_ptr<ModelAPI_Data> aData = theSketch->data();
  if (aData) {
    std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    if (aNormal && anOrigin) {
      double adX = aNormal->x();
      double adY = aNormal->y();
      double adZ = aNormal->z();

      if ( (adX != 0) || (adY != 0) || (adZ != 0) ) { // Plane is valid
        double aX = anOrigin->x();
        double aY = anOrigin->y();
        double aZ = anOrigin->z();
        gp_Pln aPln(gp_Pnt(aX, aY, aZ), gp_Dir(adX, adY, adZ));
        double aA, aB, aC, aD;
        aPln.Coefficients(aA, aB, aC, aD);
        aPlane = std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(aA, aB, aC, aD));
      }
    }
  }
  return aPlane;
}

std::shared_ptr<GeomAPI_Pnt> PartSet_Tools::point3D(std::shared_ptr<GeomAPI_Pnt2d> thePoint2D,
                                                      CompositeFeaturePtr theSketch)
{
  std::shared_ptr<GeomAPI_Pnt> aPoint;
  if (!theSketch || !thePoint2D)
    return aPoint;

  DataPtr aData = theSketch->data();
  std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));

  return thePoint2D->to3D(aC->pnt(), aX->dir(), aY);
}

ResultPtr PartSet_Tools::findFixedObjectByExternal(const TopoDS_Shape& theShape,
                                                   const ObjectPtr& theObject,
                                                   CompositeFeaturePtr theSketch)
{
  ResultPtr aResult;
  if (theShape.ShapeType() == TopAbs_EDGE) {
    // Check that we already have such external edge
    std::shared_ptr<GeomAPI_Edge> aInEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge());
    aInEdge->setImpl(new TopoDS_Shape(theShape));
    aResult = findExternalEdge(theSketch, aInEdge);
  }
  if (theShape.ShapeType() == TopAbs_VERTEX) {
    std::shared_ptr<GeomAPI_Vertex> aInVert = std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex());
    aInVert->setImpl(new TopoDS_Shape(theShape));
    aResult = findExternalVertex(theSketch, aInVert);
  }
  return aResult;
}

ResultPtr PartSet_Tools::createFixedObjectByExternal(const TopoDS_Shape& theShape, 
                                                     const ObjectPtr& theObject, 
                                                     CompositeFeaturePtr theSketch,
                                                     const bool theTemporary)
{
  if (theShape.ShapeType() == TopAbs_EDGE) {
    Standard_Real aStart, aEnd;
    Handle(V3d_View) aNullView;
    FeaturePtr aMyFeature;

    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(theShape), aStart, aEnd);
    GeomAdaptor_Curve aAdaptor(aCurve);
    std::shared_ptr<GeomAPI_Edge> anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge);
    anEdge->setImpl(new TopoDS_Shape(theShape));
    if (aAdaptor.GetType() == GeomAbs_Line) {
      // Create line
      aMyFeature = theSketch->addFeature(SketchPlugin_Line::ID());
      if (!theObject.get()) {
        // There is no selected result
        std::shared_ptr<GeomAPI_Pnt> aPnt1 = anEdge->firstPoint();
        std::shared_ptr<GeomAPI_Pnt> aPnt2 = anEdge->lastPoint();
        std::shared_ptr<GeomAPI_Pnt2d> aPnt2d1 = convertTo2D(theSketch, aPnt1);
        std::shared_ptr<GeomAPI_Pnt2d> aPnt2d2 = convertTo2D(theSketch, aPnt2);

        std::shared_ptr<ModelAPI_Data> aData = aMyFeature->data();
        std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::START_ID()));
        std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::END_ID()));

        aPoint1->setValue(aPnt2d1);
        aPoint2->setValue(aPnt2d2);

        // If this is an axis then its name has to be changed correspondently
        std::string aSuffix = "";
        bool aXdir = fabs(aPnt1->x() - aPnt2->x()) > Precision::Confusion();
        bool aYdir = fabs(aPnt1->y() - aPnt2->y()) > Precision::Confusion();
        bool aZdir = fabs(aPnt1->z() - aPnt2->z()) > Precision::Confusion();
        if (aXdir && (!aYdir) && (!aZdir))
          aSuffix = "X";
        else if ((!aXdir) && aYdir && (!aZdir))
          aSuffix = "Y";
        else if ((!aXdir) && (!aYdir) && aZdir)
          aSuffix = "Z";
        if (aSuffix.length() > 0)
          aData->setName("Axis_" + aSuffix);
        aMyFeature->execute();

      }
    } else if (aAdaptor.GetType() == GeomAbs_Circle) {
      if (anEdge->isArc()) {
        // Create arc
        aMyFeature = theSketch->addFeature(SketchPlugin_Arc::ID());
      }
      else {
        // Create circle
        aMyFeature = theSketch->addFeature(SketchPlugin_Circle::ID());
      }
    }
    if (aMyFeature) {
      DataPtr aData = aMyFeature->data();
      AttributeSelectionPtr anAttr = 
        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
        (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));

      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (!aRes.get()) {
        aRes = aMyFeature->firstResult();
      }
      if (anAttr.get() && aRes.get()) {
        std::shared_ptr<GeomAPI_Shape> anEdge(new GeomAPI_Shape);
        anEdge->setImpl(new TopoDS_Shape(theShape));

        anAttr->setValue(aRes, anEdge);
        //if (!theTemporary) {
          aMyFeature->execute();

          // fix this edge
          FeaturePtr aFix = theSketch->addFeature(SketchPlugin_ConstraintRigid::ID());
          aFix->data()->refattr(SketchPlugin_Constraint::ENTITY_A())->
            setObject(aMyFeature->lastResult());
        //}
        return aMyFeature->lastResult();
      }
    }
  }
  if (theShape.ShapeType() == TopAbs_VERTEX) {
    FeaturePtr aMyFeature = theSketch->addFeature(SketchPlugin_Point::ID());

    if (aMyFeature) {
      DataPtr aData = aMyFeature->data();
      AttributeSelectionPtr anAttr = 
        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
        (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));

      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (!aRes.get()) {
        // If the point is selected not from Result object
        std::shared_ptr<GeomAPI_Shape> aShape = 
          std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
        aShape->setImpl(new TopoDS_Shape(theShape));

        std::shared_ptr<GeomAPI_Vertex> aVertex = 
          std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(aShape));
        std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();

        std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = convertTo2D(theSketch, aPnt);
        std::shared_ptr<GeomDataAPI_Point2D> aPoint = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Point::COORD_ID()));
        aPoint->setValue(aPnt2d);
        if ((aPnt->x() < Precision::Confusion()) && 
            (aPnt->y() < Precision::Confusion()) &&
            (aPnt->z() < Precision::Confusion()))
          aData->setName("Origin");

        aMyFeature->execute();
        aRes = aMyFeature->firstResult();
      }
      if (anAttr.get() && aRes.get()) {
        std::shared_ptr<GeomAPI_Shape> aVert(new GeomAPI_Shape);
        aVert->setImpl(new TopoDS_Shape(theShape));

        anAttr->setValue(aRes, aVert);
        //if (theTemporary) {
          aMyFeature->execute();

          // fix this edge
          FeaturePtr aFix = theSketch->addFeature(SketchPlugin_ConstraintRigid::ID());
          aFix->data()->refattr(SketchPlugin_Constraint::ENTITY_A())->
            setObject(aMyFeature->lastResult());
        //}
        return aMyFeature->lastResult();
      }
    }
  }
  return ResultPtr();
}

bool PartSet_Tools::isContainPresentation(const QList<ModuleBase_ViewerPrs>& theSelected,
                                          const ModuleBase_ViewerPrs& thePrs)
{
  foreach (ModuleBase_ViewerPrs aPrs, theSelected) {
    if (aPrs.object() == thePrs.object())
      return true;
  }
  return false;
}

ResultPtr PartSet_Tools::findExternalEdge(CompositeFeaturePtr theSketch, std::shared_ptr<GeomAPI_Edge> theEdge)
{
  for (int i = 0; i < theSketch->numberOfSubs(); i++) {
    FeaturePtr aFeature = theSketch->subFeature(i);
    std::shared_ptr<SketchPlugin_Feature> aSketchFea = 
      std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if (aSketchFea) {
      if (aSketchFea->isExternal()) {
        std::list<ResultPtr> aResults = aSketchFea->results();
        std::list<ResultPtr>::const_iterator aIt;
        for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
          ResultConstructionPtr aRes = 
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aIt);
          if (aRes) {
            std::shared_ptr<GeomAPI_Shape> aShape = aRes->shape();
            if (aShape) {
              if (theEdge->isEqual(aShape))
                return aRes;
            }
          }
        }
      }
    }
  }
  return ResultPtr();
}


ResultPtr PartSet_Tools::findExternalVertex(CompositeFeaturePtr theSketch, std::shared_ptr<GeomAPI_Vertex> theVert)
{
  for (int i = 0; i < theSketch->numberOfSubs(); i++) {
    FeaturePtr aFeature = theSketch->subFeature(i);
    std::shared_ptr<SketchPlugin_Feature> aSketchFea = 
      std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if (aSketchFea) {
      if (aSketchFea->isExternal()) {
        std::list<ResultPtr> aResults = aSketchFea->results();
        std::list<ResultPtr>::const_iterator aIt;
        for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
          ResultConstructionPtr aRes = 
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aIt);
          if (aRes) {
            std::shared_ptr<GeomAPI_Shape> aShape = aRes->shape();
            if (aShape) {
              if (theVert->isEqual(aShape))
                return aRes;
            }
          }
        }
      }
    }
  }
  return ResultPtr();
}


bool PartSet_Tools::hasVertexShape(const ModuleBase_ViewerPrs& thePrs, FeaturePtr theSketch,
                                   Handle_V3d_View theView, double& theX, double& theY)
{
  bool aHasVertex = false;

  const TopoDS_Shape& aShape = thePrs.shape();
  if (!aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX)
  {
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
    if (!aVertex.IsNull())
    {
      gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
      PartSet_Tools::convertTo2D(aPoint, theSketch, theView, theX, theY);
      aHasVertex = true;
    }
  }

  return aHasVertex;
}

GeomShapePtr PartSet_Tools::findShapeBy2DPoint(const AttributePtr& theAttribute,
                                               ModuleBase_IWorkshop* theWorkshop)
{
  // 1. find an attribute value in attribute reference attribute value
  GeomShapePtr aShape;
  AttributeRefAttrPtr aRefAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (aRefAttr) {
    if (!aRefAttr->isObject()) {
      AttributePtr theAttribute = aRefAttr->attr();
      if (theAttribute.get()) {
        XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWorkshop);
        XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

        // 2. find visualized vertices of the attribute and if the attribute of the vertex is
        // the same, return it
        FeaturePtr anAttributeFeature = ModelAPI_Feature::feature(theAttribute->owner());
        // 2.1 get visualized results of the feature
        const std::list<ResultPtr>& aResList = anAttributeFeature->results();
        std::list<ResultPtr>::const_iterator anIt = aResList.begin(), aLast = aResList.end();
        for (; anIt != aLast; anIt++) {
          AISObjectPtr aAISObj = aDisplayer->getAISObject(*anIt);
          if (aAISObj.get() != NULL) {
            Handle(AIS_InteractiveObject) anAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
            // 2.2 find selected owners of a visualizedd object
            SelectMgr_IndexedMapOfOwner aSelectedOwners;
            aConnector->workshop()->selector()->selection()->entityOwners(anAISIO, aSelectedOwners);
            for (Standard_Integer i = 1, n = aSelectedOwners.Extent(); i <= n; i++) {
              Handle(SelectMgr_EntityOwner) anOwner = aSelectedOwners(i);
              if (!anOwner.IsNull()) {
                Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast(anOwner);
                if (!aBRepOwner.IsNull() && aBRepOwner->HasShape()) {
                  const TopoDS_Shape& aBRepShape = aBRepOwner->Shape();
                  if (aBRepShape.ShapeType() == TopAbs_VERTEX) {
                    // 2.3 if the owner is vertex and an attribute of the vertex is equal to the initial
                    // attribute, returns the shape
                    PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(theWorkshop->module());
                    PartSet_SketcherMgr* aSketchMgr = aModule->sketchMgr();
                    AttributePtr aPntAttr = PartSet_Tools::findAttributeBy2dPoint(anAttributeFeature,
                                                             aBRepShape, aSketchMgr->activeSketch());
                    if (aPntAttr.get() != NULL && aPntAttr == theAttribute) {
                      aShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape);
                      aShape->setImpl(new TopoDS_Shape(aBRepShape));
                      break;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return aShape;
}

std::shared_ptr<GeomAPI_Pnt2d> PartSet_Tools::getPoint(std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                       const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theFeature->data())
    return std::shared_ptr<GeomAPI_Pnt2d>();

  FeaturePtr aFeature;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theFeature->data()->attribute(theAttribute));
  if (!anAttr)
    return std::shared_ptr<GeomAPI_Pnt2d>();

  aFeature = ModelAPI_Feature::feature(anAttr->object());

  if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));

  else if (anAttr->attr()) {
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
  }
  if (aPointAttr.get() != NULL)
    return aPointAttr->pnt();
  return std::shared_ptr<GeomAPI_Pnt2d>();
}

FeaturePtr findFirstCoincidenceByData(const DataPtr& theData, std::shared_ptr<GeomAPI_Pnt2d> thePoint)
{
  FeaturePtr aCoincident;

  const std::set<AttributePtr>& aRefsList = theData->refsToMe();
  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
    FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) { 
      std::shared_ptr<GeomAPI_Pnt2d> a2dPnt = 
        PartSet_Tools::getPoint(aConstrFeature, SketchPlugin_ConstraintCoincidence::ENTITY_A());
      if (a2dPnt.get() && thePoint->isEqual(a2dPnt)) { 
        aCoincident = aConstrFeature;
        break;
      } else {
        a2dPnt = PartSet_Tools::getPoint(aConstrFeature,
                                          SketchPlugin_ConstraintCoincidence::ENTITY_B());
        if (a2dPnt.get() && thePoint->isEqual(a2dPnt)) { 
          aCoincident = aConstrFeature;
          break;
        }
      }
    }
  }
  return aCoincident;
}

FeaturePtr PartSet_Tools::findFirstCoincidence(const FeaturePtr& theFeature,
                                               std::shared_ptr<GeomAPI_Pnt2d> thePoint,
                                               const bool theSearchInResults)
{
  FeaturePtr aCoincident;
  if (theFeature.get() == NULL)
    return aCoincident;

  const std::set<AttributePtr>& aRefsList = theFeature->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
    FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) { 
      std::shared_ptr<GeomAPI_Pnt2d> a2dPnt = 
        PartSet_Tools::getPoint(aConstrFeature, SketchPlugin_ConstraintCoincidence::ENTITY_A());
      if (a2dPnt.get() && thePoint->isEqual(a2dPnt)) { 
        aCoincident = aConstrFeature;
        break;
      } else {
        a2dPnt = PartSet_Tools::getPoint(aConstrFeature,
                                          SketchPlugin_ConstraintCoincidence::ENTITY_B());
        if (a2dPnt.get() && thePoint->isEqual(a2dPnt)) { 
          aCoincident = aConstrFeature;
          break;
        }
      }
    }
  }
  if (theSearchInResults) {
    if (!aCoincident.get()) {
      std::list<ResultPtr> aResults = theFeature->results();
      std::list<ResultPtr>::const_iterator aIt;
      for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
        ResultPtr aResult = *aIt;
        aCoincident = findFirstCoincidenceByData(aResult->data(), thePoint);
        if (aCoincident.get())
          break;
      }
    }
  }
  return aCoincident;
}

void PartSet_Tools::findCoincidences(FeaturePtr theStartCoin, QList<FeaturePtr>& theList,
                                     std::string theAttr)
{
  AttributeRefAttrPtr aPnt = theStartCoin->refattr(theAttr);
  if (!aPnt) return;
  FeaturePtr aObj = ModelAPI_Feature::feature(aPnt->object());
  if (!theList.contains(aObj)) {
    std::shared_ptr<GeomAPI_Pnt2d> aOrig = getCoincedencePoint(theStartCoin);
    if (aOrig.get() == NULL)
      return;
    theList.append(aObj);
    const std::set<AttributePtr>& aRefsList = aObj->data()->refsToMe();
    std::set<AttributePtr>::const_iterator aIt;
    for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
      FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) { 
        std::shared_ptr<GeomAPI_Pnt2d> aPnt = getCoincedencePoint(aConstrFeature);
        if (aPnt.get() && aOrig->isEqual(aPnt)) {
          findCoincidences(aConstrFeature, theList, SketchPlugin_ConstraintCoincidence::ENTITY_A());
          findCoincidences(aConstrFeature, theList, SketchPlugin_ConstraintCoincidence::ENTITY_B());
        }
      }
    }
  }
}

std::shared_ptr<GeomAPI_Pnt2d> PartSet_Tools::getCoincedencePoint(FeaturePtr theStartCoin)
{
  std::shared_ptr<GeomAPI_Pnt2d> aPnt = SketcherPrs_Tools::getPoint(theStartCoin.get(), 
                                                                    SketchPlugin_Constraint::ENTITY_A());
  if (aPnt.get() == NULL)
    aPnt = SketcherPrs_Tools::getPoint(theStartCoin.get(), SketchPlugin_Constraint::ENTITY_B());
  return aPnt;
}

AttributePtr PartSet_Tools::findAttributeBy2dPoint(ObjectPtr theObj, 
                                                   const TopoDS_Shape theShape, 
                                                   FeaturePtr theSketch)
{

  AttributePtr anAttribute;
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObj);
  if (aFeature) {
    if (theShape.ShapeType() == TopAbs_VERTEX) {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(theShape);
      if (!aVertex.IsNull())  {
        gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
        std::shared_ptr<GeomAPI_Pnt> aValue = std::shared_ptr<GeomAPI_Pnt>(
            new GeomAPI_Pnt(aPoint.X(), aPoint.Y(), aPoint.Z()));

        // find the given point in the feature attributes
        std::list<AttributePtr> anAttiributes = 
          aFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
        std::list<AttributePtr>::const_iterator anIt = anAttiributes.begin(), 
                                                aLast = anAttiributes.end();
        for (; anIt != aLast && !anAttribute; anIt++) {
          std::shared_ptr<GeomDataAPI_Point2D> aCurPoint = 
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);

          std::shared_ptr<GeomAPI_Pnt> aPnt = convertTo3D(aCurPoint->x(), aCurPoint->y(), theSketch);
          if (aPnt && (aPnt->distance(aValue) < Precision::Confusion())) {
            anAttribute = aCurPoint;
            break;
          }
        }
      }
    }
  }
  return anAttribute;
}
