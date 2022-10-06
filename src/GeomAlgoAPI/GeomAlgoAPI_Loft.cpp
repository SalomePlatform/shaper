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

#include "GeomAlgoAPI_DFLoader.h"
#include "GeomAlgoAPI_Loft.h"
#include "GeomAlgoAPI_WireBuilder.h"

#include <BRepOffsetAPI_ThruSections.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//==================================================================================================
GeomAlgoAPI_Loft::GeomAlgoAPI_Loft(const GeomShapePtr theFirstShape, const GeomShapePtr theSecondShape)
{
  build(theFirstShape, theSecondShape);
}

//==================================================================================================
void GeomAlgoAPI_Loft::build(const GeomShapePtr theFirstShape,
                             const GeomShapePtr theSecondShape)
{
  // Getting base shape.
  if(!theFirstShape.get() || !theSecondShape.get()) {
    return;
  }
  TopoDS_Shape aFirstShape = theFirstShape->impl<TopoDS_Shape>();
  if(aFirstShape.IsNull()) {
    return;
  }
  TopoDS_Shape aSecondShape= theSecondShape->impl<TopoDS_Shape>();
  if(aSecondShape.IsNull()) {
    return;
  }

  bool anIsSolid = false;

  TopoDS_Shape aFirstShapeOut;
  TopoDS_Shape aSecondShapeOut;
  if (aFirstShape.ShapeType() == TopAbs_FACE) {
    TopExp_Explorer anExp(aFirstShape, TopAbs_WIRE);
    aFirstShapeOut = anExp.Current();
    TopExp_Explorer anExp2(aSecondShape, TopAbs_WIRE);
    aSecondShapeOut = anExp2.Current();
    anIsSolid = true;
  }

  if (aFirstShape.ShapeType() == TopAbs_WIRE) {
    aFirstShapeOut = aFirstShape;
    aSecondShapeOut = aSecondShape;
  }

  if (aFirstShape.ShapeType() == TopAbs_EDGE)
  {
    GeomShapePtr aFirstWire, aSecondWire;
    ListOfShape aFirstEdge, aSecondEdge;

    // Convert first edge to wire
    aFirstEdge.push_back(theFirstShape);
    aFirstWire = GeomAlgoAPI_WireBuilder::wire(aFirstEdge);
    TopoDS_Shape aFirstShape = aFirstWire->impl<TopoDS_Shape>();

    // Convert first edge to wire
    aSecondEdge.push_back(theSecondShape);
    aSecondWire = GeomAlgoAPI_WireBuilder::wire(aSecondEdge);
    TopoDS_Shape aSecondShape = aSecondWire->impl<TopoDS_Shape>();

    aFirstShapeOut = aFirstShape;
    aSecondShapeOut = aSecondShape;
  }

  // Initialize and build
  BRepOffsetAPI_ThruSections * ThruSections =
                              new BRepOffsetAPI_ThruSections(anIsSolid);
  ThruSections->AddWire( TopoDS::Wire( aFirstShapeOut ) );
  ThruSections->AddWire( TopoDS::Wire( aSecondShapeOut) );

  try
  {
    ThruSections->Build();
  }
  catch (Standard_Failure& aFail)
  {
    delete ThruSections;
    return;
  }
  // Checking result.
  if(!ThruSections->IsDone() || ThruSections->Shape().IsNull()) {
    delete ThruSections;
    return;
  }

  this->initialize(ThruSections);

  // Setting result.
  TopoDS_Shape aResult = ThruSections->Shape();
  aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  GeomShapePtr aGeomSh(new GeomAPI_Shape());
  aGeomSh->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aGeomSh);
  this->setDone(true);
}
