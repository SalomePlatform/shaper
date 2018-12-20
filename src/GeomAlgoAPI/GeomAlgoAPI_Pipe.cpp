// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_Pipe.h"

#include "GeomAlgoAPI_DFLoader.h"

#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

#include <BRep_Tool.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <gp_Lin.hxx>
#include <NCollection_List.hxx>
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
  // Getting base shape and path.
  TopoDS_Shape aBaseShape;
  TopAbs_ShapeEnum aBaseShapeType;
  TopoDS_Wire aPathWire;
  if (!getBase(aBaseShape, aBaseShapeType, theBaseShape) ||
      !getPath(aPathWire, thePathShape)) {
    return;
  }

  aPathWire.Move(getPathToBaseTranslation(aBaseShape, aPathWire));

  // Making pipe.
  BRepOffsetAPI_MakePipe* aPipeBuilder = new BRepOffsetAPI_MakePipe(aPathWire, aBaseShape);
  if(!aPipeBuilder) {
    return;
  }
  aPipeBuilder->Build();

  // Checking result.
  if(!aPipeBuilder->IsDone() || aPipeBuilder->Shape().IsNull()) {
    delete aPipeBuilder;
    return;
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

  aPathWire.Move(getPathToBaseTranslation(theBaseShape->impl<TopoDS_Shape>(), aPathWire));

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
  aPathWire.Move(aTrsf);

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
  GeomAlgoAPI_MakeShape::generated(theShape, theHistory);
}

// Auxilary functions:
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
  aDist.Perform();
  if (aDist.IsDone() && aDist.Value() > Precision::Confusion()) {
    gp_Pnt aPntBase = aDist.PointOnShape1(1);
    gp_Pnt aPntPath = aDist.PointOnShape2(1);
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
