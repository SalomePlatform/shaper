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

#include "GeomAPI_ShapeHierarchy.h"

#include "GeomAPI_ShapeIterator.h"

#include <BRep_Builder.hxx>
#include <TopoDS_Shape.hxx>

void GeomAPI_ShapeHierarchy::addObject(const GeomShapePtr& theObject)
{
  myObjects.push_back(theObject);
}

void GeomAPI_ShapeHierarchy::addParent(const GeomShapePtr& theShape,
                                       const GeomShapePtr& theParent)
{
  myParent[theShape] = theParent;

  MapShapeToIndex::iterator aFound = myParentIndices.find(theParent);
  size_t anIndex = myParentIndices.size();
  if (aFound == myParentIndices.end()) {
    myParentIndices[theParent] = anIndex;
    mySubshapes.push_back(ShapeAndSubshapes(theParent, ListOfShape()));
  } else
    anIndex = aFound->second;

  mySubshapes[anIndex].second.push_back(theShape);
}

GeomShapePtr GeomAPI_ShapeHierarchy::parent(const GeomShapePtr& theShape,
                                            bool theMarkProcessed)
{
  MapShapeToParent::const_iterator aFound = myParent.find(theShape);
  GeomShapePtr aParent;
  if (aFound != myParent.end()) {
    aParent = aFound->second;
    if (theMarkProcessed) {
      // mark the parent and all its subs as processed by Boolean algorithm
      markProcessed(aParent);
      const ListOfShape& aSubs = mySubshapes[myParentIndices[aParent]].second;
      for (ListOfShape::const_iterator anIt = aSubs.begin(); anIt != aSubs.end(); ++anIt)
        markProcessed(*anIt);
    }
  }
  return aParent;
}

void GeomAPI_ShapeHierarchy::markProcessed(const GeomShapePtr& theShape)
{
  myProcessedObjects.insert(theShape);
}

void GeomAPI_ShapeHierarchy::markProcessed(const ListOfShape& theShapes)
{
  for (ListOfShape::const_iterator anIt = theShapes.begin(); anIt != theShapes.end(); ++anIt)
    markProcessed(*anIt);
}

void GeomAPI_ShapeHierarchy::objectsByType(
    ListOfShape& theShapesByType,
    ListOfShape& theOtherShapes,
    const GeomAPI_Shape::ShapeType theMinType,
    const GeomAPI_Shape::ShapeType theMaxType) const
{
  if (theMinType > theMaxType)
    return objectsByType(theShapesByType, theOtherShapes, theMaxType, theMinType);

  // no need to select objects if whole range is specified
  if (theMinType == GeomAPI_Shape::COMPOUND && theMaxType == GeomAPI_Shape::SHAPE) {
    theShapesByType.insert(theShapesByType.end(), myObjects.begin(), myObjects.end());
    return;
  }

  for (ListOfShape::const_iterator anIt = myObjects.begin(); anIt != myObjects.end(); ++anIt) {
    GeomAPI_Shape::ShapeType aType = (*anIt)->shapeType();
    if (aType >= theMinType && aType <= theMaxType)
      theShapesByType.push_back(*anIt);
    else
      theOtherShapes.push_back(*anIt);
  }
}


void GeomAPI_ShapeHierarchy::splitCompound(
    const GeomShapePtr& theCompShape,
    ListOfShape& theUsed,
    ListOfShape& theNotUsed) const
{
  theUsed.clear();
  theNotUsed.clear();

  MapShapeToIndex::const_iterator aFoundIndex = myParentIndices.find(theCompShape);
  if (aFoundIndex == myParentIndices.end())
    return; // no such shape

  theUsed = mySubshapes[aFoundIndex->second].second;
  SetOfShape aSubsSet;
  aSubsSet.insert(theUsed.begin(), theUsed.end());

  for (GeomAPI_ShapeIterator anExp(theCompShape); anExp.more(); anExp.next()) {
    GeomShapePtr aCurrent = anExp.current();
    if (aSubsSet.find(aCurrent) == aSubsSet.end())
      theNotUsed.push_back(aCurrent);
  }
}

bool GeomAPI_ShapeHierarchy::empty() const
{
  return myObjects.empty();
}

void GeomAPI_ShapeHierarchy::compoundsOfUnusedObjects(
    ListOfShape& theDestination) const
{
  SetOfShape aUsedObjects = myProcessedObjects;
  aUsedObjects.insert(myObjects.begin(), myObjects.end());

  for (std::vector<ShapeAndSubshapes>::const_iterator anIt = mySubshapes.begin();
       anIt != mySubshapes.end(); ++anIt) {
    MapShapeToParent::const_iterator aParent = myParent.find(anIt->first);
    if ((aParent == myParent.end() || !aParent->second) &&
         anIt->first->shapeType() ==  GeomAPI_Shape::COMPOUND) {
      // this is a top-level compound
      GeomShapePtr aCompound = collectUnusedSubs(anIt->first, aUsedObjects);
      // add to destination non-empty compounds only
      if (aCompound)
        theDestination.push_back(aCompound);
    }
  }
}

static void addSubShape(GeomShapePtr theTarget, GeomShapePtr theSub)
{
  if (!theTarget.get() || !theSub.get())
    return;

  TopoDS_Shape* aShape = theTarget->implPtr<TopoDS_Shape>();
  const TopoDS_Shape& aShapeToAdd = theSub->impl<TopoDS_Shape>();

  static BRep_Builder aBuilder;
  aBuilder.Add(*aShape, aShapeToAdd);
}

GeomShapePtr GeomAPI_ShapeHierarchy::collectUnusedSubs(
    GeomShapePtr theTopLevelCompound,
    const SetOfShape& theUsed) const
{
  GeomShapePtr aResult = theTopLevelCompound->emptyCopied();
  bool isResultEmpty = true;

  for (GeomAPI_ShapeIterator aSub(theTopLevelCompound); aSub.more(); aSub.next()) {
    GeomShapePtr aCurrent = aSub.current();
    if (theUsed.find(aCurrent) != theUsed.end())
      continue; // already used

    MapShapeToIndex::const_iterator aFoundIndex = myParentIndices.find(aCurrent);
    if (aCurrent->shapeType() > GeomAPI_Shape::COMPOUND ||
        aFoundIndex == myParentIndices.end()) {
      bool isAddShape = true;
      // check compsolid is fully unused in the Boolean operation
      if (aCurrent->shapeType() == GeomAPI_Shape::COMPSOLID) {
        for (GeomAPI_ShapeIterator anIt(aCurrent); isAddShape && anIt.more(); anIt.next())
          isAddShape = theUsed.find(anIt.current()) == theUsed.end();
      }

      if (isAddShape) { // low-level shape, add it
        addSubShape(aResult, aCurrent);
        isResultEmpty = false;
      }
    } else {
      GeomShapePtr aCompound = collectUnusedSubs(aCurrent, theUsed);
      if (aCompound) {
        addSubShape(aResult, aCompound);
        isResultEmpty = false;
      }
    }
  }
  return isResultEmpty ? GeomShapePtr() : aResult;
}


GeomAPI_ShapeHierarchy::iterator GeomAPI_ShapeHierarchy::begin()
{
  return iterator(this);
}

GeomAPI_ShapeHierarchy::iterator GeomAPI_ShapeHierarchy::end()
{
  return iterator(this, false);
}

GeomAPI_ShapeHierarchy::iterator::iterator(
    GeomAPI_ShapeHierarchy* theHierarchy, bool isBegin)
  : myHierarchy(theHierarchy)
{
  if (isBegin) {
    myObject = myHierarchy->myObjects.begin();
    skipAlreadyProcessed();
  } else
    myObject = myHierarchy->myObjects.end();
}

void GeomAPI_ShapeHierarchy::iterator::skipAlreadyProcessed()
{
  while (myObject != myHierarchy->myObjects.end() &&
         myHierarchy->myProcessedObjects.find(*myObject) != myHierarchy->myProcessedObjects.end())
    ++myObject;
}

bool GeomAPI_ShapeHierarchy::iterator::operator==(const iterator& theOther) const
{
  return myObject == theOther.myObject;
}

bool GeomAPI_ShapeHierarchy::iterator::operator!=(const iterator& theOther) const
{
  return !operator==(theOther);
}

GeomAPI_ShapeHierarchy::iterator& GeomAPI_ShapeHierarchy::iterator::operator++()
{
  ++myObject;
  skipAlreadyProcessed();
  return *this;
}

GeomAPI_ShapeHierarchy::iterator GeomAPI_ShapeHierarchy::iterator::operator++(int)
{
  iterator aCurrent;
  aCurrent.myHierarchy = myHierarchy;
  aCurrent.myObject = myObject;

  // increase iterator
  operator++();

  return aCurrent;
}

GeomShapePtr GeomAPI_ShapeHierarchy::iterator::operator*() const
{
  myHierarchy->markProcessed(*myObject);
  return *myObject;
}
