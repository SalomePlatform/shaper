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

#include "GeomAlgoAPI_MakeShape.h"

#include <BOPAlgo_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Edge.hxx>

// new shape -> old shapes -> index in the old shape
typedef NCollection_DataMap<TopoDS_Shape,
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>, TopTools_ShapeMapHasher>
  MapNewToOld;
typedef
  NCollection_DataMap<int, NCollection_DataMap<TopoDS_Shape, MapNewToOld, TopTools_ShapeMapHasher> >
  HistoryMap;

//==================================================================================================
GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape()
: myBuilderType(Unknown),
  myDone(false)
{
  myHist = 0;
}

//==================================================================================================
GeomAlgoAPI_MakeShape::~GeomAlgoAPI_MakeShape()
{
  if (myHist) {
    delete (HistoryMap*)myHist;
  }
}

//==================================================================================================
bool GeomAlgoAPI_MakeShape::isDone() const
{
  return myDone;
}

//==================================================================================================
const GeomShapePtr GeomAlgoAPI_MakeShape::shape() const
{
  return myShape;
}

//==================================================================================================
bool GeomAlgoAPI_MakeShape::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//==================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_MakeShape::mapOfSubShapes() const
{
  return myMap;
}

//==================================================================================================
void GeomAlgoAPI_MakeShape::generated(const GeomShapePtr theOldShape,
                                      ListOfShape& theNewShapes)
{
  TopTools_ListOfShape aList;
  if(myBuilderType == OCCT_BRepBuilderAPI_MakeShape) {
    BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
    aList = aMakeShape->Generated(theOldShape->impl<TopoDS_Shape>());
  } else if(myBuilderType == OCCT_BOPAlgo_Builder) {
    BOPAlgo_Builder* aBOPBuilder = implPtr<BOPAlgo_Builder>();
    aList = aBOPBuilder->Generated(theOldShape->impl<TopoDS_Shape>());
  }
  for(TopTools_ListIteratorOfListOfShape anIt(aList); anIt.More(); anIt.Next()) {
    GeomShapePtr aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(anIt.Value()));
    if (!isValidForHistory(aShape)) continue;
    fixOrientation(aShape);
    theNewShapes.push_back(aShape);
  }
}

//==================================================================================================
void GeomAlgoAPI_MakeShape::modified(const GeomShapePtr theOldShape,
                                     ListOfShape& theNewShapes)
{
  TopTools_ListOfShape aList;
  if(myBuilderType == OCCT_BRepBuilderAPI_MakeShape) {
    BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
    try {
      aList = aMakeShape->Modified(theOldShape->impl<TopoDS_Shape>());
    } catch(Standard_NoSuchObject) {
    }
  } else if(myBuilderType == OCCT_BOPAlgo_Builder) {
    BOPAlgo_Builder* aBOPBuilder = implPtr<BOPAlgo_Builder>();
    aList = aBOPBuilder->Modified(theOldShape->impl<TopoDS_Shape>());
  }
  for(TopTools_ListIteratorOfListOfShape anIt(aList); anIt.More(); anIt.Next()) {
    GeomShapePtr aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(anIt.Value()));
    if (!isValidForHistory(aShape)) continue;
    fixOrientation(aShape);
    theNewShapes.push_back(aShape);
  }
}

//==================================================================================================
bool GeomAlgoAPI_MakeShape::isDeleted(const GeomShapePtr theOldShape)
{
  bool isDeleted = false;
  if(myBuilderType == OCCT_BRepBuilderAPI_MakeShape) {
    BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
    isDeleted = aMakeShape->IsDeleted(theOldShape->impl<TopoDS_Shape>()) == Standard_True;
  } else if(myBuilderType == OCCT_BOPAlgo_Builder) {
    BOPAlgo_Builder* aBOPBuilder = implPtr<BOPAlgo_Builder>();
    isDeleted = aBOPBuilder->IsDeleted(theOldShape->impl<TopoDS_Shape>()) == Standard_True;
  }

  return isDeleted;
}

//==================================================================================================
void GeomAlgoAPI_MakeShape::setBuilderType(const BuilderType theBuilderType)
{
  myBuilderType = theBuilderType;
}

//==================================================================================================
void GeomAlgoAPI_MakeShape::setDone(const bool theFlag)
{
  myDone = theFlag;
}

//==================================================================================================
void GeomAlgoAPI_MakeShape::setShape(const GeomShapePtr theShape)
{
  if(myShape.get() && myShape->isEqual(theShape)) {
    return;
  }

  myShape = theShape;

  // Filling data map to keep correct orientation of sub-shapes.
  if(myShape.get()) {
    if(myMap.get()) {
      myMap->clear();
    } else {
      myMap.reset(new GeomAPI_DataMapOfShapeShape);
    }

    const TopoDS_Shape& aTopoDSShape = myShape->impl<TopoDS_Shape>();
    for(TopExp_Explorer anExp(aTopoDSShape,TopAbs_VERTEX); anExp.More(); anExp.Next()) {
      GeomShapePtr aCurrentShape(new GeomAPI_Shape());
      aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
      myMap->bind(aCurrentShape, aCurrentShape);
    }
    for(TopExp_Explorer anExp(aTopoDSShape,TopAbs_EDGE); anExp.More(); anExp.Next()) {
      GeomShapePtr aCurrentShape(new GeomAPI_Shape());
      aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
      myMap->bind(aCurrentShape, aCurrentShape);
    }
    for(TopExp_Explorer anExp(aTopoDSShape,TopAbs_FACE); anExp.More(); anExp.Next()) {
      GeomShapePtr aCurrentShape(new GeomAPI_Shape());
      aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
      myMap->bind(aCurrentShape, aCurrentShape);
    }
  } else {
    if(myMap.get()) {
      myMap->clear();
    }
  }
}

//==================================================================================================
bool GeomAlgoAPI_MakeShape::isValidForHistory(const GeomShapePtr theShape)
{
  if (!theShape.get()) return false;

  const TopoDS_Shape& aShape_ = theShape->impl<TopoDS_Shape>();
  if (aShape_.IsNull()) return false;

  if (aShape_.ShapeType() == TopAbs_EDGE) {
    TopoDS_Edge anEdge_ = TopoDS::Edge(aShape_);
    if (BRep_Tool::Degenerated(anEdge_)) return false;
  }

  return true;
}

//==================================================================================================
void GeomAlgoAPI_MakeShape::fixOrientation(GeomShapePtr& theShape) {
  if (myMap->isBound(theShape)) theShape = myMap->find(theShape);
}

//==================================================================================================
void GeomAlgoAPI_MakeShape::initialize()
{
  switch (myBuilderType) {
    case OCCT_BRepBuilderAPI_MakeShape: {
      myDone = implPtr<BRepBuilderAPI_MakeShape>()->IsDone() == Standard_True;
      myShape.reset(new GeomAPI_Shape());
      myShape->setImpl(new TopoDS_Shape(implPtr<BRepBuilderAPI_MakeShape>()->Shape()));
      break;
    }
    case OCCT_BOPAlgo_Builder: {
      myDone = true;
      myShape.reset(new GeomAPI_Shape());
      myShape->setImpl(new TopoDS_Shape(implPtr<BOPAlgo_Builder>()->Shape()));
      break;
    }
  }

  if(myMap.get()) {
    myMap->clear();
  } else {
    myMap.reset(new GeomAPI_DataMapOfShapeShape);
  }

  const TopoDS_Shape& aTopoDSShape = myShape->impl<TopoDS_Shape>();
  for(TopExp_Explorer anExp(aTopoDSShape,TopAbs_FACE); anExp.More(); anExp.Next()) {
    GeomShapePtr aCurrentShape(new GeomAPI_Shape());
    aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
    myMap->bind(aCurrentShape, aCurrentShape);
  }
  myHist = 0;
}


//==================================================================================================
void GeomAlgoAPI_MakeShape::prepareNamingFaces()
{
  long long index = 1;
  GeomAPI_ShapeExplorer anExp(shape(), GeomAPI_Shape::FACE);
  for(GeomAPI_ShapeExplorer anExp(shape(), GeomAPI_Shape::FACE); anExp.more(); anExp.next()) {
    GeomShapePtr aFace = anExp.current();
    myCreatedFaces["Face_" + std::to_string(index++)] = aFace;
  }
}


//==================================================================================================
bool GeomAlgoAPI_MakeShape::checkValid(std::string theMessage)
{
  // isValid() is called from this method
  if (!isValid()) {
    myError = theMessage + " :: resulting shape is not valid.";
    return false;
  }

  // Check the number of volumes in myShape, make sure there's one and only one.
  TopoDS_Shape aTopoDSShape = myShape->impl<TopoDS_Shape>();
  int aNbVolumes = 0;
  for(TopExp_Explorer anExp(aTopoDSShape,TopAbs_SOLID); anExp.More(); anExp.Next()) {
    aNbVolumes ++;
  }

  if (aNbVolumes != 1) {
    myError = theMessage +
      " :: connexity error, the resulting shape is made of several separate solids.";
    return false;
  }

  return true ;
}

//==================================================================================================
bool GeomAlgoAPI_MakeShape::isNewShapesCollected(GeomShapePtr theWholeOld,
                                                 const int theShapeType)
{
  if (!myHist)
    return false;
  HistoryMap* aHist = (HistoryMap*)myHist;
  if (!aHist->IsBound(theShapeType))
    return false;
  return aHist->Find(theShapeType).IsBound(theWholeOld->impl<TopoDS_Shape>());
}

void GeomAlgoAPI_MakeShape::collectNewShapes(
  GeomShapePtr theWholeOld, const int theShapeType)
{
  if (!myHist)
    myHist = new HistoryMap;
  HistoryMap* aHist = (HistoryMap*)myHist;
  if (!aHist->IsBound(theShapeType))
    aHist->Bind(
      theShapeType, NCollection_DataMap<TopoDS_Shape, MapNewToOld, TopTools_ShapeMapHasher>());
  aHist->ChangeFind(theShapeType). // add a new in anyway
    Bind(theWholeOld->impl<TopoDS_Shape>(), MapNewToOld());
  MapNewToOld& aCurrent =
    aHist->ChangeFind(theShapeType).ChangeFind(theWholeOld->impl<TopoDS_Shape>());
  ListOfShape aNewList;
  TopTools_MapOfShape aViewed; //avoid same shapes
  GeomAPI_ShapeExplorer anExp(theWholeOld, GeomAPI_Shape::ShapeType(theShapeType));
  for(int anIndexInWholeOld = 0; anExp.more(); anExp.next(), anIndexInWholeOld++) {
    if (!aViewed.Add(anExp.current()->impl<TopoDS_Shape>()))
      continue;
    aNewList.clear();
    modified(anExp.current(), aNewList);
    for(ListOfShape::iterator aNew = aNewList.begin(); aNew != aNewList.end(); aNew++) {
      const TopoDS_Shape& aNewShape = (*aNew)->impl<TopoDS_Shape>();
      if (!aCurrent.IsBound(aNewShape)) {
        aCurrent.Bind(
          aNewShape, NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>());
      }
      aCurrent.ChangeFind(aNewShape).Bind(anExp.current()->impl<TopoDS_Shape>(), anIndexInWholeOld);
    }
  }
}

static void addAllSubs(const TopoDS_Shape& theNewShape,
  MapNewToOld& theCurrent, std::map<int, TopoDS_Shape>& theResMap)
{
  if (theCurrent.IsBound(theNewShape)) {
    NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>::Iterator
      anOldIter(theCurrent.Find(theNewShape));
    for(; anOldIter.More(); anOldIter.Next()) {
      theResMap[anOldIter.Value()] = anOldIter.Key();
    }
  }

  TopoDS_Iterator anIter(theNewShape);
  for(; anIter.More(); anIter.Next()) {
    //if (anIter.Value().ShapeType() != TopAbs_VERTEX)
    addAllSubs(anIter.Value(), theCurrent, theResMap); // add recursively
  }
}

//==================================================================================================
GeomShapePtr GeomAlgoAPI_MakeShape::oldShapesForNew(GeomShapePtr theWholeOld,
                                                    GeomShapePtr theNewShape,
                                                    const int theShapeType)
{
  GeomShapePtr aResult(new GeomAPI_Shape);
  TopoDS_Compound aResComp;
  TopoDS_Builder aBuilder;
  aBuilder.MakeCompound(aResComp);
  aResult->setImpl<TopoDS_Shape>(new TopoDS_Shape(aResComp));

  HistoryMap* aHist = (HistoryMap*)myHist;
  if (!aHist->IsBound(theShapeType))
    return aResult; // not found, empty compound
  const TopoDS_Shape& aWholeOld = theWholeOld->impl<TopoDS_Shape>();
  if (!aHist->Find(theShapeType).IsBound(aWholeOld))
    return aResult; // not found, empty compound
  std::map<int, TopoDS_Shape> aResMap; // map with all results, ordered by index in whole old
  MapNewToOld& aCurrent = aHist->ChangeFind(theShapeType).ChangeFind(aWholeOld);
  // we don't know what type of new shapes were produced by the old theShapeType, so, iterate all
  addAllSubs(theNewShape->impl<TopoDS_Shape>(), aCurrent, aResMap);

  std::map<int, TopoDS_Shape>::iterator anOldIter = aResMap.begin();
  for(; anOldIter != aResMap.end(); anOldIter++) {
    if (anOldIter->second.ShapeType() == (TopAbs_ShapeEnum)theShapeType)
      aBuilder.Add(aResComp, anOldIter->second);
  }
  aResult->setImpl<TopoDS_Shape>(new TopoDS_Shape(aResComp));
  return aResult;
}
