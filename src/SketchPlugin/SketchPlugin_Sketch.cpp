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

#include <Config_PropManager.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <GeomAPI_Dir.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Vertex.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_Projection.h>
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Tools.h>

#include <Events_InfoMessage.h>
#include <Events_Loop.h>

#include <memory>

#include <math.h>
#include <vector>

SketchPlugin_Sketch::SketchPlugin_Sketch()
{
}

void SketchPlugin_Sketch::initAttributes()
{
  data()->addAttribute(SketchPlugin_Sketch::ORIGIN_ID(), GeomDataAPI_Point::typeId());
  data()->addAttribute(SketchPlugin_Sketch::DIRX_ID(), GeomDataAPI_Dir::typeId());
  data()->addAttribute(SketchPlugin_Sketch::NORM_ID(), GeomDataAPI_Dir::typeId());
  data()->addAttribute(SketchPlugin_Sketch::FEATURES_ID(), ModelAPI_AttributeRefList::typeId());
  // the selected face, base for the sketcher plane, not obligatory
  data()->addAttribute(SketchPlugin_SketchEntity::EXTERNAL_ID(),
    ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), SketchPlugin_SketchEntity::EXTERNAL_ID());
  data()->addAttribute(SketchPlugin_Sketch::SOLVER_ERROR(), ModelAPI_AttributeString::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), SketchPlugin_Sketch::SOLVER_ERROR());
  data()->addAttribute(SketchPlugin_Sketch::SOLVER_DOF(), ModelAPI_AttributeString::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), SketchPlugin_Sketch::SOLVER_DOF());
}

void SketchPlugin_Sketch::execute()
{
  if (!data()->isValid())
    return;
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));

  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  std::list<ObjectPtr> aFeatures = aRefList->list();
  if (aFeatures.empty()) // actually, this must be avoided by the validators
    return;

  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  std::shared_ptr<SketchPlugin_Feature> aFeature;
  std::list<std::shared_ptr<GeomAPI_Shape> > aFeaturesPreview;
  for (; anIt != aLast; anIt++) {
    aFeature = std::dynamic_pointer_cast<SketchPlugin_Feature>(*anIt);
    if (aFeature) {
      if (!aFeature->sketch()) // on load document the back references are missed
        aFeature->setSketch(this);
      // do not include into the result the external edges with disabled flag "Include into result"
      if (aFeature->data()->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID())) {
        if (aFeature->data()->selection(SketchPlugin_SketchEntity::EXTERNAL_ID())->context()) {
          const std::string& anAttrName =
              aFeature->getKind() == SketchPlugin_Projection::ID() ?
              SketchPlugin_Projection::INCLUDE_INTO_RESULT() :
              SketchPlugin_IntersectionPoint::INCLUDE_INTO_RESULT();
          AttributeBooleanPtr aKeepResult = aFeature->boolean(anAttrName);
          if (!aKeepResult || !aKeepResult->value())
            continue;
        }
      }
      // do not include the construction entities in the result
      if (aFeature->data()->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID())) {
        if (aFeature->data()->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value())
          continue;
      }

      const std::list<std::shared_ptr<ModelAPI_Result> >& aRes = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter = aRes.cbegin();
      for (; aResIter != aRes.cend(); aResIter++) {
        std::shared_ptr<ModelAPI_ResultConstruction> aConstr = std::dynamic_pointer_cast<
            ModelAPI_ResultConstruction>(*aResIter);
        if (aConstr) {
          std::shared_ptr<GeomAPI_Shape> aShape = aConstr->shape();
          if (aShape)
            aFeaturesPreview.push_back(aShape);
        }
      }
    }
  }

  // Collect all edges as one big wire
  std::shared_ptr<GeomAPI_PlanarEdges> aBigWire(new GeomAPI_PlanarEdges);
  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator aShapeIt = aFeaturesPreview.begin();
  for (; aShapeIt != aFeaturesPreview.end(); ++aShapeIt) {
    aBigWire->addEdge(*aShapeIt);
  }
  aBigWire->setPlane(anOrigin->pnt(), aDirX->dir(), aNorm->dir());
  std::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
  aConstr->setShape(aBigWire);
  setResult(aConstr);
}

std::shared_ptr<ModelAPI_Feature> SketchPlugin_Sketch::addFeature(std::string theID)
{
  // Set last feature of the sketch as current feature.
  // Reason: Changing of parameter through Python API may lead to creation of new features
  //         (e.g. changing number of copies in MultiRotation). If the sketch is not the last
  //         feature in the Object Browser, then new features will be added to the end feature.
  //         Therefore, setting any feature below the sketch as a current feature will disable
  //         these newly created features.
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));
  int aSize = aRefList->size(false);
  ObjectPtr aLastObject = aSize == 0 ? data()->owner() : aRefList->object(aSize - 1, false);
  FeaturePtr aLastFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aLastObject);
  document()->setCurrentFeature(aLastFeature, false);

  // add new feature
  std::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID, false);
  if (aNew) {
    // the sketch cannot be specified for the macro-features defined in python
    // like SketchRectangle, so we need to check the type of new feature
    std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(aNew);
    if (aSketchFeature)
      aSketchFeature->setSketch(this);
    data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->append(aNew);
  }
   // set as current also after it becomes sub to set correctly enabled for other sketch subs
  document()->setCurrentFeature(aNew, false);

  return aNew;
}

void SketchPlugin_Sketch::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (!data()->isValid()) // sketch is already removed (case on undo of sketch), sync is not needed
    return;
  AttributeRefListPtr aList = reflist(SketchPlugin_Sketch::FEATURES_ID());
  // if the object is last, remove it from the list
  // (needed to skip empty transaction on edit of sketch feature)
  if (aList->object(aList->size(true) - 1, true) == theFeature) {
    aList->remove(theFeature);
  } else {
    // to keep the persistent sub-elements indexing, do not remove elements from list,
    // but substitute by nulls
    aList->substitute(theFeature, ObjectPtr());
  }
}

int SketchPlugin_Sketch::numberOfSubs(bool forTree) const
{
  if (forTree)
    return 0;
  return data()->reflist(FEATURES_ID())->size(false);
}

std::shared_ptr<ModelAPI_Feature> SketchPlugin_Sketch::subFeature(
  const int theIndex, bool forTree)
{
  if (forTree)
    return FeaturePtr();

  ObjectPtr anObj = data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->object(theIndex, false);
  FeaturePtr aRes = std::dynamic_pointer_cast<ModelAPI_Feature>(anObj);
  return aRes;
}

int SketchPlugin_Sketch::subFeatureId(const int theIndex) const
{
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));
  std::list<ObjectPtr> aFeatures = aRefList->list();
  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin();
  int aResultIndex = 1; // number of the counted (created) features, started from 1
  int aFeatureIndex = -1; // number of the not-empty features in the list
  for (; anIt != aFeatures.end(); anIt++) {
    if (anIt->get())
      aFeatureIndex++;
    if (aFeatureIndex == theIndex)
      break;
    aResultIndex++;
  }
  return aResultIndex;
}

bool SketchPlugin_Sketch::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature) {
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aRes)
      aFeature = document()->feature(aRes);
  }
  if (aFeature) {
    return data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->isInList(aFeature);
  }
  return false;
}


static bool isOrigin(const GeomPointPtr& thePoint, const double theTolerance)
{
  return fabs(thePoint->x()) < theTolerance &&
         fabs(thePoint->y()) < theTolerance &&
         fabs(thePoint->z()) < theTolerance;
}

static bool isCoordinateAxis(const GeomDirPtr& theDir, const double theTolerance)
{
  return fabs(theDir->x() - 1.0) < theTolerance || fabs(theDir->x() + 1.0) < theTolerance ||
         fabs(theDir->y() - 1.0) < theTolerance || fabs(theDir->y() + 1.0) < theTolerance ||
         fabs(theDir->z() - 1.0) < theTolerance || fabs(theDir->z() + 1.0) < theTolerance;
}

static bool isCoordinatePlane(const GeomAx3Ptr& thePlane)
{
  static const double THE_TOLERANCE = 1.e-7;
  if (!thePlane)
    return false;

  GeomPointPtr anOrigin = thePlane->origin();
  GeomDirPtr aNormal = thePlane->normal();
  GeomDirPtr aDirX = thePlane->dirX();

  return isOrigin(anOrigin, THE_TOLERANCE) &&
         isCoordinateAxis(aNormal, THE_TOLERANCE) &&
         isCoordinateAxis(aDirX, THE_TOLERANCE);
}

void SketchPlugin_Sketch::attributeChanged(const std::string& theID) {
  if (theID == SketchPlugin_SketchEntity::EXTERNAL_ID()) {
    AttributeSelectionPtr aSelAttr = selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
    if (aSelAttr->context().get()) { // update arguments due to the selection value
      std::shared_ptr<GeomAPI_Shape> aSelection = aSelAttr->value();
      if (!aSelection.get()) aSelection = aSelAttr->context()->shape();
      // update the sketch plane
      std::shared_ptr<GeomAPI_Face> aFace;
      if (aSelection->isFace()) {
        aFace = aSelection->face();
      } else if (aSelection->isCompound()) {
        GeomAPI_ShapeIterator anIt(aSelection);
        aFace = anIt.current()->face();
      }
      if (aFace.get()) {
        std::shared_ptr<GeomAPI_Pln> aPlane = aFace->getPlane();
        if (aPlane.get()) {
          double anA, aB, aC, aD;
          aPlane->coefficients(anA, aB, aC, aD);

          // calculate attributes of the sketch
          std::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
          std::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
          std::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
          aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
          std::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
          // X axis is preferable to be dirX on the sketch
          // here can not be very small value to avoid very close to X normal axis (issue 595)
          static const double tol = 0.1;
          bool isX = fabs(anA) - 1.0 < tol && fabs(aB) < tol && fabs(aC) < tol;
          std::shared_ptr<GeomAPI_Dir> aTempDir(
            isX ? new GeomAPI_Dir(0, 1, 0) : new GeomAPI_Dir(1, 0, 0));
          std::shared_ptr<GeomAPI_Dir> aYDir(new GeomAPI_Dir(aNormDir->cross(aTempDir)));
          std::shared_ptr<GeomAPI_Dir> aXDir(new GeomAPI_Dir(aYDir->cross(aNormDir)));

          bool aWasBlocked = data()->blockSendAttributeUpdated(true);
          // update position of the sketch
          std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast
            <GeomDataAPI_Point>(data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
          anOrigin->setValue(anOrigPnt);
          std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
            data()->attribute(SketchPlugin_Sketch::NORM_ID()));
          aNormal->setValue(aNormDir);
          std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
            data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
          aDirX->setValue(aXDir);
          data()->blockSendAttributeUpdated(aWasBlocked, true);
        }
      }
    }
  } else if (theID == NORM_ID() || theID == DIRX_ID() || theID == ORIGIN_ID()) {
    // check if current and previous sketch planes are coordinate planes and they are different
    GeomAx3Ptr aCurPlane;
    bool areCoordPlanes = false;
    if (isPlaneSet()) {
      aCurPlane = coordinatePlane();
      areCoordPlanes = isCoordinatePlane(aCurPlane) && isCoordinatePlane(myPlane);
    }

    // send all sub-elements are also updated: all entities become created on different plane
    static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
    std::list<ObjectPtr> aSubs = data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->list();
    std::list<ObjectPtr>::iterator aSub = aSubs.begin();
    for(; aSub != aSubs.end(); aSub++) {
      if (aSub->get()) {
        if (areCoordPlanes)
          updateCoordinateAxis(*aSub, aCurPlane);

        ModelAPI_EventCreator::get()->sendUpdated(*aSub, anUpdateEvent);
      }
    }
    if (aCurPlane)
      myPlane = aCurPlane;
  }
}

void SketchPlugin_Sketch::createPoint2DResult(ModelAPI_Feature* theFeature,
                                              SketchPlugin_Sketch* theSketch,
                                              const std::string& theAttributeID, const int theIndex)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    theFeature->attribute(theAttributeID));

  if (!aPoint || !aPoint->isInitialized())
    return;

  std::shared_ptr<GeomAPI_Pnt> aCenter(theSketch->to3D(aPoint->x(), aPoint->y()));
  //std::cout<<"Execute circle "<<aCenter->x()<<" "<<aCenter->y()<<" "<<aCenter->z()<<std::endl;
  // make a visible point
  std::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);
  std::shared_ptr<ModelAPI_ResultConstruction> aResult = theFeature->document()->createConstruction(
                     theFeature->data(), theIndex);
  aResult->setShape(aCenterPointShape);
  aResult->setIsInHistory(false);

  theFeature->setResult(aResult, theIndex);
}

void SketchPlugin_Sketch::createLine2DResult(ModelAPI_Feature* theFeature,
                                             SketchPlugin_Sketch* theSketch,
                                             const std::string& theStartAttrID,
                                             const std::string& theEndAttrID,
                                             const int theIndex)
{
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theFeature->attribute(theStartAttrID));
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theFeature->attribute(theEndAttrID));

  if (!aStartAttr || !aStartAttr->isInitialized() ||
      !anEndAttr || !anEndAttr->isInitialized())
    return;

  std::shared_ptr<GeomAPI_Pnt> aStart(theSketch->to3D(aStartAttr->x(), aStartAttr->y()));
  std::shared_ptr<GeomAPI_Pnt> anEnd(theSketch->to3D(anEndAttr->x(), anEndAttr->y()));
  //std::cout<<"Execute line "<<aStart->x()<<" "<<aStart->y()<<" "<<aStart->z()<<" - "
  //  <<anEnd->x()<<" "<<anEnd->y()<<" "<<anEnd->z()<<std::endl;
  // make linear edge
  std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);
  // store the result
  std::shared_ptr<ModelAPI_ResultConstruction> aResult =
      theFeature->document()->createConstruction(theFeature->data(), theIndex);
  aResult->setShape(anEdge);
  aResult->setIsInHistory(false);
  theFeature->setResult(aResult, theIndex);
}

FeaturePtr SketchPlugin_Sketch::addUniqueNamedCopiedFeature(FeaturePtr theFeature,
                                                            SketchPlugin_Sketch* theSketch,
                                                            const bool theIsCopy)
{
  FeaturePtr aNewFeature = theSketch->addFeature(theFeature->getKind());
  // addFeature generates a unique name for the feature, it caches the name
  std::string aUniqueFeatureName = aNewFeature->data()->name();
  // all attribute values are copied\pasted to the new feature, name is not an exception
  theFeature->data()->copyTo(aNewFeature->data());
  // external state should not be copied as a new object is an object of the current sketch
  if (theFeature->selection(SketchPlugin_SketchEntity::EXTERNAL_ID()).get())
    aNewFeature->selection(SketchPlugin_SketchEntity::EXTERNAL_ID())->setValue(ResultPtr(),
                                                                               GeomShapePtr());
  aNewFeature->data()->setName(aUniqueFeatureName);
  // text expressions could block setValue of some attributes
  SketchPlugin_Tools::clearExpressions(aNewFeature);
  // Set copy attribute
  AttributeBooleanPtr anAttr = aNewFeature->data()->boolean(SketchPlugin_SketchEntity::COPY_ID());
  if(anAttr.get()) {
    anAttr->setValue(theIsCopy);
  }

  return aNewFeature;
}

std::shared_ptr<GeomAPI_Ax3> SketchPlugin_Sketch::plane(SketchPlugin_Sketch* theSketch)
{
  std::shared_ptr<ModelAPI_Data> aData = theSketch->data();

  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::NORM_ID()));

  return std::shared_ptr<GeomAPI_Ax3>(new GeomAPI_Ax3(anOrigin->pnt(), aDirX->dir(), aNorm->dir()));
}

bool SketchPlugin_Sketch::customAction(const std::string& theActionId)
{
  bool isOk = false;
  if (theActionId == ACTION_REMOVE_EXTERNAL())
    isOk = removeLinksToExternal();
  else {
    std::string aMsg = "Error: Feature \"%1\" does not support action \"%2\".";
    Events_InfoMessage("SketchPlugin_Sketch", aMsg).arg(getKind()).arg(theActionId).send();
  }
  return isOk;
}

static bool isExternalBased(const FeaturePtr theFeature)
{
  return theFeature->getKind() == SketchPlugin_Projection::ID() ||
         theFeature->getKind() == SketchPlugin_IntersectionPoint::ID();
}

bool SketchPlugin_Sketch::removeLinksToExternal()
{
  std::list<FeaturePtr> aRemove;
  std::list<ObjectPtr> aSubs = reflist(FEATURES_ID())->list();
  for (std::list<ObjectPtr>::iterator anIt = aSubs.begin(); anIt != aSubs.end(); ++anIt) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
    if (!aFeature)
      continue;
    if (isExternalBased(aFeature)) {
      // mark feature as to be removed
      aRemove.push_back(aFeature);
    }
    else {
      AttributeSelectionPtr anExtAttr =
          aFeature->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
      ResultPtr anExternal = anExtAttr ? anExtAttr->context() : ResultPtr();
      if (anExternal) {
        FeaturePtr anExtFeature = ModelAPI_Feature::feature(anExternal);
        if (anExtFeature && isExternalBased(anExtFeature)) {
          // make result of projection/intersection as non-external,
          aFeature->selection(SketchPlugin_SketchEntity::EXTERNAL_ID())->setValue(
            ObjectPtr(), GeomShapePtr());
          // set feature auxiliary if the parent is not included into sketch result
          bool isIncludedToSketchResult = false;
          if (anExtFeature->getKind() == SketchPlugin_Projection::ID()) {
            isIncludedToSketchResult = anExtFeature->boolean(
                SketchPlugin_Projection::INCLUDE_INTO_RESULT())->value();
          }
          if (anExtFeature->getKind() == SketchPlugin_IntersectionPoint::ID()) {
            isIncludedToSketchResult = anExtFeature->boolean(
                SketchPlugin_IntersectionPoint::INCLUDE_INTO_RESULT())->value();
          }

          aFeature->boolean(SketchPlugin_SketchEntity::COPY_ID())->setValue(false);
          aFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(
              !isIncludedToSketchResult);
        }
      }
    }
  }
  for (std::list<FeaturePtr>::iterator anIt = aRemove.begin(); anIt != aRemove.end(); ++anIt)
    document()->removeFeature(*anIt);
  return true;
}


static ObjectPtr findAxis(GeomShapePtr theAxisToCompare,
                          ObjectPtr theOX,
                          ObjectPtr theOY,
                          ObjectPtr theOZ)
{
  if (theAxisToCompare) {
    ObjectPtr anAxes[] = { theOX, theOY, theOZ };
    for (int i = 0; i < 3; ++i) {
      ResultPtr anAx = std::dynamic_pointer_cast<ModelAPI_Result>(anAxes[i]);
      if (anAx && theAxisToCompare->isEqual(anAx->shape()))
        return anAxes[i];
    }
  }
  return ObjectPtr();
}

static ObjectPtr findAxis(ObjectPtr theAxisToCompare,
                          ObjectPtr theOX,
                          ObjectPtr theOY,
                          ObjectPtr theOZ)
{
  if (theAxisToCompare == theOX)
    return theOX;
  else if (theAxisToCompare == theOY)
    return theOY;
  else if (theAxisToCompare == theOZ)
    return theOZ;
  // nothing helped, search by shape
  ResultPtr anAxis = std::dynamic_pointer_cast<ModelAPI_Result>(theAxisToCompare);
  return findAxis(anAxis ? anAxis->shape() : GeomShapePtr(), theOX, theOY, theOZ);
}

GeomShapePtr axisOnNewPlane(ObjectPtr theAxis, GeomAx3Ptr theOldPlane, GeomAx3Ptr theNewPlane)
{
  ResultPtr anAxis = std::dynamic_pointer_cast<ModelAPI_Result>(theAxis);
  if (!anAxis)
    return GeomShapePtr();

  GeomEdgePtr anAxisEdge = anAxis->shape()->edge();
  GeomLinePtr anAxisLine = anAxisEdge->line();
  GeomDirPtr anAxisDir = anAxisLine->direction();

  double aFirstParam, aLastParam;
  anAxisEdge->getRange(aFirstParam, aLastParam);

  if (theOldPlane->dirX()->isParallel(anAxisDir))
    anAxisDir = theNewPlane->dirX();
  else if (theOldPlane->dirY()->isParallel(anAxisDir))
    anAxisDir = theNewPlane->dirY();
  else if (theOldPlane->normal()->isParallel(anAxisDir))
    anAxisDir = theNewPlane->normal();

  GeomPointPtr aFirstPoint(new GeomAPI_Pnt(aFirstParam * anAxisDir->x(),
                                           aFirstParam * anAxisDir->y(),
                                           aFirstParam * anAxisDir->z()));
  GeomPointPtr aLastPoint(new GeomAPI_Pnt(aLastParam * anAxisDir->x(),
                                          aLastParam * anAxisDir->y(),
                                          aLastParam * anAxisDir->z()));
  return GeomAlgoAPI_EdgeBuilder::line(aFirstPoint, aLastPoint);
}

void  SketchPlugin_Sketch::updateCoordinateAxis(ObjectPtr theSub, GeomAx3Ptr thePlane)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theSub);
  if (!aFeature)
    return;

  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  ObjectPtr anOX = aRootDoc->objectByName(ModelAPI_ResultConstruction::group(), "OX");
  ObjectPtr anOY = aRootDoc->objectByName(ModelAPI_ResultConstruction::group(), "OY");
  ObjectPtr anOZ = aRootDoc->objectByName(ModelAPI_ResultConstruction::group(), "OZ");

  AttributeSelectionPtr anExtFeature;
  if (aFeature->getKind() == SketchPlugin_Projection::ID())
    anExtFeature = aFeature->selection(SketchPlugin_Projection::EXTERNAL_FEATURE_ID());
  else if (aFeature->getKind() == SketchPlugin_IntersectionPoint::ID())
    anExtFeature = aFeature->selection(SketchPlugin_IntersectionPoint::EXTERNAL_FEATURE_ID());
  else
    return;

  ObjectPtr aContext = anExtFeature->context();
  GeomShapePtr aShape = anExtFeature->value();
  if (!aShape) { // selected object is a construction
    ObjectPtr anAxis = findAxis(aContext, anOX, anOY, anOZ);
    GeomShapePtr aNewAxis = axisOnNewPlane(anAxis, myPlane, thePlane);
    anAxis = findAxis(aNewAxis, anOX, anOY, anOZ);
    if (anAxis)
      anExtFeature->setValue(anAxis, aShape);
  }
}
