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

#include "BuildPlugin_Interpolation.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <Events_InfoMessage.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_CurveBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeExplorer.h>

#include <algorithm>

//=================================================================================================
BuildPlugin_Interpolation::BuildPlugin_Interpolation()
{
}

//=================================================================================================
void BuildPlugin_Interpolation::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(CLOSED_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(REORDER_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(USE_TANGENTS_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(TANGENT_START_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TANGENT_END_ID(), ModelAPI_AttributeSelection::typeId());
}

//=================================================================================================
static GeomDirPtr selectionToDir(const AttributeSelectionPtr& theSelection)
{
  GeomDirPtr aDir;
  GeomEdgePtr anEdge;

  GeomShapePtr aShape = theSelection->value();
  if (!aShape && theSelection->context()) {
    aShape = theSelection->context()->shape();
  }

  if (aShape && aShape->isEdge()) {
    anEdge = GeomEdgePtr(new GeomAPI_Edge(aShape));
  }

  if (anEdge && anEdge->isLine()) {
    aDir = anEdge->line()->direction();
  }

  return aDir;
}

//=================================================================================================
void BuildPlugin_Interpolation::execute()
{
  // Get closed flag value
  bool isClosed = boolean(CLOSED_ID())->value();

  // Get reorder flag value
  bool isToReorder = boolean(REORDER_ID())->value();

  // Get use tangents flag value
  bool isToUseTangents = isClosed? false : (!string(USE_TANGENTS_ID())->value().empty());

  // Get tangent for start and end points
  GeomDirPtr aDirStart, aDirEnd;
  if (isToUseTangents) {
    aDirStart = selectionToDir(selection(TANGENT_START_ID()));
    aDirEnd = selectionToDir(selection(TANGENT_END_ID()));
  }

  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());

  // Collect points.
  std::list<GeomPointPtr> aPoints;
  std::set<GeomShapePtr> aContexts;
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);

    GeomShapePtr aContextShape = aSelection->context()->shape();
    aContexts.insert(aContextShape);

		GeomShapePtr aShape = aSelection->value();
    if (!aShape.get()) {
      aShape = aContextShape;
		}

    GeomPointPtr aPoint = GeomAlgoAPI_PointBuilder::point(aShape);
    aPoints.push_back(aPoint);
	}

  // Create curve from points
  GeomEdgePtr anEdge =
    GeomAlgoAPI_CurveBuilder::edge(aPoints, isClosed, isToReorder, aDirStart, aDirEnd);
  if (!anEdge.get()) {
    setError("Error: Result curve is empty.");
    return;
  }

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data());
  std::set<GeomShapePtr>::const_iterator aContextIt = aContexts.begin();
  for (; aContextIt != aContexts.end(); aContextIt++) {
    aResultBody->storeModified(*aContextIt, anEdge, aContextIt == aContexts.begin());
  }
  int aVertexIndex = 1;
  for (GeomAPI_ShapeExplorer anExp(anEdge, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
    std::string aVertexName = "Vertex_" + std::to_string((long long)aVertexIndex);
    aResultBody->generated(anExp.current(), aVertexName);
  }

  setResult(aResultBody);
}
