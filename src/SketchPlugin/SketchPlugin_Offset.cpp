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
#include <SketchPlugin_Tools.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Offset.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Circ.h>

#include <GeomDataAPI_Point2D.h>

#include <iostream>

SketchPlugin_Offset::SketchPlugin_Offset()
  : SketchPlugin_SketchEntity()
{
}

void SketchPlugin_Offset::initDerivedClassAttributes()
{
  data()->addAttribute(EDGES_ID(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(VALUE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId());
}

void SketchPlugin_Offset::execute()
{
  ModelAPI_Tools::removeFeaturesAndReferences(myCreatedFeatures);
  myCreatedFeatures.clear();

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch) return;

  // 1. Sketch plane
  std::shared_ptr<GeomAPI_Pln> aPlane = aSketch->plane();

  // 2. Offset value
  AttributeDoublePtr aValueAttr = real(VALUE_ID());
  if (!aValueAttr->isInitialized()) return;
  double aValue = aValueAttr->value();
  const double tolerance = 1.e-7;
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

  // 5. Gather wires and make offset for each wire
  for (anEdgesIt = anEdgesList.begin(); anEdgesIt != anEdgesList.end(); anEdgesIt++) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anEdgesIt);
    if (aFeature.get()) {
      if (anEdgesSet.find(aFeature) == anEdgesSet.end())
        continue;

      // 5.a. End points (if any)
      std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, anEndPoint;
      SketchPlugin_SegmentationTools::getFeaturePoints(aFeature, aStartPoint, anEndPoint);

      // 5.b. Find a chain of edges
      std::list<FeaturePtr> aChain;
      aChain.push_back(aFeature);
      if (aStartPoint && anEndPoint) { // not closed edge
        bool isClosed = findWireOneWay(aFeature, aFeature, aStartPoint, anEdgesSet, aChain);
        if (!isClosed)
          findWireOneWay(aFeature, aFeature, anEndPoint, anEdgesSet, aChain);
      }
      std::set<FeaturePtr>::iterator aPos = anEdgesSet.find(aFeature);
      if (aPos != anEdgesSet.end())
        anEdgesSet.erase(aPos);

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
      GeomShapePtr anEdgeOrWire = GeomAlgoAPI_WireBuilder::wire(aTopoChain);

      // 5.d. Make offset for each wire
      std::shared_ptr<GeomAPI_Shape> anOffsetShape =
        GeomAlgoAPI_Offset::OffsetInPlane(aPlane, anEdgeOrWire, aValue);

      // 5.e. Store offset results.
      //      Create sketch feature for each edge of anOffsetShape, and also store
      //      created features in myCreatedFeatures to remove them on next execute()
      addToSketch(anOffsetShape);
    }
  }
}

bool SketchPlugin_Offset::findWireOneWay (const FeaturePtr& theFirstEdge,
                                          const FeaturePtr& theEdge,
                                          const std::shared_ptr<GeomDataAPI_Point2D>& theEndPoint,
                                          std::set<FeaturePtr>& theEdgesSet,
                                          std::list<FeaturePtr>& theChain)
{
  // 1. Find a single edge, coincident to theEndPoint by one of its ends
  if (!theEndPoint) return false;

  std::shared_ptr<GeomAPI_Pnt2d> aP2d = theEndPoint->pnt();

  FeaturePtr aNextEdgeFeature;
  int nbFound = 0;

  std::set<AttributePoint2DPtr> aCoincPoints =
    SketchPlugin_Tools::findPointsCoincidentToPoint(theEndPoint);

  std::set<AttributePoint2DPtr>::iterator aPointsIt = aCoincPoints.begin();
  for (; aPointsIt != aCoincPoints.end(); aPointsIt++) {
    AttributePoint2DPtr aP = (*aPointsIt);
    FeaturePtr aCoincFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aP->owner());

    // Condition 1: not a point feature
    if (aCoincFeature->getKind() != SketchPlugin_Point::ID()) {
      // Condition 2: it is not the current edge
      if (aCoincFeature != theEdge) {
        // Condition 3: it is in the set of interest.
        //              Empty set means all sketch edges.
        bool isInSet = true;
        if (theEdgesSet.size()) {
          isInSet = (theEdgesSet.find(aCoincFeature) != theEdgesSet.end());
        }
        if (isInSet) {
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
  theChain.push_back(aNextEdgeFeature);
  // remove from the set, if the set is used
  if (theEdgesSet.size()) {
    std::set<FeaturePtr>::iterator aPos = theEdgesSet.find(aNextEdgeFeature);
    if (aPos != theEdgesSet.end())
      theEdgesSet.erase(aPos);
  }

  // 4. Which end of aNextEdgeFeature we need to proceed
  std::shared_ptr<GeomDataAPI_Point2D> aP1, aP2;
  SketchPlugin_SegmentationTools::getFeaturePoints(aNextEdgeFeature, aP1, aP2);
  if (aP2->pnt()->isEqual(theEndPoint->pnt())) {
    // reversed
    aP2 = aP1;
  }

  // 5. Continue gathering the chain (recursive)
  return findWireOneWay (theFirstEdge, aNextEdgeFeature, aP2, theEdgesSet, theChain);
}

void SketchPlugin_Offset::addToSketch(const std::shared_ptr<GeomAPI_Shape>& anOffsetShape)
{
  //GeomAPI_ShapeExplorer::GeomAPI_ShapeExplorer
  ListOfShape aResEdges = GeomAlgoAPI_ShapeTools::getLowLevelSubShapes(anOffsetShape);
  std::list<GeomShapePtr>::const_iterator aResEdgesIt = aResEdges.begin();
  for (; aResEdgesIt != aResEdges.end(); aResEdgesIt++) {
    GeomShapePtr aResShape = (*aResEdgesIt);
    if (aResShape->shapeType() == GeomAPI_Shape::EDGE) {
      // Add new feature
      FeaturePtr aResFeature;
      std::shared_ptr<GeomAPI_Edge> aResEdge (new GeomAPI_Edge(aResShape));

      std::shared_ptr<GeomAPI_Pnt2d> aFP, aLP;
      std::shared_ptr<GeomAPI_Pnt> aFP3d = aResEdge->firstPoint();
      std::shared_ptr<GeomAPI_Pnt> aLP3d = aResEdge->lastPoint();
      if (aFP3d.get() && aLP3d.get()) {
        aFP = sketch()->to2D(aFP3d);
        aLP = sketch()->to2D(aLP3d);
      }

      if (aResEdge->isLine()) {
        aResFeature = sketch()->addFeature(SketchPlugin_Line::ID());

        std::dynamic_pointer_cast<GeomDataAPI_Point2D>
          (aResFeature->attribute(SketchPlugin_Line::START_ID()))->setValue(aFP);
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>
          (aResFeature->attribute(SketchPlugin_Line::END_ID()))->setValue(aLP);
      }
      else if (aResEdge->isArc()) {
        std::shared_ptr<GeomAPI_Circ> aCircEdge = aResEdge->circle();
        std::shared_ptr<GeomAPI_Pnt> aCP3d = aCircEdge->center();
        std::shared_ptr<GeomAPI_Pnt2d> aCP = sketch()->to2D(aCP3d);

        aResFeature = sketch()->addFeature(SketchPlugin_Arc::ID());

        bool aWasBlocked = aResFeature->data()->blockSendAttributeUpdated(true);
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>
          (aResFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->setValue(aCP);
       std::dynamic_pointer_cast<GeomDataAPI_Point2D>
          (aResFeature->attribute(SketchPlugin_Arc::START_ID()))->setValue(aFP);
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>
          (aResFeature->attribute(SketchPlugin_Arc::END_ID()))->setValue(aLP);
        aResFeature->data()->blockSendAttributeUpdated(aWasBlocked);
      }
      else if (aResEdge->isCircle()) {
        std::shared_ptr<GeomAPI_Circ> aCircEdge = aResEdge->circle();
        std::shared_ptr<GeomAPI_Pnt> aCP3d = aCircEdge->center();
        std::shared_ptr<GeomAPI_Pnt2d> aCP = sketch()->to2D(aCP3d);

        aResFeature = sketch()->addFeature(SketchPlugin_Circle::ID());
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>
          (aResFeature->attribute(SketchPlugin_Circle::CENTER_ID()))->setValue(aCP);
        aResFeature->real(SketchPlugin_Circle::RADIUS_ID())->setValue(aCircEdge->radius());
      }
      else {
      }

      if (aResFeature.get()) {
        myCreatedFeatures.insert(aResFeature);

        aResFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue
          (boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value());
        aResFeature->execute();
      }
    }
  }
}

void SketchPlugin_Offset::attributeChanged(const std::string& theID)
{
  ModelAPI_Tools::removeFeaturesAndReferences(myCreatedFeatures);
  myCreatedFeatures.clear();
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
  std::set<FeaturePtr> aProcessedSet;

  // Put all selected edges in a set to avoid adding them in reflist(EDGES_ID())
  std::set<FeaturePtr> aSelectedSet;
  std::list<ObjectPtr>::const_iterator anEdgesIt = anEdgesList.begin();
  for (; anEdgesIt != anEdgesList.end(); anEdgesIt++) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anEdgesIt);
    if (aFeature) {
      aSelectedSet.insert(aFeature);
    }
  }

  // Gather chains of edges
  for (anEdgesIt = anEdgesList.begin(); anEdgesIt != anEdgesList.end(); anEdgesIt++) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anEdgesIt);
    if (aFeature.get()) {
      if (aProcessedSet.find(aFeature) != aProcessedSet.end())
        continue;
      aProcessedSet.insert(aFeature);

      // End points (if any)
      std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, anEndPoint;
      SketchPlugin_SegmentationTools::getFeaturePoints(aFeature, aStartPoint, anEndPoint);

      std::list<FeaturePtr> aChain;
      aChain.push_back(aFeature);
      bool isClosed = findWireOneWay(aFeature, aFeature, aStartPoint, anEdgesSet, aChain);
      if (!isClosed)
        findWireOneWay(aFeature, aFeature, anEndPoint, anEdgesSet, aChain);

      std::list<FeaturePtr>::iterator aChainIt = aChain.begin();
      for (; aChainIt != aChain.end(); ++aChainIt) {
        FeaturePtr aChainFeature = (*aChainIt);
        aProcessedSet.insert(aChainFeature);
        if (aSelectedSet.find(aChainFeature) == aSelectedSet.end()) {
          aSelectedEdges->append(aChainFeature->lastResult());
        }
      }
    }
  }
  // TODO: hilight selection in the viewer

  return true;
}
