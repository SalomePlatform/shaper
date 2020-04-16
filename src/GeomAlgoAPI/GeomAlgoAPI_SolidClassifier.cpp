// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_SolidClassifier.h"

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Solid.h>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <TopoDS.hxx>


static GeomAlgoAPI_SolidClassifier::State stateToState(const TopAbs_State theState)
{
  GeomAlgoAPI_SolidClassifier::State aResult;
  switch (theState)
  {
  case TopAbs_IN:
    aResult = GeomAlgoAPI_SolidClassifier::State_IN;
    break;
  case TopAbs_ON:
    aResult = GeomAlgoAPI_SolidClassifier::State_ON;
    break;
  case TopAbs_OUT:
    aResult = GeomAlgoAPI_SolidClassifier::State_OUT;
    break;
  default:
    aResult = GeomAlgoAPI_SolidClassifier::State_ALL;
    break;
  }
  return aResult;
}

static GeomAlgoAPI_SolidClassifier::State
classifyMiddlePoint(BRepClass3d_SolidClassifier& theClassifier,
                    const GeomShapePtr& theShape,
                    const double theTolerance)
{
  GeomPointPtr aMiddlePoint = theShape->middlePoint();
  theClassifier.Perform(aMiddlePoint->impl<gp_Pnt>(), theTolerance);
  return stateToState(theClassifier.State());
}

static GeomAlgoAPI_SolidClassifier::State
classifyVertices(BRepClass3d_SolidClassifier& theClassifier,
                 const GeomShapePtr& theShape,
                 const double theTolerance)
{
  GeomAlgoAPI_SolidClassifier::State aResult = GeomAlgoAPI_SolidClassifier::State_UNKNOWN;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  for (TopExp_Explorer anExp(aShape, TopAbs_VERTEX);
       anExp.More() && aResult != GeomAlgoAPI_SolidClassifier::State_ALL;
       anExp.Next()) {
    TopoDS_Vertex aCurV = TopoDS::Vertex(anExp.Current());
    theClassifier.Perform(BRep_Tool::Pnt(aCurV), theTolerance);
    aResult |= stateToState(theClassifier.State());
  }
  return aResult;
}

static GeomAlgoAPI_SolidClassifier::State
classifyVicinityOnEdge(BRepClass3d_SolidClassifier& theClassifier,
                       const TopoDS_Edge& theEdge,
                       const double theParameter,
                       const double theTolerance)
{
  // coefficient to step out of the vicinity of theParameter
  static const double THE_STEP_COEFF = 2.0;

  BRepAdaptor_Curve aCurve(theEdge);
  double aParStep = aCurve.Resolution(theTolerance) * THE_STEP_COEFF;
  double aParams[2] = { theParameter - aParStep, theParameter + aParStep };

  GeomAlgoAPI_SolidClassifier::State aResult = GeomAlgoAPI_SolidClassifier::State_UNKNOWN;
  for (double* anIt = std::begin(aParams); anIt != std::end(aParams); ++anIt) {
    if (*anIt < aCurve.FirstParameter())
      *anIt = aCurve.FirstParameter();
    else if (*anIt > aCurve.LastParameter())
      *anIt = aCurve.LastParameter();

    gp_Pnt aPnt = aCurve.Value(*anIt);
    theClassifier.Perform(aPnt, theTolerance);
    aResult |= stateToState(theClassifier.State());
  }
  return aResult;
}

static GeomAlgoAPI_SolidClassifier::State
classifyVicinityOnFace(BRepClass3d_SolidClassifier& theClassifier,
                       const TopoDS_Face& theFace,
                       const double theU,
                       const double theV,
                       const double theTolerance)
{
  // coefficient to step out of the vicinity of parameters
  static const double THE_STEP_COEFF = 2.0;

  BRepAdaptor_Surface aSurf(theFace);
  double aStepU = aSurf.UResolution(theTolerance) * THE_STEP_COEFF;
  double aStepV = aSurf.VResolution(theTolerance) * THE_STEP_COEFF;
  double aParamsU[3] = { theU - aStepU, theU, theU + aStepU };
  double aParamsV[3] = { theV - aStepV, theV, theV + aStepV };
  for (double* aU = std::begin(aParamsU); aU != std::end(aParamsU); ++aU) {
    if (*aU < aSurf.FirstUParameter())
      *aU = aSurf.FirstUParameter();
    else if (*aU > aSurf.LastUParameter())
      *aU = aSurf.LastUParameter();
  }
  for (double* aV = std::begin(aParamsV); aV != std::end(aParamsV); ++aV) {
    if (*aV < aSurf.FirstVParameter())
      *aV = aSurf.FirstVParameter();
    else if (*aV > aSurf.LastVParameter())
      *aV = aSurf.LastVParameter();
  }

  GeomAlgoAPI_SolidClassifier::State aResult = GeomAlgoAPI_SolidClassifier::State_UNKNOWN;
  for (double* aU = std::begin(aParamsU); aU != std::end(aParamsU); ++aU)
    for (double* aV = std::begin(aParamsV); aV != std::end(aParamsV); ++aV) {
      gp_Pnt aPnt = aSurf.Value(*aU, *aV);
      theClassifier.Perform(aPnt, theTolerance);
      aResult |= stateToState(theClassifier.State());
    }
  return aResult;
}

static GeomAlgoAPI_SolidClassifier::State
classifyByDistance(BRepClass3d_SolidClassifier& theClassifier,
                   const GeomShapePtr& theSolid,
                   const GeomShapePtr& theShape,
                   const double theTolerance)
{
  GeomAlgoAPI_SolidClassifier::State aResult = GeomAlgoAPI_SolidClassifier::State_UNKNOWN;

  const TopoDS_Shape& aSolid = theSolid->impl<TopoDS_Shape>();
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  for (TopExp_Explorer anExp(aSolid, TopAbs_SHELL); anExp.More(); anExp.Next()) {
    // compare distance from the shape to the solid's shells
    BRepExtrema_DistShapeShape aDistance(anExp.Current(), aShape);
    if (aDistance.Perform() && aDistance.Value() < theTolerance) {
      aResult |= GeomAlgoAPI_SolidClassifier::State_ON;
      // classify vicinity of intersection points
      for (int sol = 1; sol <= aDistance.NbSolution(); ++sol) {
        if (aDistance.SupportTypeShape2(sol) == BRepExtrema_IsOnEdge) {
          TopoDS_Edge anEdge = TopoDS::Edge(aDistance.SupportOnShape2(sol));
          double aParOnEdge;
          aDistance.ParOnEdgeS2(sol, aParOnEdge);

          aResult |= classifyVicinityOnEdge(theClassifier, anEdge, aParOnEdge, theTolerance);
        }
        else if (aDistance.SupportTypeShape2(sol) == BRepExtrema_IsInFace) {
          TopoDS_Face aFace = TopoDS::Face(aDistance.SupportOnShape2(sol));
          double aParU, aParV;
          aDistance.ParOnFaceS2(sol, aParU, aParV);

          aResult |= classifyVicinityOnFace(theClassifier, aFace, aParU, aParV, theTolerance);
        }
      }
    }
  }
  return aResult;
}


//==================================================================================================
GeomAlgoAPI_SolidClassifier::GeomAlgoAPI_SolidClassifier(const GeomSolidPtr theSolid,
                                                         const GeomShapePtr theShape,
                                                         const double theTolerance)
  : myState(State_UNKNOWN)
{
  if (!theSolid || !theShape)
    return;

  BRepClass3d_SolidClassifier aClassifierAlgo(theSolid->impl<TopoDS_Shape>());

  myState = classifyMiddlePoint(aClassifierAlgo, theShape, theTolerance);
  if (!theShape->isVertex())
    myState |= classifyVertices(aClassifierAlgo, theShape, theTolerance);
  myState |= classifyByDistance(aClassifierAlgo, theSolid, theShape, theTolerance);

  if (myState == State_ALL)
    myState = State_UNKNOWN;
}
