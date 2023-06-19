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

#include "GeomAlgoAPI_MakeVolume.h"

#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_ShapeTools.h>

#include <BOPAlgo_MakerVolume.hxx>

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_MakeVolume::make(const ListOfShape& theFaces)
{
  GeomAlgoAPI_MakeVolume aMkVolAlgo(theFaces, false);
  GeomShapePtr aResult;
  if(aMkVolAlgo.isDone() && !aMkVolAlgo.shape()->isNull() && aMkVolAlgo.isValid())
    aResult = aMkVolAlgo.shape();
  return aResult;
}

//=================================================================================================
GeomAlgoAPI_MakeVolume::GeomAlgoAPI_MakeVolume(
  const ListOfShape& theFaces, const bool theAvoidInternal)
{
  myAvoidInternal = theAvoidInternal;
  build(theFaces);
}

static void convertToTopoDS(const ListOfShape& theShapes, TopTools_ListOfShape& theTopoDSShapes)
{
  for (ListOfShape::const_iterator anIt = theShapes.begin(); anIt != theShapes.end(); ++anIt)
    theTopoDSShapes.Append((*anIt)->impl<TopoDS_Shape>());
}

//=================================================================================================
void GeomAlgoAPI_MakeVolume::build(const ListOfShape& theFaces)
{
  if (theFaces.empty()) {
    return;
  }

  // create make volume opration
  BOPAlgo_MakerVolume* aVolumeMaker = new BOPAlgo_MakerVolume;
  this->setImpl(aVolumeMaker);
  this->setBuilderType(OCCT_BOPAlgo_Builder);

  // list of arguments
  TopTools_ListOfShape anArgs;
  convertToTopoDS(theFaces, anArgs);

  // parameters of the volume maker
  aVolumeMaker->SetArguments(anArgs);
  aVolumeMaker->SetIntersect(true); // split edges and faces
  aVolumeMaker->SetAvoidInternalShapes(myAvoidInternal);
  aVolumeMaker->SetGlue(BOPAlgo_GlueOff);

  // building and getting result
  aVolumeMaker->Perform();
  if (aVolumeMaker->HasErrors())
    return;
  TopoDS_Shape aResult = aVolumeMaker->Shape();

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    aGeomShape->setImpl(new TopoDS_Shape(aResult));
    aResult = GeomAlgoAPI_ShapeTools::groupSharedTopology(aGeomShape)->impl<TopoDS_Shape>();
  }

  // Setting result.
  if(aResult.IsNull()) {
    return;
  }
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}

//=================================================================================================
void GeomAlgoAPI_MakeVolume::modified(const GeomShapePtr theOldShape,
                                      ListOfShape& theNewShapes)
{
  if (theOldShape->shapeType() == GeomAPI_Shape::SOLID) {
    ListOfShape aNewShapes;
    // collect faces and parent shapes, if it is not done yet
    if (!isNewShapesCollected(theOldShape, GeomAPI_Shape::FACE))
      collectNewShapes(theOldShape, GeomAPI_Shape::FACE);

    for (GeomAPI_ShapeExplorer anIt(shape(), GeomAPI_Shape::SOLID); anIt.more(); anIt.next()) {
      for (GeomAPI_ShapeExplorer anExp(anIt.current(), GeomAPI_Shape::FACE);
           anExp.more(); anExp.next()) {
        GeomShapePtr anOldShapesCompound =
            oldShapesForNew(theOldShape, anExp.current(), GeomAPI_Shape::FACE);
        if (!anOldShapesCompound->isNull()) {
          aNewShapes.push_back(anIt.current());
          break;
        }
      }
    }

    if (!aNewShapes.empty())
      theNewShapes = aNewShapes;
  }
  else
    GeomAlgoAPI_MakeShape::modified(theOldShape, theNewShapes);
}
