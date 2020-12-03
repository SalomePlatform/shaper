// Copyright (C) 2020  CEA/DEN, EDF R&D
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

#include <SketchPlugin_Offset.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_BSpline.h>
#include <SketchPlugin_BSplinePeriodic.h>
#include <SketchPlugin_Tools.h>

#include <SketcherPrs_Factory.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Offset.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <GeomAPI_BSpline.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Wire.h>
#include <GeomAPI_WireExplorer.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>

static const double tolerance = 1.e-7;

SketchPlugin_Offset::SketchPlugin_Offset()
{
}

void SketchPlugin_Offset::initAttributes()
{
  data()->addAttribute(EDGES_ID(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(VALUE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId());

  // store original entities
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefList::typeId());
  // store offset entities
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
  // store mapping between original entity and index of the corresponding offset entity
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeIntArray::typeId());

  ModelAPI_Session::get()->validators()->
      registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_A());
  ModelAPI_Session::get()->validators()->
      registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_B());
  ModelAPI_Session::get()->validators()->
      registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_C());
}

void SketchPlugin_Offset::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch) return;

  // 1. Sketch plane
  std::shared_ptr<GeomAPI_Pln> aPlane = aSketch->plane();

  // 2. Offset value
  AttributeDoublePtr aValueAttr = real(VALUE_ID());
  if (!aValueAttr->isInitialized()) return;
  double aValue = aValueAttr->value();
  if (aValue < tolerance) return;

  // 2.a. Reversed?
  AttributeBooleanPtr aReversedAttr = boolean(REVERSED_ID());
  if (!aReversedAttr->isInitialized()) return;
  if (aReversedAttr->value()) aValue = -aValue; // reverse offset direction

  // 3. List of all selected edges
  AttributeRefListPtr aSelectedEdges = reflist(EDGES_ID());
  std::list<ObjectPtr> anEdgesList = aSelectedEdges->list();

  // 4. Put all selected edges in a set to pass them into findWireOneWay() below
  std::set<FeaturePtr> anEdgesSet;
  std::list<ObjectPtr>::const_iterator anEdgesIt = anEdgesList.begin();
  for (; anEdgesIt != anEdgesList.end(); anEdgesIt++) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anEdgesIt);
    if (aFeature) {
      anEdgesSet.insert(aFeature);
    }
  }

  // Wait all objects being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  // Save the current feature of the document, because new features may appear while executing.
  // In this case, they will become current. But if the number of copies is updated from outside
  // of sketch (e.g. by parameter change), the history line should not hold in sketch.
  keepCurrentFeature();

  // 5. Gather wires and make offset for each wire
  ListOfMakeShape anOffsetAlgos;
  std::set<FeaturePtr> aProcessedEdgesSet;
  for (anEdgesIt = anEdgesList.begin(); anEdgesIt != anEdgesList.end(); anEdgesIt++) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anEdgesIt);
    if (aFeature.get()) {
      if (aProcessedEdgesSet.find(aFeature) != aProcessedEdgesSet.end())
        continue;

      // 5.a. End points (if any)
      std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, anEndPoint;
      SketchPlugin_SegmentationTools::getFeaturePoints(aFeature, aStartPoint, anEndPoint);

      // 5.b. Find a chain of edges
      std::list<FeaturePtr> aChain;
      aChain.push_back(aFeature);
      bool isClosed = !(aStartPoint && anEndPoint);  // not closed edge
      if (!isClosed) {
        isClosed = findWireOneWay(aFeature, aFeature, aStartPoint, anEdgesSet,
                                  aProcessedEdgesSet, aChain, true);
        if (!isClosed) {
          isClosed = findWireOneWay(aFeature, aFeature, anEndPoint, anEdgesSet,
                                    aProcessedEdgesSet, aChain, false);
        }
      }
      aProcessedEdgesSet.insert(aFeature);

      // 5.c. Make wire
      ListOfShape aTopoChain;
      std::list<FeaturePtr>::iterator aChainIt = aChain.begin();
      for (; aChainIt != aChain.end(); ++aChainIt) {
        FeaturePtr aChainFeature = (*aChainIt);
        GeomShapePtr aTopoEdge = aChainFeature->lastResult()->shape();
        if (aTopoEdge->shapeType() == GeomAPI_Shape::EDGE) {
          aTopoChain.push_back(aTopoEdge);
        }
      }
      std::shared_ptr<GeomAlgoAPI_WireBuilder> aWireBuilder(
          new GeomAlgoAPI_WireBuilder(aTopoChain, !isClosed));

      GeomShapePtr aWireShape = aWireBuilder->shape();
      GeomWirePtr aWire (new GeomAPI_Wire (aWireShape));

      // Fix for a problem of offset side change with selection change.
      // Wire direction is defined by the first selected edge of this wire.
      double aSign = 1.;
      if (!aWire->isClosed()) {
        ListOfShape aModified;
        // First selected edge of current chain
        GeomShapePtr aFirstSel = aFeature->lastResult()->shape();
        aWireBuilder->modified(aFirstSel, aModified);
        GeomShapePtr aModFS = aModified.front();
        if (aModFS->orientation() != aFirstSel->orientation())
          aSign = -1.;
      }

      // 5.d. Make offset for the wire
      std::shared_ptr<GeomAlgoAPI_Offset> anOffsetShape(
          new GeomAlgoAPI_Offset(aPlane, aWireShape, aValue*aSign));

      std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeList(new GeomAlgoAPI_MakeShapeList);
      aMakeList->appendAlgo(aWireBuilder);
      aMakeList->appendAlgo(anOffsetShape);
      anOffsetAlgos.push_back(aMakeList);
    }
  }

  // 6. Store offset results.
  //    Create sketch feature for each edge of anOffsetShape, and also store
  //    created features in CREATED_ID() to remove them on next execute()
  addToSketch(anOffsetAlgos);

  restoreCurrentFeature();

  // send events to update the sub-features by the solver
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
}

bool SketchPlugin_Offset::findWireOneWay (const FeaturePtr& theFirstEdge,
                                          const FeaturePtr& theEdge,
                                          const std::shared_ptr<GeomDataAPI_Point2D>& theEndPoint,
                                          std::set<FeaturePtr>& theEdgesSet,
                                          std::set<FeaturePtr>& theProcessedEdgesSet,
                                          std::list<FeaturePtr>& theChain,
                                          const bool isPrepend)
{
  // 1. Find a single edge, coincident to theEndPoint by one of its ends
  if (!theEndPoint) return false;

  FeaturePtr aNextEdgeFeature;
  int nbFound = 0;

  std::set<AttributePoint2DPtr> aCoincPoints =
      SketchPlugin_Tools::findPointsCoincidentToPoint(theEndPoint);

  std::set<AttributePoint2DPtr>::iterator aPointsIt = aCoincPoints.begin();
  for (; aPointsIt != aCoincPoints.end(); aPointsIt++) {
    AttributePoint2DPtr aP = (*aPointsIt);
    FeaturePtr aCoincFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aP->owner());
    bool isInSet = (theEdgesSet.find(aCoincFeature) != theEdgesSet.end());

    // Condition 0: not auxiliary
    if (!isInSet && aCoincFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value())
      continue;

    // Condition 1: not a point feature
    if (aCoincFeature->getKind() != SketchPlugin_Point::ID()) {
      // Condition 2: it is not the current edge
      if (aCoincFeature != theEdge) {
        // Condition 3: it is in the set of interest.
        //              Empty set means all sketch edges.
        if (isInSet || theEdgesSet.empty()) {
          // Condition 4: consider only features with two end points
          std::shared_ptr<GeomDataAPI_Point2D> aP1, aP2;
          SketchPlugin_SegmentationTools::getFeaturePoints(aCoincFeature, aP1, aP2);
          if (aP1 && aP2) {
            // Condition 5: consider only features, that have aP as one of they ends.
            //              For example, we do not need an arc, coincident to aP by its center.
            if (theEndPoint->pnt()->isEqual(aP1->pnt()) ||
                theEndPoint->pnt()->isEqual(aP2->pnt())) {
              // Condition 6: only one edge can prolongate the chain. If several, we stop here.
              nbFound++;
              if (nbFound > 1)
                return false;

              // One found
              aNextEdgeFeature = aCoincFeature;
            }
          }
        }
      }
    }
  }

  // Only one edge can prolongate the chain. If several or none, we stop here.
  if (nbFound != 1)
    return false;

  // 2. So, we have the single edge, that prolongate the chain

  // Condition 7: if we reached the very first edge of the chain
  if (aNextEdgeFeature == theFirstEdge)
    // Closed chain found
    return true;

  // 3. Add the found edge to the chain
  if (isPrepend)
    theChain.push_front(aNextEdgeFeature);
  else
    theChain.push_back(aNextEdgeFeature);
  theProcessedEdgesSet.insert(aNextEdgeFeature);

  // 4. Which end of aNextEdgeFeature we need to proceed
  std::shared_ptr<GeomDataAPI_Point2D> aP1, aP2;
  SketchPlugin_SegmentationTools::getFeaturePoints(aNextEdgeFeature, aP1, aP2);
  if (aP2->pnt()->isEqual(theEndPoint->pnt())) {
    // reversed
    aP2 = aP1;
  }

  // 5. Continue gathering the chain (recursive)
  return findWireOneWay (theFirstEdge, aNextEdgeFeature, aP2, theEdgesSet,
                         theProcessedEdgesSet, theChain, isPrepend);
}

static void setRefListValue(AttributeRefListPtr theList, int theListSize,
                            ObjectPtr theValue, int theIndex)
{
  if (theIndex < theListSize) {
    ObjectPtr aCur = theList->object(theIndex);
    if (aCur != theValue)
      theList->substitute(aCur, theValue);
  }
  else
    theList->append(theValue);
}

// Reorder shapes according to the wire's order
static void reorderShapes(ListOfShape& theShapes, GeomShapePtr theWire)
{
  std::set<GeomShapePtr, GeomAPI_Shape::Comparator> aShapes;
  aShapes.insert(theShapes.begin(), theShapes.end());
  theShapes.clear();

  GeomWirePtr aWire(new GeomAPI_Wire(theWire));
  GeomAPI_WireExplorer anExp(aWire);
  for (; anExp.more(); anExp.next()) {
    GeomShapePtr aCurEdge = anExp.current();
    auto aFound = aShapes.find(aCurEdge);
    if (aFound != aShapes.end()) {
      theShapes.push_back(aCurEdge);
      aShapes.erase(aFound);
    }
  }
}

static void removeLastFromIndex(AttributeRefListPtr theList, int theListSize, int& theLastIndex)
{
  if (theLastIndex < theListSize) {
    std::set<int> anIndicesToRemove;
    for (; theLastIndex < theListSize; ++theLastIndex)
      anIndicesToRemove.insert(theLastIndex);
    theList->remove(anIndicesToRemove);
  }
}

void SketchPlugin_Offset::addToSketch(const ListOfMakeShape& theOffsetAlgos)
{
  AttributeRefListPtr aSelectedRefList = reflist(EDGES_ID());
  AttributeRefListPtr aBaseRefList = reflist(ENTITY_A());
  AttributeRefListPtr anOffsetRefList = reflist(ENTITY_B());
  AttributeIntArrayPtr anOffsetToBaseMap = intArray(ENTITY_C());

  // compare the list of selected edges and the previously stored,
  // and store maping between them
  std::map<ObjectPtr, std::list<ObjectPtr> > aMapExistent;
  std::list<ObjectPtr> anObjectsToRemove;
  std::list<ObjectPtr> aSelectedList = aSelectedRefList->list();
  for (std::list<ObjectPtr>::iterator aSIt = aSelectedList.begin();
       aSIt != aSelectedList.end(); ++aSIt) {
    aMapExistent[*aSIt] = std::list<ObjectPtr>();
  }
  for (int anIndex = 0, aSize = anOffsetRefList->size(); anIndex < aSize; ++anIndex) {
    ObjectPtr aCurrent = anOffsetRefList->object(anIndex);
    int aBaseIndex = anOffsetToBaseMap->value(anIndex);
    if (aBaseIndex >= 0) {
      ObjectPtr aBaseObj = aBaseRefList->object(aBaseIndex);
      std::map<ObjectPtr, std::list<ObjectPtr> >::iterator aFound = aMapExistent.find(aBaseObj);
      if (aFound != aMapExistent.end())
        aFound->second.push_back(aCurrent);
      else
        anObjectsToRemove.push_back(aCurrent);
    }
    else
      anObjectsToRemove.push_back(aCurrent);
  }

  // update lists of base shapes and of offset shapes
  int aBaseListSize = aBaseRefList->size();
  int anOffsetListSize = anOffsetRefList->size();
  int aBaseListIndex = 0, anOffsetListIndex = 0;
  std::list<int> anOffsetBaseBackRefs;
  std::set<GeomShapePtr, GeomAPI_Shape::ComparatorWithOri> aProcessedOffsets;
  for (std::list<ObjectPtr>::iterator aSIt = aSelectedList.begin();
       aSIt != aSelectedList.end(); ++aSIt) {
    // find an offseted edge
    FeaturePtr aBaseFeature = ModelAPI_Feature::feature(*aSIt);
    GeomShapePtr aBaseShape = aBaseFeature->lastResult()->shape();
    ListOfShape aNewShapes;
    for (ListOfMakeShape::const_iterator anAlgoIt = theOffsetAlgos.begin();
         anAlgoIt != theOffsetAlgos.end(); ++anAlgoIt) {
      (*anAlgoIt)->generated(aBaseShape, aNewShapes);
      if (!aNewShapes.empty()) {
        reorderShapes(aNewShapes, (*anAlgoIt)->shape());
        break;
      }
    }

    // store base feature
    setRefListValue(aBaseRefList, aBaseListSize, *aSIt, aBaseListIndex);

    // create or update an offseted feature
    const std::list<ObjectPtr>& anImages = aMapExistent[*aSIt];
    std::list<ObjectPtr>::const_iterator anImgIt = anImages.begin();
    for (ListOfShape::iterator aNewIt = aNewShapes.begin(); aNewIt != aNewShapes.end(); ++aNewIt) {
      FeaturePtr aNewFeature;
      if (anImgIt != anImages.end())
        aNewFeature = ModelAPI_Feature::feature(*anImgIt++);
      updateExistentOrCreateNew(*aNewIt, aNewFeature, anObjectsToRemove);
      aProcessedOffsets.insert(*aNewIt);

      // store an offseted feature
      setRefListValue(anOffsetRefList, anOffsetListSize, aNewFeature, anOffsetListIndex);

      anOffsetBaseBackRefs.push_back(aBaseListIndex);
      ++anOffsetListIndex;
    }
    ++aBaseListIndex;
    anObjectsToRemove.insert(anObjectsToRemove.end(), anImgIt, anImages.end());
  }
  // create arcs generated from vertices
  for (ListOfMakeShape::const_iterator anAlgoIt = theOffsetAlgos.begin();
       anAlgoIt != theOffsetAlgos.end(); ++anAlgoIt) {
    GeomShapePtr aCurWire = (*anAlgoIt)->shape();
    GeomAPI_ShapeExplorer anExp(aCurWire, GeomAPI_Shape::EDGE);
    for (; anExp.more(); anExp.next()) {
      GeomShapePtr aCurEdge = anExp.current();
      if (aProcessedOffsets.find(aCurEdge) == aProcessedOffsets.end()) {
        FeaturePtr aNewFeature;
        updateExistentOrCreateNew(aCurEdge, aNewFeature, anObjectsToRemove);
        aProcessedOffsets.insert(aCurEdge);

        // store an offseted feature
        setRefListValue(anOffsetRefList, anOffsetListSize, aNewFeature, anOffsetListIndex);

        anOffsetBaseBackRefs.push_back(-1);
        ++anOffsetListIndex;
      }
    }
  }

  removeLastFromIndex(aBaseRefList, aBaseListSize, aBaseListIndex);
  removeLastFromIndex(anOffsetRefList, anOffsetListSize, anOffsetListIndex);

  anOffsetToBaseMap->setSize((int)anOffsetBaseBackRefs.size(), false);
  int anIndex = 0;
  for (std::list<int>::iterator anIt = anOffsetBaseBackRefs.begin();
       anIt != anOffsetBaseBackRefs.end(); ++anIt) {
    anOffsetToBaseMap->setValue(anIndex++, *anIt, false);
  }

  // remove unused objects
  std::set<FeaturePtr> aSet;
  for (std::list<ObjectPtr>::iterator anIt = anObjectsToRemove.begin();
       anIt != anObjectsToRemove.end(); ++anIt) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
    if (aFeature)
      aSet.insert(aFeature);
  }
  ModelAPI_Tools::removeFeaturesAndReferences(aSet);
}

static void findOrCreateFeatureByKind(SketchPlugin_Sketch* theSketch,
                                      const std::string& theFeatureKind,
                                      FeaturePtr& theFeature,
                                      std::list<ObjectPtr>& thePoolOfFeatures)
{
  if (theFeature) {
    // check the feature type is the same as required
    if (theFeature->getKind() != theFeatureKind) {
      // return feature to the pool and try to find the most appropriate
      thePoolOfFeatures.push_back(theFeature);
      theFeature = FeaturePtr();
    }
  }
  if (!theFeature) {
    // try to find appropriate feature in the pool
    for (std::list<ObjectPtr>::iterator it = thePoolOfFeatures.begin();
         it != thePoolOfFeatures.end(); ++it) {
      FeaturePtr aCurFeature = ModelAPI_Feature::feature(*it);
      if (aCurFeature->getKind() == theFeatureKind) {
        theFeature = aCurFeature;
        thePoolOfFeatures.erase(it);
        break;
      }
    }
    // feature not found, create new
    if (!theFeature)
      theFeature = theSketch->addFeature(theFeatureKind);
  }
}

void SketchPlugin_Offset::updateExistentOrCreateNew(const GeomShapePtr& theShape,
                                                    FeaturePtr& theFeature,
                                                    std::list<ObjectPtr>& thePoolOfFeatures)
{
  if (theShape->shapeType() != GeomAPI_Shape::EDGE)
    return;

  std::shared_ptr<GeomAPI_Edge> aResEdge(new GeomAPI_Edge(theShape));

  std::shared_ptr<GeomAPI_Pnt2d> aFP, aLP;
  std::shared_ptr<GeomAPI_Pnt> aFP3d = aResEdge->firstPoint();
  std::shared_ptr<GeomAPI_Pnt> aLP3d = aResEdge->lastPoint();
  if (aFP3d && aLP3d) {
    aFP = sketch()->to2D(aFP3d);
    aLP = sketch()->to2D(aLP3d);
  }

  if (aResEdge->isLine()) {
    findOrCreateFeatureByKind(sketch(), SketchPlugin_Line::ID(), theFeature, thePoolOfFeatures);

    std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (theFeature->attribute(SketchPlugin_Line::START_ID()))->setValue(aFP);
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (theFeature->attribute(SketchPlugin_Line::END_ID()))->setValue(aLP);
  }
  else if (aResEdge->isArc()) {
    std::shared_ptr<GeomAPI_Circ> aCircEdge = aResEdge->circle();
    std::shared_ptr<GeomAPI_Pnt> aCP3d = aCircEdge->center();
    std::shared_ptr<GeomAPI_Pnt2d> aCP = sketch()->to2D(aCP3d);

    findOrCreateFeatureByKind(sketch(), SketchPlugin_Arc::ID(), theFeature, thePoolOfFeatures);

    GeomDirPtr aCircNormal = aCircEdge->normal();
    GeomDirPtr aSketchNormal = sketch()->coordinatePlane()->normal();
    if (aSketchNormal->dot(aCircNormal) < -tolerance)
      std::swap(aFP, aLP);

    bool aWasBlocked = theFeature->data()->blockSendAttributeUpdated(true);
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (theFeature->attribute(SketchPlugin_Arc::END_ID()))->setValue(aLP);
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (theFeature->attribute(SketchPlugin_Arc::START_ID()))->setValue(aFP);
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (theFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->setValue(aCP);
    theFeature->data()->blockSendAttributeUpdated(aWasBlocked);
  }
  else if (aResEdge->isCircle()) {
    std::shared_ptr<GeomAPI_Circ> aCircEdge = aResEdge->circle();
    std::shared_ptr<GeomAPI_Pnt> aCP3d = aCircEdge->center();
    std::shared_ptr<GeomAPI_Pnt2d> aCP = sketch()->to2D(aCP3d);

    findOrCreateFeatureByKind(sketch(), SketchPlugin_Circle::ID(), theFeature, thePoolOfFeatures);

    std::dynamic_pointer_cast<GeomDataAPI_Point2D>
      (theFeature->attribute(SketchPlugin_Circle::CENTER_ID()))->setValue(aCP);
    theFeature->real(SketchPlugin_Circle::RADIUS_ID())->setValue(aCircEdge->radius());
  }
  else if (aResEdge->isEllipse()) {
    std::shared_ptr<GeomAPI_Ellipse> anEllipseEdge = aResEdge->ellipse();

    GeomPointPtr aCP3d = anEllipseEdge->center();
    GeomPnt2dPtr aCP = sketch()->to2D(aCP3d);

    GeomPointPtr aFocus3d = anEllipseEdge->firstFocus();
    GeomPnt2dPtr aFocus = sketch()->to2D(aFocus3d);

    if (aFP3d && aLP3d) {
      // Elliptic arc
      findOrCreateFeatureByKind(sketch(), SketchPlugin_EllipticArc::ID(),
                                theFeature, thePoolOfFeatures);

      bool aWasBlocked = theFeature->data()->blockSendAttributeUpdated(true);
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
        (theFeature->attribute(SketchPlugin_EllipticArc::CENTER_ID()))->setValue(aCP);
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
        (theFeature->attribute(SketchPlugin_EllipticArc::FIRST_FOCUS_ID()))->setValue(aFocus);
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
        (theFeature->attribute(SketchPlugin_EllipticArc::START_POINT_ID()))->setValue(aFP);
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
        (theFeature->attribute(SketchPlugin_EllipticArc::END_POINT_ID()))->setValue(aLP);
      theFeature->data()->blockSendAttributeUpdated(aWasBlocked);
    }
    else {
      // Ellipse
      findOrCreateFeatureByKind(sketch(), SketchPlugin_Ellipse::ID(),
                                theFeature, thePoolOfFeatures);

      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
        (theFeature->attribute(SketchPlugin_Ellipse::CENTER_ID()))->setValue(aCP);
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>
        (theFeature->attribute(SketchPlugin_Ellipse::FIRST_FOCUS_ID()))->setValue(aFocus);
      theFeature->real(SketchPlugin_Ellipse::MINOR_RADIUS_ID())->setValue(
        anEllipseEdge->minorRadius());
    }
  }
  else {
    // convert to b-spline
    mkBSpline(theFeature, aResEdge, thePoolOfFeatures);
  }

  if (theFeature.get()) {
    theFeature->boolean(SketchPlugin_SketchEntity::COPY_ID())->setValue(true);
    theFeature->execute();

    static Events_ID aRedisplayEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
    ModelAPI_EventCreator::get()->sendUpdated(theFeature, aRedisplayEvent);
    const std::list<ResultPtr>& aResults = theFeature->results();
    for (std::list<ResultPtr>::const_iterator anIt = aResults.begin();
         anIt != aResults.end(); ++anIt)
      ModelAPI_EventCreator::get()->sendUpdated(*anIt, aRedisplayEvent);
  }
}

void SketchPlugin_Offset::mkBSpline (FeaturePtr& theResult,
                                     const GeomEdgePtr& theEdge,
                                     std::list<ObjectPtr>& thePoolOfFeatures)
{
  GeomCurvePtr aCurve (new GeomAPI_Curve (theEdge));
  // Forced conversion to b-spline, if aCurve is not b-spline
  GeomBSplinePtr aBSpline = GeomAPI_BSpline::convertToBSpline(aCurve);

  const std::string& aBSplineKind = aBSpline->isPeriodic() ? SketchPlugin_BSplinePeriodic::ID()
                                                           : SketchPlugin_BSpline::ID();
  findOrCreateFeatureByKind(sketch(), aBSplineKind, theResult, thePoolOfFeatures);

  theResult->integer(SketchPlugin_BSpline::DEGREE_ID())->setValue(aBSpline->degree());

  AttributePoint2DArrayPtr aPolesAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>
    (theResult->attribute(SketchPlugin_BSpline::POLES_ID()));
  std::list<GeomPointPtr> aPoles = aBSpline->poles();
  aPolesAttr->setSize((int)aPoles.size());
  std::list<GeomPointPtr>::iterator anIt = aPoles.begin();
  for (int anIndex = 0; anIt != aPoles.end(); ++anIt, ++anIndex) {
    GeomPnt2dPtr aPoleInSketch = sketch()->to2D(*anIt);
    aPolesAttr->setPnt(anIndex, aPoleInSketch);
  }

  AttributeDoubleArrayPtr aWeightsAttr =
      theResult->data()->realArray(SketchPlugin_BSpline::WEIGHTS_ID());
  std::list<double> aWeights = aBSpline->weights();
  if (aWeights.empty()) { // rational B-spline
    int aSize = (int)aPoles.size();
    aWeightsAttr->setSize(aSize);
    for (int anIndex = 0; anIndex < aSize; ++anIndex)
      aWeightsAttr->setValue(anIndex, 1.0);
  }
  else { // non-rational B-spline
    aWeightsAttr->setSize((int)aWeights.size());
    std::list<double>::iterator anIt = aWeights.begin();
    for (int anIndex = 0; anIt != aWeights.end(); ++anIt, ++anIndex)
      aWeightsAttr->setValue(anIndex, *anIt);
  }

  AttributeDoubleArrayPtr aKnotsAttr =
      theResult->data()->realArray(SketchPlugin_BSpline::KNOTS_ID());
  std::list<double> aKnots = aBSpline->knots();
  int aSize = (int)aKnots.size();
  aKnotsAttr->setSize(aSize);
  std::list<double>::iterator aKIt = aKnots.begin();
  for (int index = 0; index < aSize; ++index, ++aKIt)
    aKnotsAttr->setValue(index, *aKIt);

  AttributeIntArrayPtr aMultsAttr =
      theResult->data()->intArray(SketchPlugin_BSpline::MULTS_ID());
  std::list<int> aMultiplicities = aBSpline->mults();
  aSize = (int)aMultiplicities.size();
  aMultsAttr->setSize(aSize);
  std::list<int>::iterator aMIt = aMultiplicities.begin();
  for (int index = 0; index < aSize; ++index, ++aMIt)
    aMultsAttr->setValue(index, *aMIt);
}

void SketchPlugin_Offset::attributeChanged(const std::string& theID)
{
  if (theID == EDGES_ID()) {
    AttributeRefListPtr aSelected = reflist(EDGES_ID());
    if (aSelected->size() == 0) {
      // Clear list of objects
      AttributeRefListPtr anOffsetAttr = reflist(SketchPlugin_Constraint::ENTITY_B());
      std::list<ObjectPtr> anOffsetList = anOffsetAttr->list();
      std::set<FeaturePtr> aFeaturesToBeRemoved;
      for (std::list<ObjectPtr>::iterator anIt = anOffsetList.begin();
           anIt != anOffsetList.end(); ++anIt) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
        if (aFeature)
          aFeaturesToBeRemoved.insert(aFeature);
      }

      reflist(SketchPlugin_Constraint::ENTITY_A())->clear();
      anOffsetAttr->clear();
      intArray(SketchPlugin_Constraint::ENTITY_C())->setSize(0);

      ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved);
    }
  }
}

bool SketchPlugin_Offset::customAction(const std::string& theActionId)
{
  bool isOk = false;
  if (theActionId == ADD_WIRE_ACTION_ID()) {
    isOk = findWires();
  }
  else {
    std::string aMsg = "Error: Feature \"%1\" does not support action \"%2\".";
    Events_InfoMessage("SketchPlugin_Offset", aMsg).arg(getKind()).arg(theActionId).send();
  }
  return isOk;
}

bool SketchPlugin_Offset::findWires()
{
  AttributeRefListPtr aSelectedEdges = reflist(EDGES_ID());
  std::list<ObjectPtr> anEdgesList = aSelectedEdges->list();

  // Empty set
  std::set<FeaturePtr> anEdgesSet;

  // Processed set
  std::set<FeaturePtr> aProcessedEdgesSet;

  // Put all selected edges in a set to avoid adding them in reflist(EDGES_ID())
  std::set<FeaturePtr> aSelectedSet;
  std::list<ObjectPtr>::const_iterator anEdgesIt = anEdgesList.begin();
  for (; anEdgesIt != anEdgesList.end(); anEdgesIt++) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anEdgesIt);
    if (aFeature) {
      aSelectedSet.insert(aFeature);
    }
  }

  bool aWasBlocked = data()->blockSendAttributeUpdated(true);

  // Gather chains of edges
  for (anEdgesIt = anEdgesList.begin(); anEdgesIt != anEdgesList.end(); anEdgesIt++) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anEdgesIt);
    if (aFeature.get()) {
      if (aProcessedEdgesSet.find(aFeature) != aProcessedEdgesSet.end())
        continue;
      aProcessedEdgesSet.insert(aFeature);

      // End points (if any)
      std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, anEndPoint;
      SketchPlugin_SegmentationTools::getFeaturePoints(aFeature, aStartPoint, anEndPoint);

      std::list<FeaturePtr> aChain;
      aChain.push_back(aFeature);
      bool isClosed = findWireOneWay(aFeature, aFeature, aStartPoint, anEdgesSet,
                                     aProcessedEdgesSet, aChain, true);
      if (!isClosed) {
        findWireOneWay(aFeature, aFeature, anEndPoint, anEdgesSet,
                       aProcessedEdgesSet, aChain, false);
      }

      std::list<FeaturePtr>::iterator aChainIt = aChain.begin();
      for (; aChainIt != aChain.end(); ++aChainIt) {
        FeaturePtr aChainFeature = (*aChainIt);
        if (aSelectedSet.find(aChainFeature) == aSelectedSet.end()) {
          aSelectedEdges->append(aChainFeature->lastResult());
        }
      }
    }
  }
  // TODO: hilight selection in the viewer

  data()->blockSendAttributeUpdated(aWasBlocked);
  return true;
}


AISObjectPtr SketchPlugin_Offset::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::offsetObject(this, sketch(),
    thePrevious);
  return anAIS;
}
