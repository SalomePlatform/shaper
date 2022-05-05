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

#include "ModelGeomAlgo_Point2D.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>

#include <ModelGeomAlgo_Shape.h>

#include <GeomAPI_ShapeIterator.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_ShapeExplorer.h>

//#define DEBUG_POINT_INSIDE_SHAPE
#ifdef DEBUG_POINT_INSIDE_SHAPE
#include <iostream>
#endif

#ifdef WIN32
#pragma warning(disable : 4996) // for sprintf
#endif

std::shared_ptr<GeomDataAPI_Point2D> ModelGeomAlgo_Point2D::getPointOfRefAttr(
                                                  ModelAPI_Feature* theFeature,
                                                  const std::string& theAttribute,
                                                  const std::string& theObjectFeatureKind,
                                                  const std::string& theObjectFeatureAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  /// essential check as it is called in openGl thread
  if (!theFeature || !theFeature->data().get() || !theFeature->data()->isValid())
    return std::shared_ptr<GeomDataAPI_Point2D>();

  FeaturePtr aFeature;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theFeature->data()->attribute(theAttribute));
  if(anAttr.get() && anAttr->isInitialized()) {
    aFeature = ModelAPI_Feature::feature(anAttr->object());
    if (aFeature.get()) {
      bool aFeatureOfObjectKind = !theObjectFeatureKind.empty() &&
                                  !theObjectFeatureAttribute.empty() &&
                                  aFeature->getKind() == theObjectFeatureKind;
      if(aFeatureOfObjectKind)
          aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                  aFeature->data()->attribute(theObjectFeatureAttribute));
      else if (anAttr->attr())
        aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
    }
  }
  return aPointAttr;
}

void ModelGeomAlgo_Point2D::getPointsOfReference(
                            const std::shared_ptr<ModelAPI_Object>& theObject,
                            const std::string& theReferenceFeatureKind,
                            std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                            const std::string& theObjectFeatureKind,
                            const std::string& theObjectFeatureAttribute,
                            const bool isSkipFeatureAttributes)
{
  // find by feature
  FeaturePtr aSourceFeature = ModelAPI_Feature::feature(theObject);

  const std::set<AttributePtr>& aRefsList = theObject->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
    FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aRefFeature->getKind() == theReferenceFeatureKind) {
      std::list<AttributePtr> anAttributes =
                        aRefFeature->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
      std::list<AttributePtr>::iterator anIter = anAttributes.begin(), aLast = anAttributes.end();
      bool isSkippedAttribute = false;
      if (isSkipFeatureAttributes) {
        for(anIter = anAttributes.begin(); anIter != aLast && !isSkippedAttribute; anIter++) {
          AttributeRefAttrPtr aRefAttribute =
            std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
          if (aRefAttribute.get() && !aRefAttribute->isObject()) {
            std::shared_ptr<GeomDataAPI_Point2D> aPointAttr =
                            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aRefAttribute->attr());
            FeaturePtr anAttributeFeature = ModelAPI_Feature::feature(aPointAttr->owner());
            isSkippedAttribute = aSourceFeature == anAttributeFeature;
          }
        }
      }
      if (isSkippedAttribute)
        continue;

      // it searches the first point of AttributeRefAtt
      std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;
      for(anIter = anAttributes.begin(); anIter != aLast && !aPointAttr.get(); anIter++) {
        AttributeRefAttrPtr aRefAttribute =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
        if (aRefAttribute.get()) {
          aPointAttr = getPointOfRefAttr(aRefFeature.get(), aRefAttribute->id(),
                        theObjectFeatureKind, theObjectFeatureAttribute);
        }
      }
      if (aPointAttr.get()) {
        theAttributes.insert(aPointAttr);
      }
    }
  }
  // find by results
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (aFeature.get()) {
    const std::list<std::shared_ptr<ModelAPI_Result> > aResults = aFeature->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
    for (; aRIter != aResults.cend(); aRIter++) {
      ResultPtr aResult = *aRIter;
      getPointsOfReference(aResult, theReferenceFeatureKind, theAttributes, theObjectFeatureKind,
                            theObjectFeatureAttribute);
    }
  }
}

void appendPoint(const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                   const std::shared_ptr<ModelAPI_Result>& theResult,
                   ModelGeomAlgo_Point2D::PointToRefsMap& thePointToAttributeOrObject)
{
  bool aPointFound = false;
  FeaturePtr aPointFeature = ModelAPI_Feature::feature(theResult);
  // check if the given point is already in the container in attribute list
  for (ModelGeomAlgo_Point2D::PointToRefsMap::const_iterator
                  anIt = thePointToAttributeOrObject.begin();
                  anIt != thePointToAttributeOrObject.end() && !aPointFound; anIt++) {
    std::shared_ptr<GeomAPI_Pnt> aPoint = anIt->first;
    if (aPoint->isEqual(thePoint)) {
      std::list<std::shared_ptr<GeomDataAPI_Point2D> > anAttributes = anIt->second.first;
      for (std::list<AttributePoint2DPtr>::const_iterator anAttrIt = anAttributes.begin();
          anAttrIt != anAttributes.end() && !aPointFound; anAttrIt++) {
        AttributePtr anAttribute = *anAttrIt;
        aPointFound = ModelAPI_Feature::feature(anAttribute->owner()) == aPointFeature;
      }
    }
  }

  if (!aPointFound) {
    if (thePointToAttributeOrObject.find(thePoint) != thePointToAttributeOrObject.end())
      thePointToAttributeOrObject.at(thePoint).second.push_back(theResult);
    else {
      std::list<std::shared_ptr<GeomDataAPI_Point2D> > anAttributes;
      std::list<std::shared_ptr<ModelAPI_Object> > anObjects;
      anObjects.push_back(theResult);
      thePointToAttributeOrObject[thePoint] = std::make_pair(anAttributes, anObjects);
    }
#ifdef DEBUG_POINT_INSIDE_SHAPE
    std::cout << "["<< thePoint->x() << ", " << thePoint->y() << "," << thePoint->z() << "]";
#endif
  }
}

void appendShapePoints(const GeomShapePtr& theShape,
                       const std::shared_ptr<ModelAPI_Result>& theResult,
                       ModelGeomAlgo_Point2D::PointToRefsMap& thePointToAttributeOrObject)
{
  if (!theShape.get())
    return;

  switch (theShape->shapeType()) {
    case GeomAPI_Shape::VERTEX: {
      std::shared_ptr<GeomAPI_Vertex> aVertex =
        std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(theShape));
      std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
      appendPoint(aPnt, theResult, thePointToAttributeOrObject);
    }
    break;
    case GeomAPI_Shape::EDGE: {
      std::shared_ptr<GeomAPI_Edge> anEdge =
        std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(theShape));
      appendPoint(anEdge->firstPoint(), theResult, thePointToAttributeOrObject);
      appendPoint(anEdge->lastPoint(), theResult, thePointToAttributeOrObject);
    }
    break;
    case GeomAPI_Shape::COMPOUND: {
      for(GeomAPI_ShapeIterator anIt(theShape); anIt.more(); anIt.next()) {
        appendShapePoints(anIt.current(), theResult, thePointToAttributeOrObject);
      }
    }
    break;
    default: break;
  }
}

void ModelGeomAlgo_Point2D::getPointsIntersectedShape(
                        const std::shared_ptr<ModelAPI_Feature>& theBaseFeature,
                        const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
                        PointToRefsMap& thePointToAttributeOrObject)
{
#ifdef DEBUG_POINT_INSIDE_SHAPE
  std::cout << "ModelGeomAlgo_Point2D::getPointsIntersectedShape" << std::endl;
#endif
  GeomShapePtr aFeatureShape;
  {
    std::set<ResultPtr> anEdgeShapes;
    ModelGeomAlgo_Shape::shapesOfType(theBaseFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
    if (anEdgeShapes.empty())
      return;
    aFeatureShape = (*anEdgeShapes.begin())->shape();
  }

  std::list<std::shared_ptr<ModelAPI_Feature> >::const_iterator anIt = theFeatures.begin(),
                                                                aLast = theFeatures.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (aFeature.get() == theBaseFeature.get())
      continue;
    if (aFeature.get()) {
      std::set<ResultPtr> anEdgeShapes;
      ModelGeomAlgo_Shape::shapesOfType(aFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
      if (anEdgeShapes.empty())
        ModelGeomAlgo_Shape::shapesOfType(aFeature, GeomAPI_Shape::VERTEX, anEdgeShapes);

      if (anEdgeShapes.empty())
        continue;
      ResultPtr aResult = *anEdgeShapes.begin();
      GeomShapePtr aShape = aResult->shape();

      GeomShapePtr aShapeOfIntersection = aFeatureShape->intersect(aShape);
#ifdef DEBUG_POINT_INSIDE_SHAPE
      int aPrevSize = thePointToAttributeOrObject.size();
#endif
      appendShapePoints(aShapeOfIntersection, aResult, thePointToAttributeOrObject);
#ifdef DEBUG_POINT_INSIDE_SHAPE
      if (aPrevSize != thePointToAttributeOrObject.size())
        std::cout << " <- appendShapePoints"
                  << thePointToAttributeOrObject.size() - aPrevSize << std::endl;
#endif
    }
  }
}

std::list<std::shared_ptr<GeomAPI_Pnt> > ModelGeomAlgo_Point2D::getSetOfPntIntersectedShape(
                  const std::shared_ptr<ModelAPI_Feature>& theBaseFeature,
                  const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures)
{
  std::list<std::shared_ptr<GeomAPI_Pnt> > aPoints;

  PointToRefsMap aRefsMap;
  getPointsIntersectedShape(theBaseFeature, theFeatures, aRefsMap);

  for (PointToRefsMap::const_iterator aPointIt = aRefsMap.begin();
       aPointIt != aRefsMap.end(); aPointIt++)
    aPoints.push_back(aPointIt->first);

  return aPoints;
}

void ModelGeomAlgo_Point2D::getPointsInsideShape(
                        const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                        const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                        const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                        const std::shared_ptr<GeomAPI_Dir>& theDirX,
                        const std::shared_ptr<GeomAPI_Dir>& theDirY,
                        PointToRefsMap& thePointToAttributeOrObject)
{
#ifdef DEBUG_POINT_INSIDE_SHAPE
  std::cout << "ModelGeomAlgo_Point2D::getPointsInsideShape:" << std::endl;
#endif
  std::set<std::shared_ptr<GeomDataAPI_Point2D> >::const_iterator anIt = theAttributes.begin(),
                                                          aLast = theAttributes.end();
  for (; anIt != aLast; anIt++) {
    std::shared_ptr<GeomDataAPI_Point2D> anAttribute = *anIt;
    std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = anAttribute->pnt();
    std::shared_ptr<GeomAPI_Pnt> aPoint = aPnt2d->to3D(theOrigin, theDirX, theDirY);
    std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
    if (isInnerPointOnEdge(theBaseShape, aPoint, aProjectedPoint)) {
      if (thePointToAttributeOrObject.find(aProjectedPoint) != thePointToAttributeOrObject.end())
        thePointToAttributeOrObject.at(aProjectedPoint).first.push_back(anAttribute);
      else {
        std::list<std::shared_ptr<GeomDataAPI_Point2D> > anAttributes;
        std::list<std::shared_ptr<ModelAPI_Object> > anObjects;
        anAttributes.push_back(anAttribute);
        thePointToAttributeOrObject[aProjectedPoint] = std::make_pair(anAttributes, anObjects);
      }
#ifdef DEBUG_POINT_INSIDE_SHAPE
      std::cout << "  " << anAttribute->owner()->data()->name() << ": " << anAttribute->id()
                << "[" << aPoint->x() << ", " << aPoint->y() << ", " << aPoint->z() << "]"
                << std::endl;
#endif
    }
    else {
#ifdef DEBUG_POINT_INSIDE_SHAPE
      std::cout << "  " << anAttribute->owner()->data()->name() << ": " << anAttribute->id()
                << "OUT of shape" << std::endl;
#endif
    }
  }
}

void ModelGeomAlgo_Point2D::getPointsInsideShape_p(
                            const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                            const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                            const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                            const std::shared_ptr<GeomAPI_Dir>& theDirX,
                            const std::shared_ptr<GeomAPI_Dir>& theDirY,
                            std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                            std::map<std::shared_ptr<GeomDataAPI_Point2D>,
                                     std::shared_ptr<GeomAPI_Pnt> >& theAttributeToPoint)
{
  std::set<std::shared_ptr<GeomDataAPI_Point2D> >::const_iterator anIt = theAttributes.begin(),
                                                          aLast = theAttributes.end();
  for (; anIt != aLast; anIt++) {
    std::shared_ptr<GeomDataAPI_Point2D> anAttribute = *anIt;
    std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = anAttribute->pnt();
    std::shared_ptr<GeomAPI_Pnt> aPoint = aPnt2d->to3D(theOrigin, theDirX, theDirY);
    std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
    if (isInnerPointOnEdge(theBaseShape, aPoint, aProjectedPoint)) {
      thePoints.push_back(aProjectedPoint);
      theAttributeToPoint[anAttribute] = aProjectedPoint;
    }
  }
}

bool ModelGeomAlgo_Point2D::isPointOnEdge(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                     const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                     std::shared_ptr<GeomAPI_Pnt>& theProjectedPoint)
{
  bool isInside = false;
  if (theBaseShape->shapeType() == GeomAPI_Shape::EDGE) {
    GeomCurvePtr aCurve(new GeomAPI_Curve(theBaseShape->edge()));
    theProjectedPoint = aCurve->project(thePoint);
    if (theProjectedPoint.get()) {
      std::shared_ptr<GeomAPI_Vertex> aVertexShape(new GeomAPI_Vertex(theProjectedPoint->x(),
                                                theProjectedPoint->y(), theProjectedPoint->z()));
      isInside = GeomAlgoAPI_ShapeTools::isSubShapeInsideShape(aVertexShape, theBaseShape);
    }
  }
  return isInside;
}


bool ModelGeomAlgo_Point2D::isInnerPointOnEdge(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                     const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                     std::shared_ptr<GeomAPI_Pnt>& theProjectedPoint)
{
  bool isInside = isPointOnEdge(theBaseShape, thePoint, theProjectedPoint);
  if (isInside) {
    std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(theBaseShape));
    if (!anEdge->isClosed()) {
      // check the point is not on the boundary
      GeomVertexPtr aVertex(new GeomAPI_Vertex(theProjectedPoint->x(),
          theProjectedPoint->y(), theProjectedPoint->z()));
      GeomAPI_ShapeExplorer anExp(anEdge, GeomAPI_Shape::VERTEX);
      for (; anExp.more(); anExp.next()) {
        GeomVertexPtr aCurV = anExp.current()->vertex();
        isInside = !GeomAlgoAPI_ShapeTools::isSubShapeInsideShape(aVertex, aCurV);
      }
    }
  }
  return isInside;
}

std::string doubleToString(double theValue)
{
  std::string aValueStr;
  char aBuf[50];
  sprintf(aBuf, "%g", theValue);
  aValueStr = std::string(aBuf);
  return aValueStr;
}

#ifdef _DEBUG
std::string ModelGeomAlgo_Point2D::getPontAttributesInfo(
                    const std::shared_ptr<ModelAPI_Feature>& theFeature,
                    const std::set<std::shared_ptr<ModelAPI_Attribute> >& theAttributesOnly)
{
  std::string anInfo;

  std::list<AttributePtr> anAttrs = theFeature->data()->attributes(
                                                          GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr>::const_iterator anIt = anAttrs.begin(), aLast = anAttrs.end();

  for(; anIt != aLast; anIt++) {
    AttributePtr anAttribute = *anIt;
    if (anAttribute.get() && (theAttributesOnly.empty() ||
        theAttributesOnly.find(anAttribute) != theAttributesOnly.end())) {
    if (!anInfo.empty()) {
      anInfo.append(", ");
      anInfo.append("\n");
    }
    anInfo.append("    " + getPointAttributeInfo(anAttribute));
    }
  }
  return anInfo;
}

std::string ModelGeomAlgo_Point2D::getPointAttributeInfo(
                              const std::shared_ptr<ModelAPI_Attribute>& theAttribute)
{
  std::string anInfo;
  std::string aValue = "not defined";
  std::string aType = theAttribute->attributeType();
  if (aType == GeomDataAPI_Point2D::typeId()) {
    std::shared_ptr<GeomDataAPI_Point2D> aPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
    if (aPoint.get() && aPoint->isInitialized()) {
      aValue = std::string("(" + doubleToString(aPoint->x()) + ", "+
                            doubleToString(aPoint->y()) + ")");
    }
  }
  anInfo.append(theAttribute->id() + ": " + aValue);

  return anInfo;
}
#endif
