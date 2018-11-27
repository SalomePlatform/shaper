// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
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

#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Wire.h>

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
  if (aSelectionList->size() <= 1)
    return;

  // collect base shapes
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomEdgePtr anEdge = toEdge(aSelection->value(), aParameters.method);
    if (!anEdge) {
      myLastEdgeStartPoint = GeomPointPtr();
      myLastEdgeEndPoint = GeomPointPtr();
      return;
    }
    aFilling->add(anEdge);
  }
  myLastEdgeStartPoint = GeomPointPtr();
  myLastEdgeEndPoint = GeomPointPtr();

  // build result
  aFilling->build(aParameters.isApprox);
  if (isAlgorithmFailed(aFilling)) {
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

bool BuildPlugin_Filling::isAlgorithmFailed(
    const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgorithm)
{
  if (!theAlgorithm->isDone()) {
    static const std::string aFeatureError = "Error: filling algorithm failed.";
    std::string anAlgoError = theAlgorithm->getError();
    if (anAlgoError.empty())
      anAlgoError = aFeatureError;
    else
      anAlgoError = aFeatureError + " " + anAlgoError;
    setError(anAlgoError);
    return true;
  }
  if (theAlgorithm->shape()->isNull()) {
    static const std::string aShapeError = "Error: Resulting shape of filling is Null.";
    setError(aShapeError);
    return true;
  }
  if (!theAlgorithm->isValid()) {
    std::string aFeatureError = "Error: Resulting shape of filling is not valid.";
    setError(aFeatureError);
    return true;
  }
  return false;
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
  GeomEdgePtr anEdge;
  switch (theShape->shapeType()) {
  case GeomAPI_Shape::EDGE:
    anEdge = GeomEdgePtr(new GeomAPI_Edge(GeomAlgoAPI_Copy(theShape).shape()));
    break;
  case GeomAPI_Shape::WIRE:
    anEdge = GeomAlgoAPI_ShapeTools::wireToEdge(
        GeomWirePtr(new GeomAPI_Wire(theShape)));
    break;
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
    GeomPointPtr aStartPnt = anEdge->firstPoint();
    GeomPointPtr aEndPnt = anEdge->lastPoint();
    if (anEdge->orientation() == GeomAPI_Shape::REVERSED) {
      aStartPnt = anEdge->lastPoint();
      aEndPnt = anEdge->firstPoint();
    }
    bool isReverse = false;
    if (myLastEdgeStartPoint) {
      double d1 = myLastEdgeStartPoint->distance(aStartPnt)
                + myLastEdgeEndPoint->distance(aEndPnt);
      double d2 = myLastEdgeStartPoint->distance(aEndPnt)
                + myLastEdgeEndPoint->distance(aStartPnt);
      if (fabs(d1 - d2) < 1.e-7) {
        // undefined case => check distance to start point only
        d1 = myLastEdgeStartPoint->distance(aStartPnt);
        d2 = myLastEdgeStartPoint->distance(aEndPnt);
      }
      isReverse = d2 < d1;
    }

    if (isReverse) {
      anEdge->reverse();
      myLastEdgeStartPoint = aEndPnt;
      myLastEdgeEndPoint = aStartPnt;
    } else {
      myLastEdgeStartPoint = aStartPnt;
      myLastEdgeEndPoint = aEndPnt;
    }
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
