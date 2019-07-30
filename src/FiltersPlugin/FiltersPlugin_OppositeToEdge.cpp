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

#include "FiltersPlugin_OppositeToEdge.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Wire.h>
#include <GeomAPI_WireExplorer.h>

#include <map>

typedef std::map<GeomShapePtr, SetOfShapes, GeomAPI_Shape::Comparator> MapShapeAndAncestors;

static void mapEdgesAndFaces(const GeomShapePtr theShape, MapShapeAndAncestors& theMap)
{
  GeomAPI_ShapeExplorer aFExp(theShape, GeomAPI_Shape::FACE);
  for (; aFExp.more(); aFExp.next()) {
    GeomShapePtr aFace = aFExp.current();
    GeomAPI_ShapeExplorer aEExp(aFace, GeomAPI_Shape::EDGE);
    for (; aEExp.more(); aEExp.next())
      theMap[aEExp.current()].insert(aFace);
  }
}

// Return edge in the quadratic face opposite to the given one.
// If the face is not quadratic, returns empty shape.
static GeomShapePtr oppositeEdgeInQuadFace(const GeomShapePtr theEdge,
                                           const GeomShapePtr theFace)
{
  static int THE_QUAD = 4;

  int aNbEdges = 0;
  int anOriginalEdgeIndex = -THE_QUAD;
  GeomShapePtr anOppositeEdge;
  GeomAPI_ShapeExplorer aWExp(theFace, GeomAPI_Shape::WIRE);
  GeomWirePtr aWire = aWExp.current()->wire();
  aWExp.next();
  if (aWExp.more()) {
    // face with a hole is not a quadrangle
    return anOppositeEdge;
  }

  GeomAPI_WireExplorer anExp(aWire);
  while (anExp.more()) {
    if (anExp.current()->isSame(theEdge))
      anOriginalEdgeIndex = aNbEdges;
    else if (aNbEdges == anOriginalEdgeIndex + THE_QUAD / 2) {
      if (anOriginalEdgeIndex < THE_QUAD)
        anOppositeEdge = anExp.current();
      if (aNbEdges >= THE_QUAD)
        break;
    }

    ++aNbEdges;
    anExp.next();
    if (!anExp.more()) {
      if (aNbEdges != THE_QUAD) {
        // not quad face
        anOppositeEdge = GeomShapePtr();
        break;
      }
      if (!anOppositeEdge)
        anExp.init(aWire);
    }
  }
  return anOppositeEdge;
}

// Find all opposite edges for the given.
static void cacheOppositeEdge(const GeomShapePtr theEdge,
                              const MapShapeAndAncestors& theEdgeToFaces,
                              SetOfShapes& theCache)
{
  MapShapeAndAncestors::const_iterator aFound = theEdgeToFaces.find(theEdge);
  if (aFound == theEdgeToFaces.end())
    return;

  for (SetOfShapes::const_iterator aFIt = aFound->second.begin();
       aFIt != aFound->second.end(); ++aFIt) {
    GeomShapePtr anOpposite = oppositeEdgeInQuadFace(theEdge, *aFIt);
    if (anOpposite && theCache.find(anOpposite) == theCache.end()) {
      theCache.insert(anOpposite);
      cacheOppositeEdge(anOpposite, theEdgeToFaces, theCache);
    }
  }
}

static void cacheOppositeEdges(const GeomShapePtr theTopLevelShape,
                               const GeomShapePtr theEdge,
                               SetOfShapes& theCache)
{
  if (!theTopLevelShape || !theEdge)
    return;

  MapShapeAndAncestors anEdgesToFaces;
  mapEdgesAndFaces(theTopLevelShape, anEdgesToFaces);

  // keep the original edge
  theCache.insert(theEdge);
  // cache opposite edges
  cacheOppositeEdge(theEdge, anEdgesToFaces, theCache);
}


bool FiltersPlugin_OppositeToEdge::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::EDGE;
}

bool FiltersPlugin_OppositeToEdge::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                        const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr aAttr = theArgs.argument("OppositeToEdge");
  AttributeSelectionPtr aList = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(aAttr);
  if (!aList.get())
    return false;
  GeomShapePtr anEdge = aList->value();
  if (!myOriginalEdge || !myOriginalEdge->isSame(anEdge)) {
    // new edge is selected, need to update the cache
    const_cast<FiltersPlugin_OppositeToEdge*>(this)->myOriginalEdge = anEdge;
    const_cast<FiltersPlugin_OppositeToEdge*>(this)->myCachedShapes.clear();
  }

  if (myCachedShapes.empty()) {
    ResultBodyPtr aBaseResult = ModelAPI_Tools::bodyOwner(aList->context(), true);
    if (!aBaseResult.get()) {
      aBaseResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aList->context());
      if (!aBaseResult.get())
        return false;
    }

    cacheOppositeEdges(aBaseResult->shape(), anEdge,
        const_cast<FiltersPlugin_OppositeToEdge*>(this)->myCachedShapes);
  }

  return myCachedShapes.find(theShape) != myCachedShapes.end();
}

std::string FiltersPlugin_OppositeToEdge::xmlRepresentation() const
{
  return xmlFromFile("filter-OppositeToEdge.xml");
}

void FiltersPlugin_OppositeToEdge::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("OppositeToEdge", ModelAPI_AttributeSelection::typeId());
}
