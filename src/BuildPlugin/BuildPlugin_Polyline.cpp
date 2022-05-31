// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "BuildPlugin_Polyline.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <Events_InfoMessage.h>

#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <algorithm>

//=================================================================================================
BuildPlugin_Polyline::BuildPlugin_Polyline()
{
}

//=================================================================================================
void BuildPlugin_Polyline::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(CLOSED_ID(), ModelAPI_AttributeBoolean::typeId());
}

//=================================================================================================
void BuildPlugin_Polyline::execute()
{
  // Get closed flag value
  bool isClosed = boolean(CLOSED_ID())->value();

  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());

  // Collect points.
  ListOfShape aPoints;
  std::set<GeomShapePtr> aContexts;
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
		GeomShapePtr aShape = aSelection->value();

    ResultPtr aContext = aSelection->context();
    aContexts.insert(aContext->shape());

    if (!aShape.get()) {
      aShape = aContext->shape();
		}

    aPoints.push_back(aShape);
	}

  // Prepare a list of edges
  ListOfShape anEdges;

  ListOfShape::const_iterator aPointsIt = aPoints.begin();
  for (; aPointsIt != std::prev(aPoints.end()); ++aPointsIt) {
    std::shared_ptr<GeomAPI_Pnt> aPnt = GeomAlgoAPI_PointBuilder::point(*aPointsIt);
    std::shared_ptr<GeomAPI_Pnt> aPntNext = GeomAlgoAPI_PointBuilder::point(*std::next(aPointsIt));
    anEdges.push_back(GeomAlgoAPI_EdgeBuilder::line(aPnt, aPntNext));
  }

  if (isClosed) {
    std::shared_ptr<GeomAPI_Pnt> aLastPnt = GeomAlgoAPI_PointBuilder::point(aPoints.back());
    std::shared_ptr<GeomAPI_Pnt> aFirstPnt = GeomAlgoAPI_PointBuilder::point(aPoints.front());
    anEdges.push_back(GeomAlgoAPI_EdgeBuilder::line(aLastPnt, aFirstPnt));
  }

  // Create wire from edges
  GeomShapePtr aWire = GeomAlgoAPI_WireBuilder::wire(anEdges);
  if (!aWire.get()) {
    setError("Error: Result polyline is empty.");
    return;
  }

  // Check the wire.
  if (GeomAlgoAPI_WireBuilder::isSelfIntersected(aWire)) {
    setError("Error: Result polyline has self-intersections.");
    return;
  }

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data());
  std::set<GeomShapePtr>::const_iterator aContextIt = aContexts.begin();
  for (; aContextIt != aContexts.end(); aContextIt++) {
    aResultBody->storeModified(*aContextIt, aWire, aContextIt == aContexts.begin());
  }

  aPointsIt = aPoints.cbegin();
  GeomAPI_ShapeExplorer anExp(aWire, GeomAPI_Shape::EDGE);
  for (; anExp.more() && aPointsIt != aPoints.cend(); anExp.next(), ++aPointsIt) {
    GeomShapePtr aPoint = *aPointsIt;
    GeomShapePtr anEdge = anExp.current();
    aResultBody->generated(aPoint, anEdge);
  }

  if (!isClosed) {
    ListOfShape aResPoints;
    anExp.init(aWire, GeomAPI_Shape::VERTEX);
    for (; anExp.more(); anExp.next()) {
      aResPoints.push_back(anExp.current());
    }

    aResultBody->generated(aResPoints.front(), "FirstVertex");
    aResultBody->generated(aResPoints.back(), "LastVertex");
  }

  setResult(aResultBody);
}
