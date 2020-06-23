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
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Offset.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_BSpline.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>

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
        bool isClosed = findWireOneWay(aFeature, aFeature, aStartPoint, anEdgesSet, aChain, true);
        if (!isClosed)
          findWireOneWay(aFeature, aFeature, anEndPoint, anEdgesSet, aChain, false);
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
                                          std::list<FeaturePtr>& theChain,
                                          const bool isPrepend)
{
  // 1. Find a single edge, coincident to theEndPoint by one of its ends
  if (!theEndPoint) return false;

  std::shared_ptr<GeomAPI_Pnt2d> aP2d = theEndPoint->pnt();

  FeaturePtr aNextEdgeFeature;
  int nbFound = 0;

  std::set<AttributePoint2DPtr> aCoincPoints;
  std::map<AttributePoint2DArrayPtr, int> aCoincPointsInArray;
  SketchPlugin_Tools::findPointsCoincidentToPoint(theEndPoint, aCoincPoints, aCoincPointsInArray);

  // store all found attributes to a single array
  std::set<AttributePtr> anAllCoincPoints;
  anAllCoincPoints.insert(aCoincPoints.begin(), aCoincPoints.end());
  for (auto it = aCoincPointsInArray.begin(); it != aCoincPointsInArray.end(); ++it)
    anAllCoincPoints.insert(it->first);

  std::set<AttributePtr>::iterator aPointsIt = anAllCoincPoints.begin();
  for (; aPointsIt != anAllCoincPoints.end(); aPointsIt++) {
    AttributePtr aP = (*aPointsIt);
    FeaturePtr aCoincFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aP->owner());

    // Condition 0: not auxiliary
    if (aCoincFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value()) continue;

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
  if (isPrepend)
    theChain.push_front(aNextEdgeFeature);
  else
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
  return findWireOneWay (theFirstEdge, aNextEdgeFeature, aP2, theEdgesSet, theChain, isPrepend);
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
      //if (aFP3d.get() && aLP3d.get()) {
      if (aFP3d && aLP3d) {
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
      else if (aResEdge->isEllipse()) {
        std::shared_ptr<GeomAPI_Ellipse> anEllipseEdge = aResEdge->ellipse();

        GeomPointPtr aCP3d = anEllipseEdge->center();
        GeomPnt2dPtr aCP = sketch()->to2D(aCP3d);

        GeomPointPtr aFocus3d = anEllipseEdge->firstFocus();
        GeomPnt2dPtr aFocus = sketch()->to2D(aFocus3d);

        if (aFP3d && aLP3d) {
          // Elliptic arc
          aResFeature = sketch()->addFeature(SketchPlugin_EllipticArc::ID());

          bool aWasBlocked = aResFeature->data()->blockSendAttributeUpdated(true);
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>
            (aResFeature->attribute(SketchPlugin_EllipticArc::CENTER_ID()))->setValue(aCP);
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>
            (aResFeature->attribute(SketchPlugin_EllipticArc::FIRST_FOCUS_ID()))->setValue(aFocus);
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>
            (aResFeature->attribute(SketchPlugin_EllipticArc::START_POINT_ID()))->setValue(aFP);
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>
            (aResFeature->attribute(SketchPlugin_EllipticArc::END_POINT_ID()))->setValue(aLP);
          aResFeature->data()->blockSendAttributeUpdated(aWasBlocked);
        }
        else {
          // Ellipse
          aResFeature = sketch()->addFeature(SketchPlugin_Ellipse::ID());

          std::dynamic_pointer_cast<GeomDataAPI_Point2D>
            (aResFeature->attribute(SketchPlugin_Ellipse::CENTER_ID()))->setValue(aCP);
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>
            (aResFeature->attribute(SketchPlugin_Ellipse::FIRST_FOCUS_ID()))->setValue(aFocus);
          aResFeature->real(SketchPlugin_Ellipse::MINOR_RADIUS_ID())->setValue(anEllipseEdge->minorRadius());
        }
      }
      else if (aResEdge->isBSpline()) {
        mkBSpline(aResFeature, aResEdge);
      }
      else {
        // convert to b-spline
        mkBSpline(aResFeature, aResEdge);
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

void SketchPlugin_Offset::mkBSpline (FeaturePtr& theResult,
                                     const GeomEdgePtr& theEdge)
{
  GeomCurvePtr aCurve (new GeomAPI_Curve (theEdge));
  // Forced conversion to b-spline, if aCurve is not b-spline
  GeomAPI_BSpline aBSpline (aCurve, /*isForced*/true);

  if (aBSpline.isPeriodic())
    theResult = sketch()->addFeature(SketchPlugin_BSplinePeriodic::ID());
  else
    theResult = sketch()->addFeature(SketchPlugin_BSpline::ID());

  theResult->integer(SketchPlugin_BSpline::DEGREE_ID())->setValue(aBSpline.degree());

  AttributePoint2DArrayPtr aPolesAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>
    (theResult->attribute(SketchPlugin_BSpline::POLES_ID()));
  std::list<GeomPointPtr> aPoles = aBSpline.poles();
  aPolesAttr->setSize((int)aPoles.size());
  std::list<GeomPointPtr>::iterator anIt = aPoles.begin();
  for (int anIndex = 0; anIt != aPoles.end(); ++anIt, ++anIndex) {
    GeomPnt2dPtr aPoleInSketch = sketch()->to2D(*anIt);
    aPolesAttr->setPnt(anIndex, aPoleInSketch);
  }

  AttributeDoubleArrayPtr aWeightsAttr =
      theResult->data()->realArray(SketchPlugin_BSpline::WEIGHTS_ID());
  std::list<double> aWeights = aBSpline.weights();
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
  std::list<double> aKnots = aBSpline.knots();
  int aSize = (int)aKnots.size();
  aKnotsAttr->setSize(aSize);
  std::list<double>::iterator aKIt = aKnots.begin();
  for (int index = 0; index < aSize; ++index, ++aKIt)
    aKnotsAttr->setValue(index, *aKIt);

  AttributeIntArrayPtr aMultsAttr =
      theResult->data()->intArray(SketchPlugin_BSpline::MULTS_ID());
  std::list<int> aMultiplicities = aBSpline.mults();
  aSize = (int)aMultiplicities.size();
  aMultsAttr->setSize(aSize);
  std::list<int>::iterator aMIt = aMultiplicities.begin();
  for (int index = 0; index < aSize; ++index, ++aMIt)
    aMultsAttr->setValue(index, *aMIt);
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
      bool isClosed = findWireOneWay(aFeature, aFeature, aStartPoint, anEdgesSet, aChain, true);
      if (!isClosed)
        findWireOneWay(aFeature, aFeature, anEndPoint, anEdgesSet, aChain, false);

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


AISObjectPtr SketchPlugin_Offset::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::offsetObject(this, sketch(),
    thePrevious);
  return anAIS;
}
