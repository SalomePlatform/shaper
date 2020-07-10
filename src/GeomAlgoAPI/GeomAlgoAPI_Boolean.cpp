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

#include "GeomAlgoAPI_Boolean.h"

#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BOPAlgo_BOP.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
GeomAlgoAPI_Boolean::GeomAlgoAPI_Boolean(const GeomShapePtr theObject,
                                         const GeomShapePtr theTool,
                                         const GeomAlgoAPI_Tools::BOPType theOperationType)
{
  ListOfShape aListWithObject, aListWithTool;
  aListWithObject.push_back(theObject);
  aListWithTool.push_back(theTool);
  build(aListWithObject, aListWithTool, theOperationType);
}

//=================================================================================================
GeomAlgoAPI_Boolean::GeomAlgoAPI_Boolean(const GeomShapePtr theObject,
                                         const ListOfShape& theTools,
                                         const GeomAlgoAPI_Tools::BOPType theOperationType)
{
  ListOfShape aListWithObject;
  aListWithObject.push_back(theObject);
  build(aListWithObject, theTools, theOperationType);
}

//=================================================================================================
GeomAlgoAPI_Boolean::GeomAlgoAPI_Boolean(const ListOfShape& theObjects,
                                         const ListOfShape& theTools,
                                         const GeomAlgoAPI_Tools::BOPType theOperationType)
{
  build(theObjects, theTools, theOperationType);
}


//=================================================================================================
void GeomAlgoAPI_Boolean::build(const ListOfShape& theObjects,
                                const ListOfShape& theTools,
                                const GeomAlgoAPI_Tools::BOPType theOperationType)
{
  if(theObjects.empty() || theTools.empty()) {
    return;
  }

  // Getting objects.
  TopTools_ListOfShape anObjects;
  for(ListOfShape::const_iterator
    anObjectsIt = theObjects.begin(); anObjectsIt != theObjects.end(); anObjectsIt++)
  {
    anObjects.Append((*anObjectsIt)->impl<TopoDS_Shape>());
  }

  // Getting tools.
  TopTools_ListOfShape aTools;
  for(ListOfShape::const_iterator
    aToolsIt = theTools.begin(); aToolsIt != theTools.end(); aToolsIt++)
  {
    aTools.Append((*aToolsIt)->impl<TopoDS_Shape>());
  }

  // Creating boolean operation.
  BOPAlgo_BOP* aBuilder = new BOPAlgo_BOP();
  switch (theOperationType) {
    case GeomAlgoAPI_Tools::BOOL_CUT: {
      aBuilder->SetOperation(BOPAlgo_CUT);
      break;
    }
    case GeomAlgoAPI_Tools::BOOL_FUSE: {
      aBuilder->SetOperation(BOPAlgo_FUSE);
      break;
    }
    case GeomAlgoAPI_Tools::BOOL_COMMON: {
      aBuilder->SetOperation(BOPAlgo_COMMON);
      break;
    }
    default: {
      return;
    }
  }
  this->setImpl(aBuilder);
  this->setBuilderType(OCCT_BOPAlgo_Builder);
  aBuilder->SetArguments(anObjects);
  aBuilder->SetTools(aTools);

  // Set parallel processing mode (default is false)
  Standard_Boolean bRunParallel = Standard_True;
  aBuilder->SetRunParallel(bRunParallel);

  // Set fuzzy value to eliminate thin results
  static const Standard_Real aFuzzy = 1.e-5;
  aBuilder->SetFuzzyValue(aFuzzy);

  // Building and getting result.
  aBuilder->Perform();
  if (aBuilder->HasErrors())
    return;
  TopoDS_Shape aResult = aBuilder->Shape();

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    aGeomShape->setImpl(new TopoDS_Shape(aResult));
    ListOfShape aResults;
    aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                       GeomAPI_Shape::COMPSOLID,
                                                       aResults);
    aResult = aGeomShape->impl<TopoDS_Shape>();
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}

static bool isHistoryType(TopAbs_ShapeEnum theType) {
  return theType == TopAbs_VERTEX || theType == TopAbs_EDGE ||
         theType == TopAbs_FACE || theType == TopAbs_SOLID;
}

/// searches the corresponding result for theOld
static void searchResult(const TopoDS_Shape& theOld, const TopoDS_Shape& theResult,
  BOPAlgo_BOP* theBuilder, TopTools_MapOfShape& theNews)
{
  if (theResult.ShapeType() == theOld.ShapeType()) { // check some sub-shape produces a sub-result
    if (theOld.IsSame(theResult)) {
      theNews.Add(theResult);
      return;
    }
    // searching for new result by sub-shapes of aSubType type
    TopAbs_ShapeEnum aSubType = TopAbs_ShapeEnum(int(theOld.ShapeType()) + 1);
    while(aSubType < TopAbs_VERTEX && !isHistoryType(aSubType))
      aSubType = TopAbs_ShapeEnum(int(aSubType) + 1);
    if (aSubType == TopAbs_SHAPE)
      return;
    TopTools_MapOfShape aResSubs;
    for(TopExp_Explorer aResExp(theResult, aSubType); aResExp.More(); aResExp.Next())
      aResSubs.Add(aResExp.Current());
    for(TopExp_Explorer anExp(theOld, aSubType); anExp.More(); anExp.Next()) {
      const TopTools_ListOfShape& aNewSubs = theBuilder->Modified(anExp.Current());
      // searching for this new sub in theResult
      for(TopTools_ListIteratorOfListOfShape aNewSub(aNewSubs); aNewSub.More(); aNewSub.Next()) {
        if (aResSubs.Contains(aNewSub.Value())) {
          theNews.Add(theResult);
          return;
        }
      }
    }
  } else if (theResult.ShapeType() < theOld.ShapeType()) { // recursive search among sub-shapes
    for(TopoDS_Iterator aSubResults(theResult); aSubResults.More(); aSubResults.Next()) {
      searchResult(theOld, aSubResults.Value(), theBuilder, theNews);
    }
  }
}

// check the shape is on the higher level of compound or compsolid
bool isInComp(const TopoDS_Shape& theComp, const TopoDS_Shape& theShape) {
  if (theComp.ShapeType() == TopAbs_COMPOUND || theComp.ShapeType() == TopAbs_COMPSOLID) {
    for(TopoDS_Iterator anIter(theComp); anIter.More(); anIter.Next()) {
      if (isInComp(anIter.Value(), theShape))
        return true;
    }
  } else return theShape.IsSame(theComp);
  return false;
}

//=================================================================================================
/// make arguments of Fuse produce result shapes with "modified" evolution
void GeomAlgoAPI_Boolean::modified(const GeomShapePtr theOldShape, ListOfShape& theNewShapes)
{
  BOPAlgo_BOP* aBuilder = this->implPtr<BOPAlgo_BOP>();
  if (aBuilder->Operation() == BOPAlgo_FUSE) { // only for fuse and when old is and argument
    TopoDS_Shape anOld = theOldShape->impl<TopoDS_Shape>();
    bool isOldComp = anOld.ShapeType() == TopAbs_COMPOUND || anOld.ShapeType() == TopAbs_COMPSOLID;
    bool aFound = false;
    TopTools_ListIteratorOfListOfShape anIter(aBuilder->Arguments());
    for(; !aFound && anIter.More(); anIter.Next())
      aFound = anOld.IsSame(anIter.Value()) || (!isOldComp && isInComp(anIter.Value(), anOld));
    for(anIter.Initialize(aBuilder->Tools()); !aFound && anIter.More(); anIter.Next())
      aFound = anOld.IsSame(anIter.Value()) || (!isOldComp && isInComp(anIter.Value(), anOld));
    if (aFound) {
      TopoDS_Shape aResult = aBuilder->Shape();
      TopTools_MapOfShape aNewsMap;
      searchResult(anOld, aResult, aBuilder, aNewsMap);
      if (!aNewsMap.IsEmpty()) {
        for(TopTools_MapIteratorOfMapOfShape aNewsIter(aNewsMap);
            aNewsIter.More(); aNewsIter.Next())
        {
          GeomShapePtr aShape(new GeomAPI_Shape);
          aShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aNewsIter.Value()));
          theNewShapes.push_back(aShape);
        }
        return;
      }
    }
  }
  GeomAlgoAPI_MakeShape::modified(theOldShape, theNewShapes); // default behavior
}
