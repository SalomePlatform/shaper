// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_MakeShapeList.h"


#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>

//==================================================================================================
GeomAlgoAPI_MakeShapeList::GeomAlgoAPI_MakeShapeList()
: GeomAlgoAPI_MakeShape()
{}

//==================================================================================================
GeomAlgoAPI_MakeShapeList::GeomAlgoAPI_MakeShapeList(const ListOfMakeShape& theMakeShapeList)
: GeomAlgoAPI_MakeShape()
{
  init(theMakeShapeList);
}

//==================================================================================================
void GeomAlgoAPI_MakeShapeList::init(const ListOfMakeShape& theMakeShapeList)
{
  if(myMap.get()) {
    myMap->clear();
  } else {
    myMap.reset(new GeomAPI_DataMapOfShapeShape);
  }

  myListOfMakeShape = theMakeShapeList;

  for(ListOfMakeShape::const_iterator anIt = theMakeShapeList.cbegin();
      anIt != theMakeShapeList.cend(); ++anIt) {
    myMap->merge((*anIt)->mapOfSubShapes());
  }
}

const ListOfMakeShape& GeomAlgoAPI_MakeShapeList::list() const
{
  return myListOfMakeShape;
}


//==================================================================================================
void GeomAlgoAPI_MakeShapeList::appendAlgo(
  const GeomMakeShapePtr theMakeShape)
{
  myListOfMakeShape.push_back(theMakeShape);
  if(!myMap.get()) {
    myMap.reset(new GeomAPI_DataMapOfShapeShape());
  }
  myMap->merge(theMakeShape->mapOfSubShapes());
}

//==================================================================================================
const GeomShapePtr GeomAlgoAPI_MakeShapeList::shape() const
{
  GeomShapePtr aShape = GeomAlgoAPI_MakeShape::shape();
  if(aShape.get() && !aShape->impl<TopoDS_Shape>().IsNull()) {
    return aShape;
  } else if(!myListOfMakeShape.empty()) {
    return myListOfMakeShape.back()->shape();
  }
  return GeomShapePtr();
}

//==================================================================================================
void GeomAlgoAPI_MakeShapeList::generated(const GeomShapePtr theOldShape,
                                          ListOfShape& theNewShapes)
{
  result(theOldShape, theNewShapes);
}

//==================================================================================================
void GeomAlgoAPI_MakeShapeList::modified(const GeomShapePtr theOldShape,
                                         ListOfShape& theNewShapes)
{
  result(theOldShape, theNewShapes);
}

//==================================================================================================
bool GeomAlgoAPI_MakeShapeList::isDeleted(const GeomShapePtr theOldShape)
{
  for (ListOfMakeShape::iterator aBuilderIt = myListOfMakeShape.begin();
       aBuilderIt != myListOfMakeShape.end();
       ++aBuilderIt)
  {
    GeomMakeShapePtr aMakeShape = *aBuilderIt;
    if(aMakeShape->isDeleted(theOldShape)) {
      return true;
    }
  }

  return false;
}

//==================================================================================================
void GeomAlgoAPI_MakeShapeList::result(const GeomShapePtr theOldShape,
                                       ListOfShape& theNewShapes)
{
  if(myListOfMakeShape.empty()) {
    return;
  }

  NCollection_Map<TopoDS_Shape> anAlgoShapes;
  NCollection_Map<TopoDS_Shape> aResultShapesMap;
  NCollection_List<TopoDS_Shape> aResultShapesList;
  anAlgoShapes.Add(theOldShape->impl<TopoDS_Shape>());
  aResultShapesMap.Add(theOldShape->impl<TopoDS_Shape>());
  aResultShapesList.Append(theOldShape->impl<TopoDS_Shape>());

  for(ListOfMakeShape::iterator aBuilderIt = myListOfMakeShape.begin();
      aBuilderIt != myListOfMakeShape.end();
      ++aBuilderIt)
  {
    GeomMakeShapePtr aMakeShape = *aBuilderIt;
    NCollection_Map<TopoDS_Shape> aTempShapes;
    for (NCollection_Map<TopoDS_Shape>::Iterator aShapeIt(anAlgoShapes);
         aShapeIt.More();
         aShapeIt.Next())
    {
      bool hasResults = false;
      bool anArgumentIsInResult = false;
      GeomShapePtr aShape(new GeomAPI_Shape);
      aShape->setImpl(new TopoDS_Shape(aShapeIt.Value()));
      ListOfShape aGeneratedShapes;
      aMakeShape->generated(aShape, aGeneratedShapes);
      for (ListOfShape::const_iterator anIt = aGeneratedShapes.cbegin();
           anIt != aGeneratedShapes.cend();
           ++anIt)
      {
        const TopoDS_Shape& anItShape = (*anIt)->impl<TopoDS_Shape>();
        if (anItShape.IsSame(aShapeIt.Value())) {
          anArgumentIsInResult = true;
          continue;
        }
        aTempShapes.Add(anItShape);
        if(aResultShapesMap.Add(anItShape) == Standard_True) {
          aResultShapesList.Append(anItShape);
        }
        hasResults = true;
      }
      ListOfShape aModifiedShapes;
      aMakeShape->modified(aShape, aModifiedShapes);
      for (ListOfShape::const_iterator anIt = aModifiedShapes.cbegin();
           anIt != aModifiedShapes.cend();
           ++anIt)
      {
        const TopoDS_Shape& anItShape = (*anIt)->impl<TopoDS_Shape>();
        if (anItShape.IsSame(aShapeIt.Value())) {
          anArgumentIsInResult = true;
          continue;
        }
        aTempShapes.Add(anItShape);
        if(aResultShapesMap.Add(anItShape) == Standard_True) {
          aResultShapesList.Append(anItShape);
        }
        hasResults = true;
      }
      if(hasResults && !anArgumentIsInResult) {
        const TopoDS_Shape& aTopoDSShape = aShapeIt.Value();
        if(aResultShapesMap.Remove(aTopoDSShape) == Standard_True) {
          for(NCollection_List<TopoDS_Shape>::Iterator
              aResIt(aResultShapesList); aResIt.More(); aResIt.Next()) {
            if(aTopoDSShape.IsEqual(aResIt.Value())) {
              aResultShapesList.Remove(aResIt);
              break;
            }
          }
        }
      }
    }
    anAlgoShapes.Unite(aTempShapes);
  }

  for (NCollection_List<TopoDS_Shape>::Iterator aShapeIt(aResultShapesList);
       aShapeIt.More();
       aShapeIt.Next())
  {
    GeomShapePtr aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aShapeIt.Value()));
    if (!isValidForHistory(aShape)) continue;
    fixOrientation(aShape);
    theNewShapes.push_back(aShape);
  }
}
