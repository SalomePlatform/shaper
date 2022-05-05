// Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

#include "BuildPlugin_Filling.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_Filling.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <GeomAPI_Curve.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Wire.h>
#include <GeomAPI_WireExplorer.h>

#include <cmath>

struct FillingParameters
{
  std::string method;
  int minDegree;
  int maxDegree;
  int nbIter;
  double tol2D;
  double tol3D;
  bool isApprox;
};

static bool isReverseClosedCurve(const GeomEdgePtr& theEdge1,
                                 const GeomEdgePtr& theEdge2);
static bool isReverseOpenedCurve(const GeomEdgePtr& theEdge1,
                                 const GeomEdgePtr& theEdge2,
                                 const double theTolerance);
static void shiftStartPoint(GeomWirePtr& theWire,
                            const GeomEdgePtr& theRefEdge,
                            const double theTolerance);


//=================================================================================================
BuildPlugin_Filling::BuildPlugin_Filling()
{
}

//=================================================================================================
void BuildPlugin_Filling::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(ADVANCED_OPTIONS_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(METHOD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(MINIMAL_DEGREE_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(MAXIMAL_DEGREE_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(NUMBER_OF_ITERATIONS_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(TOLERANCE_2D_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(TOLERANCE_3D_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(APPROXIMATION_ID(), ModelAPI_AttributeBoolean::typeId());

  if (string(ADVANCED_OPTIONS_ID())->value().empty())
    restoreDefaultParameters();
}

//=================================================================================================
void BuildPlugin_Filling::execute()
{
  // get parameters of algorithm
  FillingParameters aParameters;
  aParameters.method = string(METHOD_ID())->value();
  aParameters.minDegree = integer(MINIMAL_DEGREE_ID())->value();
  aParameters.maxDegree = integer(MAXIMAL_DEGREE_ID())->value();
  aParameters.nbIter = integer(NUMBER_OF_ITERATIONS_ID())->value();
  aParameters.tol2D = real(TOLERANCE_2D_ID())->value();
  aParameters.tol3D = real(TOLERANCE_3D_ID())->value();
  aParameters.isApprox = boolean(APPROXIMATION_ID())->value();

  if (aParameters.minDegree > aParameters.maxDegree) {
    setError("Error: " + getKind() + " algorithm failed (max deg < min deg).");
    return;
  }

  std::shared_ptr<GeomAlgoAPI_Filling> aFilling(
      new GeomAlgoAPI_Filling(aParameters.minDegree, aParameters.maxDegree, aParameters.nbIter,
                              aParameters.tol2D, aParameters.tol3D));

  // get base objects list
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if (aSelectionList->size() <= 1) {
    setError("Not enough objects is selected");
    return;
  }

  // collect base shapes
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomEdgePtr anEdge = toEdge(aSelection->value(), aParameters.method);
    if (!anEdge)
      return;
    aFilling->add(anEdge);
  }
  myLastEdge = GeomEdgePtr();

  // build result
  aFilling->build(aParameters.isApprox);
  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aFilling, getKind(), anError)) {
    setError(anError);
    removeResults(0);
    return;
  }

  /// store result
  GeomShapePtr aCreatedFace = aFilling->shape();
  ResultBodyPtr aResultBody = document()->createBody(data());
  aResultBody->store(aCreatedFace);
  // store edges
  int anEdgeInd = 0;
  for(GeomAPI_ShapeExplorer anExp(aCreatedFace, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
    GeomShapePtr anEdge = anExp.current();
    aResultBody->generated(anEdge, "Edge_" + std::to_string((long long)anEdgeInd));
  }
  setResult(aResultBody, 0);
}

//=================================================================================================
void BuildPlugin_Filling::attributeChanged(const std::string& theID)
{
  if (theID == ADVANCED_OPTIONS_ID() && string(ADVANCED_OPTIONS_ID())->value().empty()) {
    // Advanced options flag just unchecked => restore default state of all parameters
    restoreDefaultParameters();
  }
}

//=================================================================================================
GeomEdgePtr BuildPlugin_Filling::toEdge(const GeomShapePtr& theShape, const std::string& theMethod)
{
  static const double TOLERANCE = 1.e-7;

  GeomEdgePtr anEdge;
  switch (theShape->shapeType()) {
  case GeomAPI_Shape::EDGE:
    anEdge = GeomEdgePtr(new GeomAPI_Edge(GeomAlgoAPI_Copy(theShape).shape()));
    break;
  case GeomAPI_Shape::WIRE: {
    GeomWirePtr aWire(new GeomAPI_Wire(theShape));
    if (myLastEdge && theMethod == Method::AUTO_CORRECT_ORIENTATION())
      shiftStartPoint(aWire, myLastEdge, TOLERANCE);
    anEdge = GeomAlgoAPI_ShapeTools::wireToEdge(aWire);
    break;
  }
  default:
    break;
  }

  if (!anEdge || anEdge->empty()) {
    static const std::string aFeatureError =
        "Error: incorrect type of input feature (edges/wire are supported only).";
    setError(aFeatureError);
    return anEdge;
  }

  // correct edge orientation according to filling method
  if (theMethod == Method::AUTO_CORRECT_ORIENTATION()) {
    // check the distance to previous edge boundaries, reverse edge if necessary
    bool isReverse = false;
    if (myLastEdge) {
      if (myLastEdge->firstPoint()->distance(myLastEdge->lastPoint()) < TOLERANCE &&
          anEdge->firstPoint()->distance(anEdge->lastPoint()) < TOLERANCE)
        isReverse = isReverseClosedCurve(myLastEdge, anEdge);
      else
        isReverse = isReverseOpenedCurve(myLastEdge, anEdge, TOLERANCE);
    }

    myLastEdge = anEdge;
    if (isReverse)
      anEdge->reverse();
  }
  else if (theMethod == Method::USE_CURVE_INFORMATION()) {
    // make all edges FORWARD to avoid reversing the curves by GeomAlgoAPI_Filling algorithm
    anEdge->setOrientation(GeomAPI_Shape::FORWARD);
  }
  return anEdge;
}

//=================================================================================================
void BuildPlugin_Filling::restoreDefaultParameters()
{
  string(METHOD_ID())->setValue(METHOD_DEFAULT());
  integer(MINIMAL_DEGREE_ID())->setValue(MINIMAL_DEGREE_DEFAULT());
  integer(MAXIMAL_DEGREE_ID())->setValue(MAXIMAL_DEGREE_DEFAULT());
  integer(NUMBER_OF_ITERATIONS_ID())->setValue(NUMBER_OF_ITERATIONS_DEFAULT());
  real(TOLERANCE_2D_ID())->setValue(TOLERANCE_2D_DEFAULT());
  real(TOLERANCE_3D_ID())->setValue(TOLERANCE_3D_DEFAULT());
  boolean(APPROXIMATION_ID())->setValue(APPROXIMATION_DEFAULT());
}


//============     Auxiliary functions     ========================================================

static std::pair<GeomPointPtr, GeomPointPtr> edgeBoundaries(const GeomEdgePtr& theEdge)
{
  GeomPointPtr aStart = theEdge->firstPoint();
  GeomPointPtr anEnd = theEdge->lastPoint();
  if (theEdge->orientation() == GeomAPI_Shape::REVERSED)
    std::swap(aStart, anEnd);
  return std::pair<GeomPointPtr, GeomPointPtr>(aStart, anEnd);
}

static void edgePoints(const GeomEdgePtr& theEdge, std::list<GeomPointPtr>& thePoints)
{
  GeomAPI_Curve aCurve(theEdge);
  static const int aNbSegments = 10;
  double aStart = aCurve.startParam();
  double aEnd = aCurve.endParam();
  for (int i = 0; i <= aNbSegments; ++i)
    thePoints.push_back(aCurve.getPoint(aStart * (1.0 - (double)i / aNbSegments) +
                                        aEnd * (double)i / aNbSegments ));
  if (theEdge->orientation() == GeomAPI_Shape::REVERSED)
    thePoints.reverse();
}

bool isReverseClosedCurve(const GeomEdgePtr& theEdge1,
                          const GeomEdgePtr& theEdge2)
{
  std::list<GeomPointPtr> anEdge1Points, anEdge2Points;
  edgePoints(theEdge1, anEdge1Points);
  edgePoints(theEdge2, anEdge2Points);

  double d1 = 0.0;
  double d2 = 0.0;
  std::list<GeomPointPtr>::const_iterator anIt1 = anEdge1Points.begin();
  std::list<GeomPointPtr>::const_iterator anIt2 = anEdge2Points.begin();
  std::list<GeomPointPtr>::const_reverse_iterator anIt2Rev = anEdge2Points.rbegin();
  for (; anIt1 != anEdge1Points.end(); ++anIt1, ++anIt2, ++anIt2Rev) {
    d1 += (*anIt1)->distance(*anIt2);
    d2 += (*anIt1)->distance(*anIt2Rev);
  }
  return d2 < d1;
}

bool isReverseOpenedCurve(const GeomEdgePtr& theEdge1,
                          const GeomEdgePtr& theEdge2,
                          const double theTolerance)
{
  std::pair<GeomPointPtr, GeomPointPtr> anEdge1Points = edgeBoundaries(theEdge1);
  std::pair<GeomPointPtr, GeomPointPtr> anEdge2Points = edgeBoundaries(theEdge2);
  double d1 = anEdge1Points.first->distance(anEdge2Points.first)
            + anEdge1Points.second->distance(anEdge2Points.second);
  double d2 = anEdge1Points.first->distance(anEdge2Points.second)
            + anEdge1Points.second->distance(anEdge2Points.first);
  if (fabs(d1 - d2) < theTolerance) {
    // undefined case => check distance to start point only
    d1 = anEdge1Points.first->distance(anEdge2Points.first);
    d2 = anEdge1Points.first->distance(anEdge2Points.second);
  }
  return d2 < d1;
}

void shiftStartPoint(GeomWirePtr& theWire, const GeomEdgePtr& theRefEdge, const double theTolerance)
{
  if (!theWire->isClosed()) {
    GeomVertexPtr aV1, aV2;
    GeomAlgoAPI_ShapeTools::findBounds(theWire, aV1, aV2);
    if (aV1->point()->distance(aV2->point()) > theTolerance)
      return;
  }

  // find closest vertex on the wire to the start point on the edge
  GeomPointPtr aFirstRefPnt = theRefEdge->firstPoint();
  ListOfShape aBegin, aEnd;
  double aMinDist = 1.e100;
  for (GeomAPI_WireExplorer anExp(theWire); anExp.more(); anExp.next()) {
    double aDist = anExp.currentVertex()->point()->distance(aFirstRefPnt);
    if (aDist < aMinDist) {
      aMinDist = aDist;
      aEnd.insert(aEnd.end(), aBegin.begin(), aBegin.end());
      aBegin.clear();
    }
    aBegin.push_back(anExp.current());
  }
  aBegin.insert(aBegin.end(), aEnd.begin(), aEnd.end());

  GeomShapePtr aShape = GeomAlgoAPI_WireBuilder::wire(aBegin);
  theWire.reset(new GeomAPI_Wire(aShape));
}
