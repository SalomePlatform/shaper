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

#include "FiltersPlugin_ContinuousFaces.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Wire.h>

#include <GeomAlgoAPI_ShapeTools.h>

#include <map>
#include <math.h>
#include <iostream>

typedef std::map<GeomShapePtr, SetOfShapes, GeomAPI_Shape::Comparator> MapShapeAndAncestors;

//=================================================================================================
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

//=================================================================================================
// Find all continuous faces for the given.
static void cacheContinuousFace(const GeomShapePtr theFace,
                                const MapShapeAndAncestors& theEdgeToFaces,
                                SetOfShapes& theCache,
                                const double & theAngle)
{

  MapShapeAndAncestors::const_iterator aFound;
  GeomAPI_ShapeExplorer aEExp(theFace, GeomAPI_Shape::EDGE);
  for (; aEExp.more(); aEExp.next()){
    aFound = theEdgeToFaces.find(aEExp.current());
    if (aFound == theEdgeToFaces.end())
      continue;

    GeomEdgePtr anEdge;
    anEdge = GeomEdgePtr(new GeomAPI_Edge(aEExp.current()));

    for (SetOfShapes::const_iterator aFIt = aFound->second.begin();
       aFIt != aFound->second.end(); ++aFIt) {
      std::string anError = "";
      if (theCache.find(*aFIt) == theCache.end()) {
       GeomPointPtr aPoint = anEdge->middlePoint();
       if (GeomAlgoAPI_ShapeTools::isContinuousFaces(theFace,
                                                     *aFIt,
                                                     aPoint,
                                                     theAngle,
                                                     anError)) {
          theCache.insert(*aFIt);
          cacheContinuousFace(*aFIt, theEdgeToFaces, theCache, theAngle);
        }
      }
    }
  }
}

//=================================================================================================
static void cacheContinuousFaces(const GeomShapePtr theTopLevelShape,
                                 const SetOfShapes& theFaces,
                                 SetOfShapes& theCache,
                                 const double & theAngle)
{
  if (!theTopLevelShape || theFaces.empty())
    return;

  MapShapeAndAncestors anEdgesToFaces;
  mapEdgesAndFaces(theTopLevelShape, anEdgesToFaces);

  for (SetOfShapes::const_iterator aFIt = theFaces.begin();
       aFIt != theFaces.end(); ++aFIt) {
    // keep the original face
    theCache.insert(*aFIt);
    // cache continuous face
    cacheContinuousFace(*aFIt, anEdgesToFaces, theCache,theAngle);
  }
}

//=================================================================================================
static bool updateFaces(const AttributeSelectionListPtr& theList,
                        SetOfShapes& theFaces)
{
  bool aNewCache = false;
  if ((int)theFaces.size() != theList->size()) {
    aNewCache = true;
  } else {
    for (int i = 0; i < theList->size(); i++) {
      AttributeSelectionPtr aCurAttr = theList->value(i);
      GeomShapePtr aFace = aCurAttr->value();
      if (theFaces.empty() || theFaces.find(aFace) == theFaces.end()) {
        aNewCache = true;
        break;
      }
    }
  }
  if (aNewCache) {
    theFaces.clear();
    for (int i = 0; i < theList->size(); i++) {
      AttributeSelectionPtr aCurAttr = theList->value(i);
      GeomShapePtr aFace = aCurAttr->value();
      theFaces.insert(aFace);
    }
  }
  return aNewCache;
}

//=================================================================================================
bool FiltersPlugin_ContinuousFaces::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::FACE;
}

//=================================================================================================
bool FiltersPlugin_ContinuousFaces::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                         const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr aAttr = theArgs.argument("faces");
  AttributeSelectionListPtr aList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  if (!aList.get())
    return false;

  AttributePtr anAttr = theArgs.argument("value");
  AttributeDoublePtr aValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr);
  if (!aValue.get()|| !anAttr->isInitialized())
    return false;
  double anAngle= aValue->value();

  bool aNewCache = updateFaces(aList,
                               const_cast<FiltersPlugin_ContinuousFaces*>(this)->myFaces);

  if (aNewCache || fabs(myAngle - anAngle) > 1e-10 ) {
    const_cast<FiltersPlugin_ContinuousFaces*>(this)->myAngle = anAngle;
    const_cast<FiltersPlugin_ContinuousFaces*>(this)->myCachedShapes.clear();
  }

  if (myCachedShapes.empty()) {
    for (int i = 0; i < aList->size(); i++)
    {
      ResultBodyPtr aBaseResult = ModelAPI_Tools::bodyOwner(aList->value(i)->context(), true);
      if (!aBaseResult.get()) {
        aBaseResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aList->value(i)->context());
        if (!aBaseResult.get())
          return false;
      }
      cacheContinuousFaces(aBaseResult->shape(),
                          const_cast<FiltersPlugin_ContinuousFaces*>(this)->myFaces,
                          const_cast<FiltersPlugin_ContinuousFaces*>(this)->myCachedShapes,anAngle);
    }
  }
  return myCachedShapes.find(theShape) != myCachedShapes.end();
}

//=================================================================================================
std::string FiltersPlugin_ContinuousFaces::xmlRepresentation() const
{
  return xmlFromFile("filter-ContinuousFaces.xml");
}

//=================================================================================================
void FiltersPlugin_ContinuousFaces::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("value", ModelAPI_AttributeDouble::typeId());
  theArguments.initAttribute("faces", ModelAPI_AttributeSelectionList::typeId());
}
