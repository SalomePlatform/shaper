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

#include "GeomAlgoAPI_Partition.h"

#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_SortListOfShapes.h>

#include <GEOMAlgo_Splitter.hxx>

#include <NCollection_Vector.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Builder.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepTools_History.hxx>


//=================================================================================================
static bool isSubShape(const TopoDS_Shape& theShape, const TopoDS_Shape& theSubShape)
{
  for(TopExp_Explorer anExp(theShape, theSubShape.ShapeType()); anExp.More(); anExp.Next()) {
    if(theSubShape.IsSame(anExp.Current())) {
      return true;
    }
  }

  return false;
}

//=================================================================================================
void getHistorySupportedType(const TopoDS_Shape& theShape, TopTools_ListOfShape& theResult) {
  if (BRepTools_History::IsSupportedType(theShape)) {
    theResult.Append(theShape);
  } else {
    for (TopoDS_Iterator aSubIter(theShape); aSubIter.More(); aSubIter.Next()) {
      getHistorySupportedType(aSubIter.Value(), theResult);
    }
  }
}

//=================================================================================================
// Operation is used for production of ordered sorting: generated/modified from the first argument
// must be located in hte result first, etc. THis is for the issue #2517
static void sortCompound(TopoDS_Shape& theCompound, GEOMAlgo_Splitter* theOperation)
{
  TopoDS_Compound aResCompound;
  TopoDS_Builder aBuilder;
  aBuilder.MakeCompound(aResCompound);

  TopTools_MapOfShape anAlreadyThere; // to avoid duplications if it was produced by two arguments

  // flag to add to result also results which were not produced by any argument
  bool aNotProduced = true;
  TopTools_ListOfShape::Iterator anArgs(theOperation->Arguments());
  while(aNotProduced || anArgs.More()) {
    // collect shapes that were produced from the current argument
    TopTools_MapOfShape aProducedByArg;
    if (anArgs.More()) {
      TopTools_ListOfShape allArgs;
      getHistorySupportedType(anArgs.Value(), allArgs);
      for (TopTools_ListOfShape::Iterator argsIter(allArgs); argsIter.More(); argsIter.Next()) {
        // if argument was not modified, it is fully in the result
        aProducedByArg.Add(argsIter.Value());
        const TopTools_ListOfShape& aModified = theOperation->Modified(argsIter.Value());
        for (TopTools_ListOfShape::Iterator aModIter(aModified); aModIter.More(); aModIter.Next()) {
          aProducedByArg.Add(aModIter.Value());
        }
        const TopTools_ListOfShape& aGenerated = theOperation->Generated(argsIter.Value());
        for (TopTools_ListOfShape::Iterator aGenIter(aGenerated); aGenIter.More(); aGenIter.Next())
        {
          aProducedByArg.Add(aGenIter.Value());
        }
      }
      anArgs.Next();
    }
    else {
      aNotProduced = false;
    }

    ListOfShape aCombiningShapes;
    for (TopoDS_Iterator anIt(theCompound); anIt.More(); anIt.Next()) {
      bool aProducedContains = false;
      if (aNotProduced) { // collect all supported type-shapes of result
        TopTools_ListOfShape allRes;
        getHistorySupportedType(anIt.Value(), allRes);
        for (TopTools_ListOfShape::Iterator aResIter(allRes); aResIter.More(); aResIter.Next()) {
          if (aProducedByArg.Contains(aResIter.Value())) {
            aProducedContains = true;
            break;
          }
        }
      }
      if ((!aNotProduced || aProducedContains) && anAlreadyThere.Add(anIt.Value())) {
        GeomShapePtr aSub(new GeomAPI_Shape);
        aSub->setImpl(new TopoDS_Shape(anIt.Value()));
        aCombiningShapes.push_back(aSub);
      }
    }

    // sort sub-shapes of compound to stabilize the sequence of the Partition's results
    GeomAlgoAPI_SortListOfShapes::sort(aCombiningShapes);

    for (ListOfShape::iterator anIt = aCombiningShapes.begin();
      anIt != aCombiningShapes.end(); ++anIt)
      aBuilder.Add(aResCompound, (*anIt)->impl<TopoDS_Shape>());
  }

  theCompound = aResCompound;
}

//=================================================================================================
GeomAlgoAPI_Partition::GeomAlgoAPI_Partition(const ListOfShape& theObjects,
                                             const ListOfShape& theTools)
{
  build(theObjects, theTools);
}

static void prepareShapes(const TopoDS_Shape&   theShape,
                           TopTools_ListOfShape& theSimpleList)
{
  if (theShape.ShapeType() != TopAbs_COMPOUND) {
      theSimpleList.Append(theShape);
    return;
  }

  // explode compound on simple shapes to allow their intersections
  TopoDS_Iterator It (theShape, Standard_True, Standard_True);
  for (; It.More(); It.Next()) {
    TopoDS_Shape curSh = It.Value();
    prepareShapes(curSh, theSimpleList);
  }
}

//=================================================================================================
void GeomAlgoAPI_Partition::build(const ListOfShape& theObjects,
                                  const ListOfShape& theTools)
{
  if (theObjects.empty()) {
    return;
  }

  // Creating partition operation.
  GEOMAlgo_Splitter* anOperation = new GEOMAlgo_Splitter;
  this->setImpl(anOperation);
  this->setBuilderType(OCCT_BOPAlgo_Builder);

  TopTools_MapOfShape ShapesMap;
  // Getting objects.
  for(ListOfShape::const_iterator anObjectsIt = theObjects.begin();
      anObjectsIt != theObjects.end();
      anObjectsIt++)
  {
    const TopoDS_Shape& aShape = (*anObjectsIt)->impl<TopoDS_Shape>();
    // #2240: decompose compounds to get the valid result
    TopTools_ListOfShape aSimpleShapes;
    prepareShapes(aShape, aSimpleShapes);
    TopTools_ListIteratorOfListOfShape aSimpleIter(aSimpleShapes);
    for (; aSimpleIter.More(); aSimpleIter.Next()) {
      const TopoDS_Shape& aSimpleSh = aSimpleIter.Value();
      if (ShapesMap.Add(aSimpleSh)) {
        anOperation->AddArgument(aSimpleSh);
      }
    }
  }

  // Getting tools.
  for (ListOfShape::const_iterator
       aToolsIt = theTools.begin(); aToolsIt != theTools.end(); aToolsIt++) {
    const TopoDS_Shape& aShape = (*aToolsIt)->impl<TopoDS_Shape>();
    // #2419: decompose compounds to get the valid result
    TopTools_ListOfShape aSimpleShapes;
    prepareShapes(aShape, aSimpleShapes);
    TopTools_ListIteratorOfListOfShape aSimpleIter(aSimpleShapes);
    for (; aSimpleIter.More(); aSimpleIter.Next()) {
      const TopoDS_Shape& aSimpleSh = aSimpleIter.Value();
      if (ShapesMap.Add(aSimpleSh)) {
        anOperation->AddTool(aSimpleSh);
      }
    }
  }

  // Building and getting result.
  anOperation->Perform();
  if (anOperation->HasErrors())
    return;
  TopoDS_Shape aResult = anOperation->Shape();

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    // Exclude faces and edges which are shared as another sub-shape.
    NCollection_Vector<TopoDS_Shape> aFaces;
    NCollection_Vector<TopoDS_Shape> anEdges;
    TopoDS_Compound aTempCompound;
    TopoDS_Builder aBuilder;
    aBuilder.MakeCompound(aTempCompound);
    for(TopoDS_Iterator anIt(aResult);
        anIt.More();
        anIt.Next()) {
      const TopoDS_Shape& aSubShape = anIt.Value();
      if (aSubShape.ShapeType() == TopAbs_FACE) {
        aFaces.Append(aSubShape);
      } else if (aSubShape.ShapeType() == TopAbs_EDGE) {
        anEdges.Append(aSubShape);
      } else {
        aBuilder.Add(aTempCompound, aSubShape);
      }
    }

    for (NCollection_Vector<TopoDS_Shape>::Iterator anIt(aFaces);
        anIt.More();
        anIt.Next())
    {
      const TopoDS_Shape& aSubShape = anIt.Value();
      if (!isSubShape(aTempCompound, aSubShape))
      {
        aBuilder.Add(aTempCompound, aSubShape);
      }
    }

    for (NCollection_Vector<TopoDS_Shape>::Iterator anIt(anEdges);
        anIt.More();
        anIt.Next())
    {
      const TopoDS_Shape& aSubShape = anIt.Value();
      if (!isSubShape(aTempCompound, aSubShape))
      {
        aBuilder.Add(aTempCompound, aSubShape);
      }
    }

    aResult = aTempCompound;
  }

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    // sort sub-shapes of compound before creation of a compsolid
    sortCompound(aResult, anOperation);

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
