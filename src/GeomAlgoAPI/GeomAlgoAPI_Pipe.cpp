// Copyright (C) 2014-2025  CEA, EDF
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

#include "GeomAlgoAPI_Pipe.h"

#include "GeomAlgoAPI_DFLoader.h"

#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeExplorer.h>

#include <BRep_Tool.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BOPAlgo_ArgumentAnalyzer.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <gp_Lin.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Precision.hxx>

static bool getBase(TopoDS_Shape& theBaseOut,
                    TopAbs_ShapeEnum& theBaseTypeOut,
                    const GeomShapePtr theBaseShape);
static bool getPath(TopoDS_Wire& thePathOut,
                    const GeomShapePtr thePathShape);
static gp_Trsf getPathToBaseTranslation(const TopoDS_Shape& theBase,
                                        const TopoDS_Shape& thePath);
static bool buildPipe(BRepOffsetAPI_MakePipeShell* thePipeBuilder);
static ListOfShape getListFromShape(const TopoDS_Shape& theShape);

//==================================================================================================
GeomAlgoAPI_Pipe::GeomAlgoAPI_Pipe(const GeomShapePtr theBaseShape,
                                   const GeomShapePtr thePathShape)
{
  build(theBaseShape, thePathShape);
}

//==================================================================================================
GeomAlgoAPI_Pipe::GeomAlgoAPI_Pipe(const GeomShapePtr theBaseShape,
                                   const GeomShapePtr thePathShape,
                                   const GeomShapePtr theBiNormal)
{
  build(theBaseShape, thePathShape, theBiNormal);
}

//==================================================================================================
GeomAlgoAPI_Pipe::GeomAlgoAPI_Pipe(const ListOfShape& theBaseShapes,
                                   const ListOfShape& theLocations,
                                   const GeomShapePtr thePathShape)
{
  build(theBaseShapes, theLocations, thePathShape);
}

//==================================================================================================
void GeomAlgoAPI_Pipe::build(const GeomShapePtr theBaseShape,
                             const GeomShapePtr thePathShape)
{
  // Getting base shape.
  if(!theBaseShape.get()) {
    return;
  }
  TopoDS_Shape aBaseShape = theBaseShape->impl<TopoDS_Shape>();
  if(aBaseShape.IsNull()) {
    return;
  }
  TopAbs_ShapeEnum aBaseShapeType = aBaseShape.ShapeType();
  if(aBaseShapeType != TopAbs_VERTEX && aBaseShapeType != TopAbs_EDGE &&
     aBaseShapeType != TopAbs_WIRE && aBaseShapeType != TopAbs_FACE &&
     aBaseShapeType != TopAbs_SHELL && aBaseShapeType != TopAbs_COMPOUND) {
    return;
  }

  // Getting path.
  TopoDS_Wire aPathWire;
  if(!getPath(aPathWire, thePathShape)) {
    return;
  }
  GeomShapePtr anOldPath(new GeomAPI_Shape), aNewPath(new GeomAPI_Shape);
  anOldPath->setImpl(new TopoDS_Shape(aPathWire));
  aPathWire.Move(getPathToBaseTranslation(aBaseShape, aPathWire));
  aNewPath->setImpl(new TopoDS_Shape(aPathWire));
  if (!anOldPath->isSame(aNewPath))
    addMovedPath(anOldPath, aNewPath);

  // Making pipe.
  BRepOffsetAPI_MakePipe* aPipeBuilder = NULL;
  try {
    aPipeBuilder = new BRepOffsetAPI_MakePipe(aPathWire, aBaseShape);
  }
  catch (...) {
  }

  // Checking result.
  bool isDone = false;
  static const Standard_Real TolPipeSurf = 5.e-4;
  if (aPipeBuilder &&
      aPipeBuilder->IsDone() &&
      !aPipeBuilder->Shape().IsNull() &&
      aPipeBuilder->ErrorOnSurface() <= TolPipeSurf) {
    // check result
    BOPAlgo_ArgumentAnalyzer aChecker;
    aChecker.SetShape1(aPipeBuilder->Shape());
    aChecker.SelfInterMode() = Standard_True;
    aChecker.StopOnFirstFaulty() = Standard_True;
    aChecker.Perform();
    if (!aChecker.HasFaulty())
      isDone = true;
  }

  if (!isDone) {
    if (aPipeBuilder)
      delete aPipeBuilder;

    // Try to use Descrete Trihedron mode.
    aPipeBuilder = new BRepOffsetAPI_MakePipe
      (aPathWire, aBaseShape, GeomFill_IsDiscreteTrihedron, Standard_True);
    if (!aPipeBuilder->IsDone() || aPipeBuilder->Shape().IsNull()) {
      myError = "Pipe algorithm has failed.";
      delete aPipeBuilder;
      return;
    }

    // Check for self-interfering result
    BOPAlgo_ArgumentAnalyzer aChecker;
    aChecker.SetShape1(aPipeBuilder->Shape());
    aChecker.SelfInterMode() = Standard_True;
    aChecker.StopOnFirstFaulty() = Standard_True;
    aChecker.Perform();
    if (aChecker.HasFaulty()) {
      myError = "Self-interfering result.";
      delete aPipeBuilder;
      return;
    }
  }

  this->initialize(aPipeBuilder);

  // Setting naming.
  this->setToShapes(getListFromShape(aPipeBuilder->LastShape()));
  this->setFromShapes(getListFromShape(aPipeBuilder->FirstShape()));

  // Setting result.
  TopoDS_Shape aResult = aPipeBuilder->Shape();
  aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  GeomShapePtr aGeomSh(new GeomAPI_Shape());
  aGeomSh->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aGeomSh);
  this->setDone(true);
}

//==================================================================================================
void GeomAlgoAPI_Pipe::build(const GeomShapePtr theBaseShape,
                             const GeomShapePtr thePathShape,
                             const GeomShapePtr theBiNormal)
{
  // Getting base shape and path.
  TopoDS_Shape aBaseShape;
  TopAbs_ShapeEnum aBaseShapeType;
  TopoDS_Wire aPathWire;
  if (!getBase(aBaseShape, aBaseShapeType, theBaseShape) ||
      !getPath(aPathWire, thePathShape) ||
      !theBiNormal.get()) {
    return;
  }

  GeomShapePtr anOldPath(new GeomAPI_Shape), aNewPath(new GeomAPI_Shape);
  anOldPath->setImpl(new TopoDS_Shape(aPathWire));
  aPathWire.Move(getPathToBaseTranslation(theBaseShape->impl<TopoDS_Shape>(), aPathWire));
  aNewPath->setImpl(new TopoDS_Shape(aPathWire));
  if (!anOldPath->isSame(aNewPath))
    addMovedPath(anOldPath, aNewPath);

  // Getting Bi-Normal.
  TopoDS_Shape aBiNormalShape = theBiNormal->impl<TopoDS_Shape>();
  if(aBiNormalShape.IsNull() || aBiNormalShape.ShapeType() != TopAbs_EDGE) {
    return;
  }
  TopoDS_Edge aBiNormalEdge = TopoDS::Edge(aBiNormalShape);
  Standard_Real aFirst, aLast;
  Handle(Geom_Curve) aBiNormalCurve = BRep_Tool::Curve(aBiNormalEdge, aFirst, aLast);
  Handle(Geom_Line) aBiNormalLine = Handle(Geom_Line)::DownCast(aBiNormalCurve);
  if(aBiNormalLine.IsNull()) {
    return;
  }
  gp_Dir aBiNormalDir = aBiNormalLine->Lin().Direction();

  // Making pipe.
  BRepOffsetAPI_MakePipeShell* aPipeBuilder = new BRepOffsetAPI_MakePipeShell(aPathWire);
  if(!aPipeBuilder) {
    return;
  }
  aPipeBuilder->Add(aBaseShape);
  aPipeBuilder->SetMode(aBiNormalDir);
  if(!buildPipe(aPipeBuilder)) {
    delete aPipeBuilder;
    return;
  }
  this->initialize(aPipeBuilder);

  // Checking result.
  if(aBaseShapeType == TopAbs_FACE && !aPipeBuilder->MakeSolid()) {
    return;
  }
  TopoDS_Shape aResult = aPipeBuilder->Shape();
  if(aResult.IsNull()) {
    return;
  }

  // Setting naming.
  this->setToShapes(getListFromShape(aPipeBuilder->LastShape()));
  this->setFromShapes(getListFromShape(aPipeBuilder->FirstShape()));

  // Setting result.
  aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  GeomShapePtr aGeomSh(new GeomAPI_Shape());
  aGeomSh->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aGeomSh);
  this->setDone(true);
}

//==================================================================================================
void GeomAlgoAPI_Pipe::build(const ListOfShape& theBaseShapes,
                             const ListOfShape& theLocations,
                             const GeomShapePtr thePathShape)
{
  if(theBaseShapes.empty() ||
     (!theLocations.empty() && theLocations.size() != theBaseShapes.size())) {
    return;
  }

  // Getting base shape and path.
  TopoDS_Shape aBaseShape;
  TopAbs_ShapeEnum aBaseShapeType;
  TopoDS_Wire aPathWire;
  if (!getBase(aBaseShape, aBaseShapeType, theBaseShapes.front()) ||
      !getPath(aPathWire, thePathShape)) {
    return;
  }

  TopoDS_Shape aReallyBase = theBaseShapes.front()->impl<TopoDS_Shape>();
  gp_Trsf aTrsf = getPathToBaseTranslation(aReallyBase, aPathWire);

  GeomShapePtr anOldPath(new GeomAPI_Shape), aNewPath(new GeomAPI_Shape);
  anOldPath->setImpl(new TopoDS_Shape(aPathWire));
  aPathWire.Move(aTrsf);
  aNewPath->setImpl(new TopoDS_Shape(aPathWire));
  if (!anOldPath->isSame(aNewPath))
    addMovedPath(anOldPath, aNewPath);

  // Get locations after moving path shape.
  std::list<TopoDS_Vertex> aLocations;
  for (ListOfShape::const_iterator aLocIt = theLocations.cbegin();
       aLocIt != theLocations.cend();
       ++aLocIt)
  {
    GeomShapePtr aLocation = *aLocIt;
    if (!aLocation.get() || aLocation->shapeType() != GeomAPI_Shape::VERTEX) {
      return;
    }

    TopoDS_Vertex aLocationVertex = aLocation->impl<TopoDS_Vertex>();
    TopoDS_Vertex aMovedVertex;
    for (TopExp_Explorer anExp(aPathWire, TopAbs_VERTEX); anExp.More(); anExp.Next()) {
      if (anExp.Current().IsPartner(aLocationVertex)) {
        aMovedVertex = TopoDS::Vertex(anExp.Current());
        aLocations.push_back(aMovedVertex);
        break;
      }
    }
    if (aMovedVertex.IsNull()) {
      return;
    }
  }

  if (theLocations.size() != aLocations.size()) {
    return;
  }

  bool aHasLocations = !aLocations.empty();

  // Making pipe.
  Standard_Boolean isDone = Standard_False;
  bool anIsSolidNeeded = false;
  BRepOffsetAPI_MakePipeShell* aPipeBuilder;
  for(int i = 0; i < 2; ++i) {
    aPipeBuilder = new BRepOffsetAPI_MakePipeShell(aPathWire);
    if(!aPipeBuilder) {
      return;
    }
    ListOfShape::const_iterator aBaseIt = theBaseShapes.cbegin();
    std::list<TopoDS_Vertex>::const_iterator aLocationsIt = aLocations.cbegin();
    while(aBaseIt != theBaseShapes.cend()) {
      GeomShapePtr aBase = *aBaseIt;
      if(!getBase(aBaseShape, aBaseShapeType, aBase)) {
        delete aPipeBuilder;
        return;
      }
      ++aBaseIt;
      if(aBaseShapeType == TopAbs_FACE) {
        anIsSolidNeeded = true;
      }

      if(aHasLocations) {
        aPipeBuilder->Add(aBaseShape, *aLocationsIt);
        ++aLocationsIt;
      } else {
        aPipeBuilder->Add(aBaseShape);
      }
    }

    if(aPipeBuilder->IsReady() == Standard_False) {
      delete aPipeBuilder;
      return;
    }

    if (i == 1) {
       // Try to use Descrete Trihedron mode.
      aPipeBuilder->SetDiscreteMode();
    }
    aPipeBuilder->Build();
    isDone = aPipeBuilder->IsDone();

    if (isDone) {
      break;
    }

    delete aPipeBuilder;
  }

  if (!isDone) {
    return;
  }

  this->initialize(aPipeBuilder);

  // Checking result.
  if(anIsSolidNeeded && !aPipeBuilder->MakeSolid()) {
    return;
  }
  TopoDS_Shape aResult = aPipeBuilder->Shape();

  // Setting naming.
  GeomShapePtr aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
  aFromShape->setImpl(new TopoDS_Shape(aPipeBuilder->FirstShape()));
  aToShape->setImpl(new TopoDS_Shape(aPipeBuilder->LastShape()));
  fixOrientation(aFromShape);
  fixOrientation(aToShape);
  this->addFromShape(aFromShape);
  this->addToShape(aToShape);

  // Setting result.
  if(aResult.IsNull()) {
    return;
  }
  aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  GeomShapePtr aGeomSh(new GeomAPI_Shape());
  aGeomSh->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aGeomSh);
  this->setDone(true);
}

//==================================================================================================
void GeomAlgoAPI_Pipe::generated(const GeomShapePtr theShape,
                                 ListOfShape& theHistory)
{
  if (myMovedPath.isBound(theShape))
    GeomAlgoAPI_MakeShape::generated(myMovedPath.find(theShape), theHistory);
  else
    GeomAlgoAPI_MakeShape::generated(theShape, theHistory);
}

// Auxiliary functions:
//==================================================================================================
bool getBase(TopoDS_Shape& theBaseOut,
             TopAbs_ShapeEnum& theBaseTypeOut,
             const GeomShapePtr theBaseShape)
{
  if(!theBaseShape.get()) {
    return false;
  }

  theBaseOut = theBaseShape->impl<TopoDS_Shape>();
  if(theBaseOut.IsNull()) {
    return false;
  }
  theBaseTypeOut = theBaseOut.ShapeType();
  if(theBaseTypeOut == TopAbs_VERTEX) {
    // Do nothing.
  } else if(theBaseTypeOut == TopAbs_EDGE) {
    theBaseOut = BRepBuilderAPI_MakeWire(TopoDS::Edge(theBaseOut)).Shape();
  } else if(theBaseTypeOut == TopAbs_WIRE) {
    // Do nothing.
  } else if(theBaseTypeOut == TopAbs_FACE) {
    TopExp_Explorer anExp(theBaseOut, TopAbs_WIRE);
    theBaseOut = anExp.Current();
  } else {
    return false;
  }

  return true;
}

//==================================================================================================
bool getPath(TopoDS_Wire& thePathOut, const GeomShapePtr thePathShape)
{
  if(!thePathShape.get()) {
    return false;
  }

  TopoDS_Shape aPathShape = thePathShape->impl<TopoDS_Shape>();
  if(aPathShape.IsNull()) {
    return false;
  }
  TopAbs_ShapeEnum aPathShapeType = aPathShape.ShapeType();
  if(aPathShapeType == TopAbs_EDGE) {
    TopoDS_Edge aPathEdge = TopoDS::Edge(aPathShape);
    thePathOut = BRepBuilderAPI_MakeWire(aPathEdge).Wire();
  } else if(aPathShapeType == TopAbs_WIRE) {
    thePathOut = TopoDS::Wire(aPathShape);
  } else {
    return false;
  }

  return true;
}

//==================================================================================================
gp_Trsf getPathToBaseTranslation(const TopoDS_Shape& theBase, const TopoDS_Shape& thePath)
{
  gp_Trsf aTranslation;

  BRepExtrema_DistShapeShape aDist(theBase, thePath);
  if (aDist.IsDone() && aDist.Value() > Precision::Confusion()) {
    gp_Pnt aPntBase = aDist.PointOnShape1(1);
    gp_Pnt aPntPath = aDist.PointOnShape2(1);
    // get the end point on the path nearest to the found point,
    // because the path is almost usually started from the one of the sections
    if (!BRep_Tool::IsClosed(thePath)) {
      TopoDS_Wire aPathWire = TopoDS::Wire(thePath);
      if (!aPathWire.IsNull()) {
        TopoDS_Vertex aV1, aV2;
        TopExp::Vertices(aPathWire, aV1, aV2);

        gp_Pnt aStart = BRep_Tool::Pnt(aV1);
        gp_Pnt anEnd = BRep_Tool::Pnt(aV2);

        // compare 3 distances
        double aDistToStart = aPntBase.Distance(aStart);
        double aDistToEnd = aPntBase.Distance(anEnd);
        double aMinDist = aPntBase.Distance(aPntPath);

        static const double THE_THRESHOLD = 0.01; // threshold for distance ratio
        double aDeltaStart = Abs(aDistToStart - aMinDist);
        double aDeltaEnd = Abs(aDistToEnd - aMinDist);
        if (aDeltaStart < THE_THRESHOLD * aDeltaEnd)
          aPntPath = aStart;
        else if (aDeltaEnd < THE_THRESHOLD * aDeltaStart)
          aPntPath = anEnd;
        else
          aPntPath = aPntBase; // no translation
      }
    }
    aTranslation.SetTranslation(aPntPath, aPntBase);
  }

  return aTranslation;
}

//==================================================================================================
bool buildPipe(BRepOffsetAPI_MakePipeShell* thePipeBuilder)
{
  thePipeBuilder->Build();

  Standard_Boolean isDone = thePipeBuilder->IsDone();

  if (!isDone) {
    // Try to use Descrete Trihedron mode.
    thePipeBuilder->SetDiscreteMode();
    thePipeBuilder->Build();
    isDone = thePipeBuilder->IsDone();
  }

  return isDone == Standard_True;
}

//==================================================================================================
ListOfShape getListFromShape(const TopoDS_Shape& theShape)
{
  ListOfShape aList;

  TopAbs_ShapeEnum aType = theShape.ShapeType();
  if(aType == TopAbs_WIRE || aType == TopAbs_SHELL || aType == TopAbs_COMPOUND) {
    for(TopoDS_Iterator anIt(theShape); anIt.More(); anIt.Next()) {
      GeomShapePtr aGeomShape(new GeomAPI_Shape());
      aGeomShape->setImpl(new TopoDS_Shape(anIt.Value()));
      aList.push_back(aGeomShape);
    }
  } else {
    GeomShapePtr aGeomShape(new GeomAPI_Shape());
    aGeomShape->setImpl(new TopoDS_Shape(theShape));
    aList.push_back(aGeomShape);
  }

  return aList;
}

//==================================================================================================
void GeomAlgoAPI_Pipe::addMovedPath(GeomShapePtr thePath, GeomShapePtr theMoved)
{
  myMovedPath.clear();
  GeomAPI_ShapeExplorer anOldExp(thePath, GeomAPI_Shape::EDGE);
  GeomAPI_ShapeExplorer aNewExp(theMoved, GeomAPI_Shape::EDGE);
  for(; anOldExp.more(); anOldExp.next(), aNewExp.next()) {
    myMovedPath.bind(anOldExp.current(), aNewExp.current());
  }
}
