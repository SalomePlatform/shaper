// Copyright (C) 2014-2023  CEA, EDF
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

#include <GeomAPI_Shape.h>

#include "ModelGeomAlgo_Shape.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Circ.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Pnt.h>

#include <GeomAlgoAPI_ShapeTools.h>


#ifdef WIN32
#pragma warning(disable : 4996) // for sprintf
#endif

namespace ModelGeomAlgo_Shape
{
  void shapesOfType(const FeaturePtr& theFeature,
                    const GeomAPI_Shape::ShapeType& theType,
                    std::set<ResultPtr>& theShapeResults)
  {
    theShapeResults.clear();
    std::list<ResultPtr> aResults = theFeature->results();
    std::list<ResultPtr>::const_iterator aRIter = aResults.cbegin();
    for (; aRIter != aResults.cend(); aRIter++) {
      ResultPtr aResult = *aRIter;
      GeomShapePtr aShape = aResult->shape();
      if (aShape.get() && aShape->shapeType() == theType)
        theShapeResults.insert(aResult);
    }
  }

  // Check the point is within shape's bounding box
  static bool isPointWithinBB(const GeomPointPtr& thePoint,
                              const GeomShapePtr& theShape,
                              const double theTolerance)
  {
    double aXMin, aXMax, aYMin, aYMax, aZMin, aZMax;
    return theShape->computeSize(aXMin, aYMin, aZMin, aXMax, aYMax, aZMax) &&
           thePoint->x() >= aXMin - theTolerance && thePoint->x() <= aXMax + theTolerance &&
           thePoint->y() >= aYMin - theTolerance && thePoint->y() <= aYMax + theTolerance &&
           thePoint->z() >= aZMin - theTolerance && thePoint->z() <= aZMax + theTolerance;
  }

  // Select sub-shape of the given type, which contains the given point
  static std::list<GeomShapePtr> findSubShape(const GeomShapePtr& theShape,
                                              const GeomAPI_Shape::ShapeType& theType,
                                              const GeomPointPtr& thePoint,
                                              const double theTolerance)
  {
    std::list<GeomShapePtr> aFoundSubs;
    std::list<GeomShapePtr> aSubs = theShape->subShapes(theType);
    for (std::list<GeomShapePtr>::const_iterator aSubIt = aSubs.begin();
         aSubIt != aSubs.end(); ++aSubIt) {
      GeomPointPtr aMiddlePoint = (*aSubIt)->middlePoint();
      if (!aMiddlePoint)
        continue;

      double aDistance = aMiddlePoint->distance(thePoint);
      bool isFound = aDistance < theTolerance;
      // issue #19019: special workaround for faces, because if the face contains B-spline contour,
      // the middle point is calculated with respect to its poles, but not a curve itself.
      // Thus, in some operations (like BOP) the curve may have different number of poles
      // from time to time, as a result, the face parametric boundaries are floating
      // as well as the middle point.
      // The workaround is to find a distance from the picking point to the face, if the distance
      // between the picking point and the middle point on the face is small to some extend.
      static const double THE_THRESHOLD = 100.;
      if (!isFound && aDistance < THE_THRESHOLD * theTolerance && (*aSubIt)->isFace()) {
        GeomVertexPtr aVertex(new GeomAPI_Vertex(thePoint));
        aDistance = GeomAlgoAPI_ShapeTools::minimalDistance(aVertex, *aSubIt);
        isFound = aDistance < theTolerance;
      }
      if (isFound)
        aFoundSubs.push_back(*aSubIt);
    }
    return aFoundSubs;
  }

  // Find circular/elliptic edge, which center/focus coincide with the given point
  static GeomShapePtr findEdgeByCenter(const GeomShapePtr& theBaseShape,
                                       const GeomPointPtr& theCenter,
                                       const double theTolerance,
                                       int& theCenterType)
  {
    theCenterType = (int)ModelAPI_AttributeSelection::NOT_CENTER;
    std::list<GeomShapePtr> anEdges = theBaseShape->subShapes(GeomAPI_Shape::EDGE);
    for (std::list<GeomShapePtr>::const_iterator anIt = anEdges.begin();
         anIt != anEdges.end(); ++anIt) {
      GeomEdgePtr anEdge = (*anIt)->edge();
      if (!anEdge)
        continue;

      if (anEdge->isCircle() || anEdge->isArc()) {
        GeomCirclePtr aCircle = anEdge->circle();
        if (aCircle->center()->distance(theCenter) < theTolerance) {
          theCenterType = (int)ModelAPI_AttributeSelection::CIRCLE_CENTER;
          return *anIt;
        }
      }
      else if (anEdge->isEllipse()) {
        GeomEllipsePtr anEllipse = anEdge->ellipse();
        if (anEllipse->firstFocus()->distance(theCenter) < theTolerance)
          theCenterType = (int)ModelAPI_AttributeSelection::ELLIPSE_FIRST_FOCUS;
        else if (anEllipse->secondFocus()->distance(theCenter) < theTolerance)
          theCenterType = (int)ModelAPI_AttributeSelection::ELLIPSE_SECOND_FOCUS;

        if (theCenterType != (int)ModelAPI_AttributeSelection::NOT_CENTER)
          return *anIt;
      }
    }

    // not found
    return GeomShapePtr();
  }

  static void appendSubshapeOfResult(std::list<SubshapeOfResult>& theList,
      const ResultPtr& theResult,
      const GeomShapePtr& theSubshape,
      int theCenterType = (int)ModelAPI_AttributeSelection::NOT_CENTER)
  {
    SubshapeOfResult aSR;
    aSR.myResult = theResult;
    aSR.mySubshape = theSubshape;
    aSR.myCenterType = theCenterType;
    // compound subshapes from other compounds should be processed as whole results
    if (aSR.mySubshape && aSR.mySubshape->shapeType() <= GeomAPI_Shape::COMPSOLID) {
      if (theResult->shape()->isEqual(theSubshape))
        aSR.mySubshape = GeomShapePtr();
      else {
        ResultBodyPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
        for (int i = 0; aResult && i < aResult->numberOfSubs(); ++i) {
          ResultBodyPtr aSub = aResult->subResult(i);
          if (aSub->shape()->isEqual(theSubshape)) {
            aSR.myResult = aSub;
            aSR.mySubshape = GeomShapePtr();
            break;
          }
        }
      }
    }
    theList.push_back(aSR);
  }

  static void appendSubshapeOfResult(std::list<SubshapeOfResult>& theList,
      const ResultPtr& theResult,
      const std::list<GeomShapePtr>& theSubshape)
  {
    for (std::list<GeomShapePtr>::const_iterator anIt = theSubshape.begin();
         anIt != theSubshape.end(); ++anIt)
      appendSubshapeOfResult(theList, theResult, *anIt);
  }

  static bool findSubshapeInCompsolid(const ResultBodyPtr& theCompsolid,
                                      const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                                      const GeomAPI_Shape::ShapeType& theShapeType,
                                      const double theTolerance,
                                      std::list<SubshapeOfResult>& theSelected)
  {
    bool isSubshapeFound = false;
    int aNbSolids = theCompsolid->numberOfSubs();
    for (int i = 0; i < aNbSolids; ++i) {
      ResultPtr aSubResult = theCompsolid->subResult(i);

      // process subs of compsolid
      ResultBodyPtr aSubCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aSubResult);
      if (theShapeType != GeomAPI_Shape::COMPSOLID &&
          aSubCompSolid && aSubCompSolid->numberOfSubs() > 0) {
        isSubshapeFound = findSubshapeInCompsolid(aSubCompSolid,
            thePoint, theShapeType, theTolerance, theSelected);
      }
      else {
        GeomShapePtr aSubSolid = aSubResult->shape();
        if (aSubSolid && isPointWithinBB(thePoint, aSubSolid, theTolerance)) {
          std::list<GeomShapePtr> aSubshapes =
              findSubShape(aSubSolid, theShapeType, thePoint, theTolerance);
          if (!aSubshapes.empty()) {
            appendSubshapeOfResult(theSelected, aSubResult, aSubshapes);
            isSubshapeFound = true;
          }
        }
      }
    }
    return isSubshapeFound;
  }

  bool findSubshapeByPoint(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                           const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                           const GeomAPI_Shape::ShapeType& theShapeType,
                           std::list<SubshapeOfResult>& theSelected)
  {
    static const double TOLERANCE = 1.5e-6;

    theSelected.clear();

    const std::list<ResultPtr>& aResults = theFeature->results();
    for (std::list<ResultPtr>::const_iterator aResIt = aResults.begin();
         aResIt != aResults.end(); ++aResIt) {
      bool isSubshapeFound = false;
      GeomShapePtr aCurShape = (*aResIt)->shape();
      // first of all, check the point is within bounding box of the result
      if (!aCurShape || !isPointWithinBB(thePoint, aCurShape, TOLERANCE))
        continue;
      // now, process all sub-shapes of the given type and check their inner points,
      // but skip the case the selected type is COMPOUND and the shape is a list of sketch edges
      // (it will be processed later)
      std::shared_ptr<GeomAPI_PlanarEdges> aSketchEdges =
          std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aCurShape);
      if (theShapeType != GeomAPI_Shape::COMPOUND || !aSketchEdges) {
        ResultBodyPtr aCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aResIt);
        if (aCompSolid) {
          isSubshapeFound = findSubshapeInCompsolid(aCompSolid,
              thePoint, theShapeType, TOLERANCE, theSelected);
        }

        if (!isSubshapeFound) {
          std::list<GeomShapePtr> aSubshapes =
              findSubShape(aCurShape, theShapeType, thePoint, TOLERANCE);
          if (!aSubshapes.empty()) {
            appendSubshapeOfResult(theSelected, *aResIt, aSubshapes);
            isSubshapeFound = true;
          }
        }
      }
      if (isSubshapeFound)
        continue;

      // special case for ResultConstruction if the FACE is selected
      ResultConstructionPtr aResConstr =
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aResIt);
      if (aResConstr && theShapeType >= GeomAPI_Shape::FACE) {
        int aNbFaces = aResConstr->facesNum();
        for (int aFaceInd = 0; aFaceInd < aNbFaces; ++aFaceInd) {
          GeomFacePtr aCurFace = aResConstr->face(aFaceInd);
          // check the point is within bounding box of the face
          if (!isPointWithinBB(thePoint, aCurFace, TOLERANCE))
            continue;
          std::list<GeomShapePtr> aSubshapes =
              findSubShape(aCurFace, theShapeType, thePoint, TOLERANCE);
          if (!aSubshapes.empty()) {
            appendSubshapeOfResult(theSelected, *aResIt, aSubshapes);
            isSubshapeFound = true;
          }
        }
      }
      if (isSubshapeFound)
        continue;

      // next special case: the full sketch is selected
      // the selection type is a COMPOUND
      if (aSketchEdges && theShapeType == GeomAPI_Shape::COMPOUND &&
          aSketchEdges->middlePoint()->distance(thePoint) < TOLERANCE) {
        // select whole result
        appendSubshapeOfResult(theSelected, *aResIt, GeomShapePtr());
        continue;
      }

      // another special case: the center of circle or the focus of ellipse is selected;
      // return the corresponding edge and a status of the center
      if (theShapeType == GeomAPI_Shape::VERTEX) {
        int aCenterType;
        GeomShapePtr aSubshape = findEdgeByCenter(aCurShape, thePoint, TOLERANCE, aCenterType);
        if (aSubshape) {
          appendSubshapeOfResult(theSelected, *aResIt, aSubshape, aCenterType);
          continue;
        }
      }
    }

    bool processSketch = theSelected.empty() || (theSelected.size() == 1 &&
        theSelected.front().myCenterType != (int)ModelAPI_AttributeSelection::NOT_CENTER);
    // one more special case: the selected entity is a separated sketch point
    // or an auxiliary sketch edge; they are not included into the sketch result;
    // thus, it is necessary to pass through the sketch sub-features and find selected.
    if (processSketch && !aResults.empty() &&
       (theShapeType == GeomAPI_Shape::VERTEX || theShapeType == GeomAPI_Shape::EDGE)) {
      CompositeFeaturePtr aCF = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
      std::shared_ptr<GeomAPI_PlanarEdges> aSketchEdges =
          std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aResults.front()->shape());

      if (aSketchEdges && aCF) {
        int aNbSubs = aCF->numberOfSubs();
        for (int aSubInd = 0; aSubInd < aNbSubs; ++aSubInd) {
          FeaturePtr aSub = aCF->subFeature(aSubInd);
          const std::list<ResultPtr>& aSubResults = aSub->results();
          for (std::list<ResultPtr>::const_iterator aSRIt = aSubResults.begin();
               aSRIt != aSubResults.end(); ++aSRIt) {
            GeomShapePtr aCurShape = (*aSRIt)->shape();
            std::list<GeomShapePtr> aSubshapes =
                findSubShape(aCurShape, theShapeType, thePoint, TOLERANCE);
            if (!aSubshapes.empty()) {
              appendSubshapeOfResult(theSelected, aResults.front(), aSubshapes);
              break;
            }
          }
        }
      }
    }

    return !theSelected.empty();
  }
} // namespace ModelGeomAlgo_Shape
