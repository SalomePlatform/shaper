// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <PartSet_Tools.h>
#include <PartSet_Module.h>
#include <PartSet_SketcherMgr.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <ModuleBase_IViewWindow.h>

#include <ModelGeomAlgo_Point2D.h>

#include <Events_Loop.h>
#include <Events_InfoMessage.h>

#include <SketcherPrs_Tools.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_ShapeExplorer.h>

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
#include <SketchPlugin_Projection.h>
#include <SketchPlugin_IntersectionPoint.h>

#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>

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
#include <V3d_Coordinate.hxx>

#include <QMouseEvent>

#ifdef _DEBUG
#include <QDebug>
#endif

const double PRECISION_TOLERANCE = 0.000001;
const int AIS_DEFAULT_WIDTH = 2;
const bool SKETCH_PROJECTION_INCLUDE_INTO_RESULT = false; // by default, it is not presented

int PartSet_Tools::getAISDefaultWidth()
{
  return AIS_DEFAULT_WIDTH;
}

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


std::shared_ptr<GeomAPI_Pnt> PartSet_Tools::convertTo3D(const double theX, const double theY,
                                                        FeaturePtr theSketch)
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

  // we need to flush created signal in order to coincidence is processed by solver
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
}

std::shared_ptr<GeomAPI_Pln> PartSet_Tools::sketchPlane(CompositeFeaturePtr theSketch)
{
  std::shared_ptr<GeomAPI_Pln> aPlane;

  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      theSketch->data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      theSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  if (aNormal.get() && aNormal->isInitialized() &&
      anOrigin.get() && anOrigin->isInitialized())
    aPlane = std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(anOrigin->pnt(), aNormal->dir()));

  return aPlane;
}

void PartSet_Tools::nullifySketchPlane(CompositeFeaturePtr theSketch)
{
  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
    theSketch->data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
    theSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  aNormal->reset();
  anOrigin->reset();
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
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (!aResult.get())
    return ResultPtr();

  for (int i = 0, aNbSubs = theSketch->numberOfSubs(); i < aNbSubs; i++) {
    FeaturePtr aFeature = theSketch->subFeature(i);
    if (aFeature->getKind() != SketchPlugin_Projection::PROJECTED_FEATURE_ID())
      continue;
    if (aFeature->lastResult() == aResult)
      return aResult;
  }
  return ResultPtr();
}

ResultPtr PartSet_Tools::createFixedObjectByExternal(
                                   const std::shared_ptr<GeomAPI_Shape>& theShape,
                                   const ObjectPtr& theObject,
                                   CompositeFeaturePtr theSketch,
                                   const bool theTemporary,
                                   FeaturePtr& theCreatedFeature)
{
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (!aResult.get())
    return ResultPtr();

  FeaturePtr aProjectionFeature = theSketch->addFeature(SketchPlugin_Projection::ID());
  theCreatedFeature = aProjectionFeature;
  AttributeSelectionPtr anExternalAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(
                 aProjectionFeature->attribute(SketchPlugin_Projection::EXTERNAL_FEATURE_ID()));
  anExternalAttr->setValue(aResult, theShape);

  AttributeBooleanPtr anIntoResult = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>
    (aProjectionFeature->data()->attribute(SketchPlugin_Projection::INCLUDE_INTO_RESULT()));
  anIntoResult->setValue(SKETCH_PROJECTION_INCLUDE_INTO_RESULT);
  aProjectionFeature->execute();

  // if projection feature has not been created, exit
  AttributeRefAttrPtr aRefAttr = aProjectionFeature->data()->refattr(
    SketchPlugin_Projection::PROJECTED_FEATURE_ID());
  if (!aRefAttr || !aRefAttr->isInitialized())
  {
    // remove external feature if the attribute is not filled
    std::set<FeaturePtr> aFeatures;
    aFeatures.insert(aProjectionFeature);
    ModelAPI_Tools::removeFeaturesAndReferences(aFeatures);
    return ResultPtr();
  }

  FeaturePtr aProjection = ModelAPI_Feature::feature(aRefAttr->object());
  if (aProjection.get() && aProjection->lastResult().get())
    return aProjection->lastResult();

  return ResultPtr();
}

bool PartSet_Tools::isContainPresentation(const QList<ModuleBase_ViewerPrsPtr>& theSelected,
                                          const ModuleBase_ViewerPrsPtr& thePrs)
{
  foreach (ModuleBase_ViewerPrsPtr aPrs, theSelected) {
    if (aPrs->object() == thePrs->object())
      return true;
  }
  return false;
}

GeomShapePtr PartSet_Tools::findShapeBy2DPoint(const AttributePtr& theAttribute,
                                                       ModuleBase_IWorkshop* theWorkshop)
{
  GeomShapePtr aShape;
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
              std::pair<AttributePtr, int> aPntAttrIndex = PartSet_Tools::findAttributeBy2dPoint(
                  anAttributeFeature, aBRepShape, aSketchMgr->activeSketch());
              if (aPntAttrIndex.first.get() != NULL && aPntAttrIndex.first == theAttribute) {
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
  return aShape;
}

std::shared_ptr<GeomAPI_Pnt2d> PartSet_Tools::getPoint(
                                                  std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                  const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr = ModelGeomAlgo_Point2D::getPointOfRefAttr(
                                          theFeature.get(), theAttribute, SketchPlugin_Point::ID(),
                                          SketchPlugin_Point::COORD_ID());
  if (aPointAttr.get() != NULL)
    return aPointAttr->pnt();
  return std::shared_ptr<GeomAPI_Pnt2d>();
}

std::shared_ptr<GeomAPI_Pnt2d> PartSet_Tools::getPnt2d(QMouseEvent* theEvent,
                                                ModuleBase_IViewWindow* theWindow,
                                                const FeaturePtr& theSketch)
{
  gp_Pnt aPnt = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());
  double aX, anY;
  Handle(V3d_View) aView = theWindow->v3dView();
  PartSet_Tools::convertTo2D(aPnt, theSketch, aView, aX, anY);

  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, anY));
}

std::shared_ptr<GeomAPI_Pnt2d> PartSet_Tools::getPnt2d(const Handle(V3d_View)& theView,
                                                       const TopoDS_Shape& theShape,
                                                       const FeaturePtr& theSketch)
{
  GeomPnt2dPtr aPoint2D;
  if (!theShape.IsNull() && theShape.ShapeType() == TopAbs_VERTEX) {
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(theShape);
    if (!aVertex.IsNull()) {
      // the case when the point is taken from the existing vertex
      gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
      double aX, aY;
      PartSet_Tools::convertTo2D(aPoint, theSketch, theView, aX, aY);
      aPoint2D.reset(new GeomAPI_Pnt2d(aX, aY));
    }
  }
  return aPoint2D;
}

FeaturePtr findFirstCoincidenceByData(const DataPtr& theData,
                                      std::shared_ptr<GeomAPI_Pnt2d> thePoint)
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
                                               std::shared_ptr<GeomAPI_Pnt2d> thePoint)
{
  FeaturePtr aCoincident;
  if (theFeature.get() == NULL)
    return aCoincident;

  const std::set<AttributePtr>& aRefsList = theFeature->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
    FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aConstrFeature && aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
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
  /// Find by result
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
  return aCoincident;
}

void PartSet_Tools::findCoincidences(FeaturePtr theStartCoin, QList<FeaturePtr>& theList,
                                     QList<FeaturePtr>& theCoincidencies,
                                     std::string theAttr, QList<bool>& theIsAttributes)
{
  std::shared_ptr<GeomAPI_Pnt2d> aOrig = getCoincedencePoint(theStartCoin);
  if (aOrig.get() == NULL)
    return;

  AttributeRefAttrPtr aPnt = theStartCoin->refattr(theAttr);
  if (!aPnt)
    return;
  ObjectPtr aObj = aPnt->object();
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
  if (aFeature.get()) {
    if (!theList.contains(aFeature)) {
      theList.append(aFeature);
      theCoincidencies.append(theStartCoin);
      theIsAttributes.append(true); // point attribute on a feature
      const std::set<AttributePtr>& aRefsList = aFeature->data()->refsToMe();
      std::set<AttributePtr>::const_iterator aIt;
      for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
        std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
        FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
        if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
          if (!theCoincidencies.contains(aConstrFeature)) {
            std::shared_ptr<GeomAPI_Pnt2d> aPnt = getCoincedencePoint(aConstrFeature);
            if (aPnt.get() && aOrig->isEqual(aPnt)) {
              findCoincidences(aConstrFeature, theList, theCoincidencies,
                SketchPlugin_ConstraintCoincidence::ENTITY_A(), theIsAttributes);
              findCoincidences(aConstrFeature, theList, theCoincidencies,
                SketchPlugin_ConstraintCoincidence::ENTITY_B(), theIsAttributes);
            }
          }
        }
      }
    }
  } else {
    // Find by Results
    ResultConstructionPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObj);
    if (aResult.get()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aPnt->object());
      if (!theList.contains(aFeature))
        theList.append(aFeature);
      theCoincidencies.append(theStartCoin);
      theIsAttributes.append(false); // point attribute on a feature

      const std::set<AttributePtr>& aRefsList = aResult->data()->refsToMe();
      std::set<AttributePtr>::const_iterator aIt;
      for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
        std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
        FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
        if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
          if (!theCoincidencies.contains(aConstrFeature)) {
            std::shared_ptr<GeomAPI_Pnt2d> aPnt = getCoincedencePoint(aConstrFeature);
            if (aPnt.get() && aOrig->isEqual(aPnt)) {
              findCoincidences(aConstrFeature, theList, theCoincidencies,
                SketchPlugin_ConstraintCoincidence::ENTITY_A(), theIsAttributes);
              findCoincidences(aConstrFeature, theList, theCoincidencies,
                SketchPlugin_ConstraintCoincidence::ENTITY_B(), theIsAttributes);
            }
          }
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

class PointWrapper
{
public:
  PointWrapper(AttributePtr theAttribute)
    : myPoint(std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute)),
      myArray(std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(theAttribute))
  {}

  int size() const { return myPoint.get() ? 1 : (myArray.get() ? myArray->size() : 0); }

  GeomPointPtr point(int theIndex, FeaturePtr theSketch)
  {
    GeomPnt2dPtr aP2d;
    if (myPoint.get())
      aP2d = myPoint->pnt();
    else if (myArray.get())
      aP2d = myArray->pnt(theIndex);

    GeomPointPtr aP3d;
    if (aP2d.get())
      aP3d = PartSet_Tools::convertTo3D(aP2d->x(), aP2d->y(), theSketch);
    return aP3d;
  }

  bool isArray() const { return myArray.get(); }

private:
  AttributePoint2DPtr myPoint;
  AttributePoint2DArrayPtr myArray;
};

std::pair<AttributePtr, int> PartSet_Tools::findAttributeBy2dPoint(ObjectPtr theObj,
                                                                   const TopoDS_Shape theShape,
                                                                   FeaturePtr theSketch)
{

  AttributePtr anAttribute;
  int aPointIndex = -1;
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
        std::list<AttributePtr> anArrays =
          aFeature->data()->attributes(GeomDataAPI_Point2DArray::typeId());
        anAttiributes.insert(anAttiributes.end(), anArrays.begin(), anArrays.end());

        std::list<AttributePtr>::const_iterator anIt = anAttiributes.begin(),
                                                aLast = anAttiributes.end();
        double aMinDistance = 1.e-6; // searching for point with minimal distance and < 1.e-6
        for (; anIt != aLast && !anAttribute; anIt++) {
          PointWrapper aWrapper(*anIt);
          for (int anIndex = 0, aSize = aWrapper.size(); anIndex < aSize; ++anIndex) {
            std::shared_ptr<GeomAPI_Pnt> aPnt = aWrapper.point(anIndex, theSketch);
            if (aPnt) {
              double aDistance = aPnt->distance(aValue);
              if (aDistance < aMinDistance) {
                anAttribute = *anIt;
                if (aWrapper.isArray())
                  aPointIndex = anIndex;
                aMinDistance = aPnt->distance(aValue);
              }
            }
          }
        }
      }
    }
  }
  return std::pair<AttributePtr, int>(anAttribute, aPointIndex);
}

void PartSet_Tools::sendSubFeaturesEvent(const CompositeFeaturePtr& theComposite,
                                         const Events_ID theEventId)
{
  if (!theComposite.get())
    return;

  static Events_Loop* aLoop = Events_Loop::loop();
  int aNumberOfSubs = theComposite->numberOfSubs();
  for (int i = 0; i < aNumberOfSubs; i++) {
    FeaturePtr aSubFeature = theComposite->subFeature(i);
    static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
    aECreator->sendUpdated(aSubFeature, theEventId);
  }
  Events_Loop::loop()->flush(theEventId);
}

bool PartSet_Tools::isAuxiliarySketchEntity(const ObjectPtr& theObject)
{
  bool isAuxiliaryFeature = false;

  FeaturePtr anObjectFeature = ModelAPI_Feature::feature(theObject);
  std::string anAuxiliaryAttribute = SketchPlugin_SketchEntity::AUXILIARY_ID();
  AttributeBooleanPtr anAuxiliaryAttr = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
                                    anObjectFeature->data()->attribute(anAuxiliaryAttribute));
  if (anAuxiliaryAttr.get())
    isAuxiliaryFeature = anAuxiliaryAttr->value();


  return isAuxiliaryFeature;
}

bool PartSet_Tools::isIncludeIntoSketchResult(const ObjectPtr& theObject)
{
  // check the feature is neither Projection nor IntersectionPoint feature
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (aFeature->getKind() == SketchPlugin_Projection::ID() ||
      aFeature->getKind() == SketchPlugin_IntersectionPoint::ID())
    return false;

  // go through the references to the feature to check
  // if it was created by Projection or Intersection
  const std::set<AttributePtr>& aRefs = theObject->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
       aRefIt != aRefs.end(); ++aRefIt) {
    AttributePtr anAttr = *aRefIt;
    std::string anIncludeToResultAttrName;
    if (anAttr->id() == SketchPlugin_Projection::PROJECTED_FEATURE_ID())
      anIncludeToResultAttrName = SketchPlugin_Projection::INCLUDE_INTO_RESULT();
    else if (anAttr->id() == SketchPlugin_IntersectionPoint::INTERSECTION_POINTS_ID())
      anIncludeToResultAttrName = SketchPlugin_IntersectionPoint::INCLUDE_INTO_RESULT();

    if (!anIncludeToResultAttrName.empty()) {
      // check "include into result" flag
      FeaturePtr aParent = ModelAPI_Feature::feature(anAttr->owner());
      return aParent->boolean(anIncludeToResultAttrName)->value();
    }
  }
  return true;
}


ResultPtr PartSet_Tools::createFixedByExternalCenter(
    const ObjectPtr& theObject,
    const std::shared_ptr<GeomAPI_Edge>& theEdge,
    ModelAPI_AttributeSelection::CenterType theType,
    const CompositeFeaturePtr& theSketch,
    bool theTemporary,
    FeaturePtr& theCreatedFeature)
{
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (!aResult.get())
    return ResultPtr();

  FeaturePtr aProjectionFeature = theSketch->addFeature(SketchPlugin_Projection::ID());
  theCreatedFeature = aProjectionFeature;
  AttributeSelectionPtr anExternalAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(
                 aProjectionFeature->attribute(SketchPlugin_Projection::EXTERNAL_FEATURE_ID()));
  anExternalAttr->setValueCenter(aResult, theEdge, theType, theTemporary);

  AttributeBooleanPtr anIntoResult = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>
    (aProjectionFeature->data()->attribute(SketchPlugin_Projection::INCLUDE_INTO_RESULT()));
  anIntoResult->setValue(SKETCH_PROJECTION_INCLUDE_INTO_RESULT);
  aProjectionFeature->execute();

  // if projection feature has not been created, exit
  AttributeRefAttrPtr aRefAttr = aProjectionFeature->data()->refattr(
    SketchPlugin_Projection::PROJECTED_FEATURE_ID());
  if (!aRefAttr || !aRefAttr->isInitialized())
    return ResultPtr();

  FeaturePtr aProjection = ModelAPI_Feature::feature(aRefAttr->object());
  if (aProjection.get() && aProjection->lastResult().get())
    return aProjection->lastResult();

  return ResultPtr();
}

void PartSet_Tools::getFirstAndLastIndexInFolder(const ObjectPtr& theFolder,
  int& theFirst, int& theLast)
{
  theFirst = -1;
  theLast = -1;

  DocumentPtr aDoc = theFolder->document();
  FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(theFolder);
  if (!aFolder.get())
    return;

  AttributeReferencePtr aFirstFeatAttr =
    aFolder->data()->reference(ModelAPI_Folder::FIRST_FEATURE_ID());
  if (!aFirstFeatAttr.get())
    return;
  FeaturePtr aFirstFeatureInFolder = ModelAPI_Feature::feature(aFirstFeatAttr->value());
  if (!aFirstFeatureInFolder.get())
    return;

  FeaturePtr aLastFeatureInFolder = aFolder->lastVisibleFeature();
  if (!aLastFeatureInFolder.get())
    return;

  theFirst = aDoc->index(aFirstFeatureInFolder);
  theLast = aDoc->index(aLastFeatureInFolder);
}


void PartSet_Tools::getDefaultColor(ObjectPtr theObject, const bool isEmptyColorValid,
  std::vector<int>& theColor)
{
  theColor.clear();
  // get default color from the preferences manager for the given result
  if (theColor.empty()) {
    std::string aSection, aName, aDefault;
    theObject->colorConfigInfo(aSection, aName, aDefault);
    if (!aSection.empty() && !aName.empty()) {
      theColor = Config_PropManager::color(aSection, aName);
    }
  }
  if (!isEmptyColorValid && theColor.empty()) {
    // all AIS objects, where the color is not set, are in black.
    // The color should be defined in XML or set in the attribute
    theColor = Config_PropManager::color("Visualization", "object_default_color");
    Events_InfoMessage("PartSet_Tools",
      "A default color is not defined in the preferences for this result type").send();
  }
}

double PartSet_Tools::getDefaultDeflection(const ObjectPtr& theObject)
{
  double aDeflection = -1;
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (aResult.get()) {
    bool isConstruction = false;

    std::string aResultGroup = aResult->groupName();
    if (aResultGroup == ModelAPI_ResultConstruction::group())
      isConstruction = true;
    else if (aResultGroup == ModelAPI_ResultBody::group()) {
      GeomShapePtr aGeomShape = aResult->shape();
      if (aGeomShape.get()) {
        // if the shape could not be exploded on faces, it contains only wires, edges, and vertices
        // correction of deviation for them should not influence to the application performance
        GeomAPI_ShapeExplorer anExp(aGeomShape, GeomAPI_Shape::FACE);
        isConstruction = !anExp.more();
      }
    }
    if (isConstruction)
      aDeflection = Config_PropManager::real("Visualization", "construction_deflection");
    else
      aDeflection = Config_PropManager::real("Visualization", "body_deflection");
  }
  return aDeflection;
}


double PartSet_Tools::getDefaultTransparency()
{
  return Config_PropManager::integer("Visualization", "shaper_default_transparency") / 100.;
}

QCursor PartSet_Tools::getOperationCursor()
{
  int aId = Config_PropManager::integer(SKETCH_TAB_NAME, "operation_cursor");
  switch (aId) {
  case 0:
    return QCursor(Qt::ArrowCursor);
  case 1:
    return QCursor(Qt::CrossCursor);
  case 2:
    return QCursor(Qt::PointingHandCursor);
  }
  return QCursor();
}
