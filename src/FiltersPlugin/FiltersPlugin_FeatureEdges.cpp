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

#include "FiltersPlugin_FeatureEdges.h"

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Wire.h>

#include <GeomAlgoAPI_ShapeTools.h>

#include <Precision.hxx>

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
static void cacheFeatureEdge(const GeomShapePtr theTopLevelShape,
                             SetOfShapes& theCache,
                             const double & theAngle)
{
  if (!theTopLevelShape)
    return;

  MapShapeAndAncestors anEdgesToFaces;
  mapEdgesAndFaces(theTopLevelShape, anEdgesToFaces);

  MapShapeAndAncestors::const_iterator aIt;
  for (aIt = anEdgesToFaces.begin(); aIt != anEdgesToFaces.end(); ++aIt) {
    GeomEdgePtr anEdge;
    anEdge = GeomEdgePtr(new GeomAPI_Edge(aIt->first));

    for (SetOfShapes::const_iterator aFIt = aIt->second.begin();
         aFIt != aIt->second.end(); ++aFIt) {
      SetOfShapes::const_iterator aFIt2 = aFIt;
      ++aFIt2;
      for (;aFIt2 != aIt->second.end(); ++aFIt2) {
        std::string anError;
        if (theCache.find(*aFIt) == theCache.end()) {
          if (theAngle < Precision::Confusion()
              || !GeomAlgoAPI_ShapeTools::isContinuousFaces(*aFIt,
                                                         *aFIt2,
                                                         anEdge->middlePoint(),
                                                         theAngle,
                                                         anError)) {
            if (anError.empty())
              theCache.insert(anEdge);
          }
        }
      }
    }
  }
}

//=================================================================================================
bool FiltersPlugin_FeatureEdges::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::EDGE;
}

//=================================================================================================
bool FiltersPlugin_FeatureEdges::isOk(const GeomShapePtr& theShape, const ResultPtr& theResult,
                                      const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr anAttr = theArgs.argument("value");
  AttributeDoublePtr aValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr);

  if (!aValue.get() || !anAttr->isInitialized())
    return false;
  double anAngle = aValue->value();

  // check base result
  ResultBodyPtr aBaseResult = ModelAPI_Tools::bodyOwner(theResult, true);
  if (!aBaseResult) {
    aBaseResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
    if (!aBaseResult.get()) {
      return false;
    }
  }
  if (fabs(myAngle - anAngle) > 1e-10
      || !myBaseShape
      || !myBaseShape->isSame(aBaseResult->shape())) {
    const_cast<FiltersPlugin_FeatureEdges*>(this)->myAngle = anAngle;
    const_cast<FiltersPlugin_FeatureEdges*>(this)->myBaseShape = aBaseResult->shape();
    const_cast<FiltersPlugin_FeatureEdges*>(this)->myCachedShapes.clear();
  }

  if (myCachedShapes.empty()) {

    cacheFeatureEdge(aBaseResult->shape(),
                     const_cast<FiltersPlugin_FeatureEdges*>(this)->myCachedShapes, anAngle);
  }

  return myCachedShapes.find(theShape) != myCachedShapes.end();
}

//=================================================================================================
std::string FiltersPlugin_FeatureEdges::xmlRepresentation() const
{
  return xmlFromFile("filter-FeatureEdges.xml");
}

//=================================================================================================
void FiltersPlugin_FeatureEdges::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("value", ModelAPI_AttributeDouble::typeId());
}
