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

#include <FeaturesPlugin_Fillet1D.h>
#include <FeaturesPlugin_Tools.h>

#include <GeomAlgoAPI_Fillet1D.h>
#include <GeomAlgoAPI_MapShapesAndAncestors.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_WireExplorer.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>

FeaturesPlugin_Fillet1D::FeaturesPlugin_Fillet1D()
{
}

void FeaturesPlugin_Fillet1D::initAttributes()
{
  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(WIRE_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(VERTEX_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
}

void FeaturesPlugin_Fillet1D::execute()
{
  ListOfShape aWires;
  MapShapeSubs aVertices;
  if (!baseShapes(aWires, aVertices))
    return;

  int aResultIndex = 0;
  for (ListOfShape::iterator anIt = aWires.begin(); anIt != aWires.end(); ++anIt)
    if (!performFillet(*anIt, aVertices[*anIt], aResultIndex++))
      break;
  removeResults(aResultIndex);
}

bool FeaturesPlugin_Fillet1D::baseShapes(ListOfShape& theWires, MapShapeSubs& theWireVertices)
{
  std::set<GeomShapePtr, GeomAPI_Shape::Comparator> aProcessedWires;
  std::string aMethod = string(CREATION_METHOD())->value();
  if (aMethod == CREATION_BY_WIRES()) {
    AttributeSelectionListPtr aSelList = selectionList(WIRE_LIST_ID());

    int aNbSel = aSelList->size();
    for (int ind = 0; ind < aNbSel; ++ind) {
      AttributeSelectionPtr aCurSel = aSelList->value(ind);
      GeomShapePtr aWire = aCurSel->context()->shape();
      if (aProcessedWires.find(aWire) != aProcessedWires.end())
        continue;

      aProcessedWires.insert(aWire);
      // get vertices applicable for fillet
      GeomAlgoAPI_MapShapesAndAncestors aMapVE(aWire, GeomAPI_Shape::VERTEX, GeomAPI_Shape::EDGE);
      const MapShapeToShapes& aSubshapes = aMapVE.map();
      std::set<GeomShapePtr, GeomAPI_Shape::Comparator> aFilletVertices;
      for (MapShapeToShapes::const_iterator anIt = aSubshapes.begin();
           anIt != aSubshapes.end(); ++anIt) {
        // vertex should have 2 adjacent edges
        if (anIt->second.size() != 2)
          continue;

        // skip vertices, which adjacent edges are not on the same plane
        ListOfShape anEdges;
        anEdges.insert(anEdges.end(), anIt->second.begin(), anIt->second.end());
        GeomPlanePtr aPlane = GeomAlgoAPI_ShapeTools::findPlane(anEdges);
        if (!aPlane)
          continue;

        // skip vertices, which smoothly connect adjacent edges
        GeomEdgePtr anEdge1(new GeomAPI_Edge(anEdges.front()));
        GeomEdgePtr anEdge2(new GeomAPI_Edge(anEdges.back()));
        GeomVertexPtr aSharedVertex(new GeomAPI_Vertex(anIt->first));
        if (GeomAlgoAPI_ShapeTools::isTangent(anEdge1, anEdge2, aSharedVertex))
          continue;

        aFilletVertices.insert(anIt->first);
      }

      if (aFilletVertices.empty()) {
        setError("Wire has no vertices for fillet.");
        return false;
      }


      // keep the sequence of wires and fillet vertices stable
      theWires.push_back(aWire);
      for (GeomAPI_WireExplorer anExp(aWire->wire()); anExp.more(); anExp.next()) {
        GeomShapePtr aVertex = anExp.currentVertex();
        if (aFilletVertices.find(aVertex) != aFilletVertices.end())
          theWireVertices[aWire].push_back(aVertex);
      }
    }
  }
  else if (aMethod == CREATION_BY_VERTICES()) {
    AttributeSelectionListPtr aSelList = selectionList(VERTEX_LIST_ID());
    int aNbSel = aSelList->size();
    for (int ind = 0; ind < aNbSel; ++ind) {
      AttributeSelectionPtr aCurSel = aSelList->value(ind);
      GeomShapePtr aWire = aCurSel->context()->shape();
      GeomShapePtr aVertex = aCurSel->value();

      // keep the sequence of wires stable
      if (aProcessedWires.find(aWire) == aProcessedWires.end()) {
        theWires.push_back(aWire);
        aProcessedWires.insert(aWire);
      }

      theWireVertices[aWire].push_back(aVertex);
    }
  }
  return true;
}

bool FeaturesPlugin_Fillet1D::performFillet(const GeomShapePtr& theWire,
                                            const ListOfShape& theVertices,
                                            const int theResultIndex)
{
  double aRadius = real(RADIUS_ID())->value();

  // perform fillet operation
  std::shared_ptr<GeomAlgoAPI_Fillet1D> aFilletBuilder(
      new GeomAlgoAPI_Fillet1D(theWire, theVertices, aRadius));

  bool isOk = true;
  bool isSendMessage = !myFailedVertices.empty();
  myFailedVertices = aFilletBuilder->failedVertices();

  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aFilletBuilder, getKind(), anError)) {
    isOk = false;
    // in case of vertices, the fillet completed, send message to highlight them in the viewer
    isSendMessage = true;
    bool isAllFailed = myFailedVertices.size() == theVertices.size();
    setError(anError, isAllFailed);
    if (isAllFailed)
      return isOk;
  }

  if (isSendMessage) {
    // send message to highlight the failed vertices
    std::shared_ptr<ModelAPI_ShapesFailedMessage> aMessage(
        new ModelAPI_ShapesFailedMessage(Events_Loop::eventByName(EVENT_OPERATION_SHAPES_FAILED)));
    aMessage->setShapes(myFailedVertices);
    Events_Loop::loop()->send(aMessage);
  }

  static const std::string THE_PREFIX = "Fillet1D";

  // store result
  ResultBodyPtr aResult = document()->createBody(data(), theResultIndex);
  ListOfShape anOriginal;
  anOriginal.push_back(theWire);
  FeaturesPlugin_Tools::loadModifiedShapes(aResult, anOriginal, ListOfShape(),
                                           aFilletBuilder, aFilletBuilder->shape(), THE_PREFIX);
  setResult(aResult, theResultIndex);
  // store new edges generated from vertices
  for (ListOfShape::const_iterator anIt = theVertices.begin(); anIt != theVertices.end(); ++anIt)
    aResult->loadGeneratedShapes(aFilletBuilder, *anIt, GeomAPI_Shape::VERTEX, THE_PREFIX, true);

  return isOk;
}
