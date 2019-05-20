// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_VersionedBoolean.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeCustom.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_UnifySameDomain.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <algorithm>
#include <map>

static void performBoolean(const GeomAlgoAPI_Tools::BOPType theBooleanType,
                           GeomMakeShapePtr& theBooleanAlgo,
                           const ListOfShape& theObjects,
                           const ListOfShape& theTools)
{
  if (theBooleanType == GeomAlgoAPI_Tools::BOOL_PARTITION)
    theBooleanAlgo.reset(new GeomAlgoAPI_Partition(theObjects, theTools));
  else {
    // separate processing of FUSE, if only objects are given
    if (theBooleanType == GeomAlgoAPI_Tools::BOOL_FUSE && theTools.empty()) {
      if (theObjects.front()->shapeType() == GeomAPI_Shape::FACE)
        theBooleanAlgo.reset(new GeomAlgoAPI_UnifySameDomain(theObjects));
      else {
        ListOfShape anObjects = theObjects;
        ListOfShape aTools;
        aTools.splice(aTools.begin(), anObjects, anObjects.begin());
        theBooleanAlgo.reset(new GeomAlgoAPI_Boolean(anObjects, aTools, theBooleanType));
      }
    }
    else
      theBooleanAlgo.reset(new GeomAlgoAPI_Boolean(theObjects, theTools, theBooleanType));
  }
}

//=================================================================================================
void FeaturesPlugin_VersionedBoolean::initVersion(const int theVersion,
                                                  const AttributePtr theObjectsAttr,
                                                  const AttributePtr theToolsAttr)
{
  AttributePtr aVerAttr = data()->addAttribute(VERSION_ID(), ModelAPI_AttributeInteger::typeId());
  aVerAttr->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VERSION_ID());
  if (!integer(VERSION_ID())->isInitialized() &&
      (!theObjectsAttr || !theObjectsAttr->isInitialized()) &&
      (!theToolsAttr || !theToolsAttr->isInitialized())) {
    // this is a newly created feature (not read from file),
    // so, initialize the latest version
    integer(VERSION_ID())->setValue(theVersion);
  }
}

//=================================================================================================
void FeaturesPlugin_VersionedBoolean::parentForShape(const GeomShapePtr& theShape,
                                            const ResultPtr& theContext,
                                            ObjectHierarchy& theShapesHierarchy)
{
  ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(theContext);
  if (aResCompSolidPtr.get()) {
    std::shared_ptr<GeomAPI_Shape> aContextShape = aResCompSolidPtr->shape();
    if (aContextShape->shapeType() <= GeomAPI_Shape::COMPSOLID) {
      theShapesHierarchy.AddParent(theShape, aContextShape);
      parentForShape(aContextShape, aResCompSolidPtr, theShapesHierarchy);
    }
  }
}

bool FeaturesPlugin_VersionedBoolean::processAttribute(const std::string& theAttributeName,
                                              ObjectHierarchy& theObjects,
                                              ListOfShape& thePlanesList)
{
  AttributeSelectionListPtr anObjectsSelList = selectionList(theAttributeName);
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    GeomShapePtr anObject = anObjectAttr->value();
    if (!anObject.get()) {
      // It could be a construction plane.
      ResultPtr aContext = anObjectAttr->context();
      anObject = anObjectAttr->context()->shape();
      if (anObject.get()) {
        thePlanesList.push_back(anObject);
        continue;
      } else
        return false;
    }

    theObjects.AddObject(anObject);

    ResultPtr aContext = anObjectAttr->context();
    parentForShape(anObject, aContext, theObjects);
  }
  return true;
}

//=================================================================================================
bool FeaturesPlugin_VersionedBoolean::processObject(
    const GeomAlgoAPI_Tools::BOPType theBooleanType,
    const GeomShapePtr& theObject,
    const ListOfShape& theTools,
    const ListOfShape& thePlanes,
    int& theResultIndex,
    std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
    ListOfShape& theResultShapesList,
    GeomShapePtr theResultCompound)
{
  ListOfShape aListWithObject;
  aListWithObject.push_back(theObject);
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  std::shared_ptr<GeomAlgoAPI_MakeShape> aBoolAlgo;
  GeomShapePtr aResShape;

  // Resize planes.
  ListOfShape aToolsWithPlanes = theTools;
  ListOfShape aPlanesCopy = thePlanes;
  resizePlanes(aListWithObject, aPlanesCopy, aMakeShapeList);
  aToolsWithPlanes.insert(aToolsWithPlanes.end(), aPlanesCopy.begin(), aPlanesCopy.end());

  if (theBooleanType == GeomAlgoAPI_Tools::BOOL_PARTITION)
    aBoolAlgo.reset(new GeomAlgoAPI_Partition(aListWithObject, aToolsWithPlanes));
  else
    aBoolAlgo.reset(new GeomAlgoAPI_Boolean(aListWithObject,
                                            aToolsWithPlanes,
                                            theBooleanType));

  // Checking that the algorithm worked properly.
  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError)) {
    setError(anError);
    return false;
  }

  aResShape = aBoolAlgo->shape();
  if (aResShape.get() && aResShape->shapeType() == GeomAPI_Shape::COMPOUND) {
    int aSubResultsNb = 0;
    GeomAPI_ShapeIterator anIt(aResShape);
    for (; anIt.more(); anIt.next())
      ++aSubResultsNb;

    if (aSubResultsNb == 1) {
      anIt.init(aResShape);
      if (anIt.more())
        aResShape = anIt.current();
    }
  }

  aMakeShapeList->appendAlgo(aBoolAlgo);

  GeomAPI_ShapeIterator aShapeIt(aResShape);
  if (aShapeIt.more() || aResShape->shapeType() == GeomAPI_Shape::VERTEX) {
    std::shared_ptr<ModelAPI_ResultBody> aResultBody;

    if (theResultCompound) { // store BOP result to the compound
      std::shared_ptr<GeomAlgoAPI_ShapeBuilder> aBuilder(new GeomAlgoAPI_ShapeBuilder);
      aBuilder->add(theResultCompound, aResShape);
      aMakeShapeList->appendAlgo(aBuilder);
    }
    else { // create a separate ResultBody
      aResultBody = document()->createBody(data(), theResultIndex);

      // tools should be added to the list to fulfill the correct history of modification
      aListWithObject.insert(aListWithObject.end(), theTools.begin(), theTools.end());

      ListOfShape aUsedTools = theTools;
      aUsedTools.insert(aUsedTools.end(), thePlanes.begin(), thePlanes.end());

      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               aListWithObject,
                                               aUsedTools,
                                               aMakeShapeList,
                                               aResShape);
      setResult(aResultBody, theResultIndex);
      ++theResultIndex;
    }


    FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
    aRBA.resultBody = aResultBody;
    aRBA.baseShape = theObject;
    aRBA.makeShape = aMakeShapeList;
    theResultBaseAlgoList.push_back(aRBA);
    theResultShapesList.push_back(aResShape);
  }
  return true;
}

//=================================================================================================
bool FeaturesPlugin_VersionedBoolean::processCompsolid(
    const GeomAlgoAPI_Tools::BOPType theBooleanType,
    ObjectHierarchy& theCompsolidHierarchy,
    const GeomShapePtr& theCompsolid,
    const ListOfShape& theTools,
    const ListOfShape& thePlanes,
    int& theResultIndex,
    std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
    ListOfShape& theResultShapesList,
    GeomShapePtr theResultCompound)
{
  ListOfShape aUsedInOperationSolids;
  ListOfShape aNotUsedSolids;
  theCompsolidHierarchy.SplitCompound(theCompsolid, aUsedInOperationSolids, aNotUsedSolids);

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());

  // Resize planes.
  ListOfShape aToolsWithPlanes = theTools;
  ListOfShape aPlanesCopy = thePlanes;
  resizePlanes(aUsedInOperationSolids, aPlanesCopy, aMakeShapeList);
  aToolsWithPlanes.insert(aToolsWithPlanes.end(), aPlanesCopy.begin(), aPlanesCopy.end());

  std::shared_ptr<GeomAlgoAPI_MakeShape> aBoolAlgo;
  performBoolean(theBooleanType, aBoolAlgo, aUsedInOperationSolids, aToolsWithPlanes);

  // Checking that the algorithm worked properly.
  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError)) {
    setError(anError);
    return false;
  }

  aMakeShapeList->appendAlgo(aBoolAlgo);
  GeomShapePtr aResultShape = aBoolAlgo->shape();

  // Add result to not used solids from compsolid.
  if (!aNotUsedSolids.empty()) {
    theCompsolidHierarchy.MarkProcessed(aNotUsedSolids);

    ListOfShape aShapesToAdd = aNotUsedSolids;
    aShapesToAdd.push_back(aBoolAlgo->shape());
    std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
        new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
    if (!aFillerAlgo->isDone()) {
      std::string aFeatureError = "Error: PaveFiller algorithm failed.";
      setError(aFeatureError);
      return false;
    }

    aMakeShapeList->appendAlgo(aFillerAlgo);
    aResultShape = aFillerAlgo->shape();
  }

  GeomAPI_ShapeIterator aShapeIt(aResultShape);
  if (aShapeIt.more() || aResultShape->shapeType() == GeomAPI_Shape::VERTEX)
  {
    std::shared_ptr<ModelAPI_ResultBody> aResultBody;

    if (theResultCompound) { // store BOP result to the compound
      std::shared_ptr<GeomAlgoAPI_ShapeBuilder> aBuilder(new GeomAlgoAPI_ShapeBuilder);
      aBuilder->add(theResultCompound, aResultShape);
      aMakeShapeList->appendAlgo(aBuilder);
    }
    else { // create a separate ResultBody
      aResultBody = document()->createBody(data(), theResultIndex);

      ListOfShape aCompSolidList;
      aCompSolidList.push_back(theCompsolid);
      // tools should be added to the list to fulfill the correct history of modification
      aCompSolidList.insert(aCompSolidList.end(), theTools.begin(), theTools.end());

      ListOfShape aUsedTools = theTools;
      aUsedTools.insert(aUsedTools.end(), thePlanes.begin(), thePlanes.end());

      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               aCompSolidList,
                                               aUsedTools,
                                               aMakeShapeList,
                                               aResultShape);
      setResult(aResultBody, theResultIndex);
      ++theResultIndex;
    }

    FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
    aRBA.resultBody = aResultBody;
    aRBA.baseShape = theCompsolid;
    aRBA.makeShape = aMakeShapeList;
    theResultBaseAlgoList.push_back(aRBA);
    theResultShapesList.push_back(aResultShape);
  }
  return true;
}

//=================================================================================================
bool FeaturesPlugin_VersionedBoolean::processCompound(
    const GeomAlgoAPI_Tools::BOPType theBooleanType,
    ObjectHierarchy& theCompoundHierarchy,
    const GeomShapePtr& theCompound,
    const ListOfShape& theTools,
    int& theResultIndex,
    std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
    ListOfShape& theResultShapesList,
    GeomShapePtr theResultCompound)
{
  ListOfShape aUsedInOperationShapes;
  ListOfShape aNotUsedShapes;
  theCompoundHierarchy.SplitCompound(theCompound, aUsedInOperationShapes, aNotUsedShapes);
  if (theResultCompound) {
    // Not necessary to keep all subs of the current compound,
    // all unused solids are already stored in the result compound.
    aNotUsedShapes.clear();
  }

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  std::shared_ptr<GeomAlgoAPI_MakeShape> aBoolAlgo;
  performBoolean(theBooleanType, aBoolAlgo, aUsedInOperationShapes, theTools);

  // Checking that the algorithm worked properly.
  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError)) {
    setError(anError);
    return false;
  }

  aMakeShapeList->appendAlgo(aBoolAlgo);
  GeomShapePtr aResultShape = aBoolAlgo->shape();

  // Add result to not used shape from compound.
  if (!aNotUsedShapes.empty()) {
    theCompoundHierarchy.MarkProcessed(aNotUsedShapes);

    ListOfShape aShapesForResult = aNotUsedShapes;
    if (aResultShape->shapeType() == GeomAPI_Shape::COMPOUND) {
      for (GeomAPI_ShapeIterator aResultIt(aResultShape); aResultIt.more(); aResultIt.next()) {
        aShapesForResult.push_back(aResultIt.current());
      }
    }
    else {
      aShapesForResult.push_back(aResultShape);
    }

    if (aShapesForResult.size() == 1) {
      aResultShape = aShapesForResult.front();
    }
    else {
      aResultShape = GeomAlgoAPI_CompoundBuilder::compound(aShapesForResult);
    }
  }

  GeomAPI_ShapeIterator aShapeIt(aResultShape);
  if (aShapeIt.more() || aResultShape->shapeType() == GeomAPI_Shape::VERTEX) {
    std::shared_ptr<ModelAPI_ResultBody> aResultBody;

    if (theResultCompound) { // store BOP result to the compound
      std::shared_ptr<GeomAlgoAPI_ShapeBuilder> aBuilder(new GeomAlgoAPI_ShapeBuilder);
      aBuilder->add(theResultCompound, aResultShape);
      aMakeShapeList->appendAlgo(aBuilder);
    }
    else { // create a separate ResultBody
      aResultBody = document()->createBody(data(), theResultIndex);

      ListOfShape aCompoundList;
      aCompoundList.push_back(theCompound);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               aCompoundList,
                                               theTools,
                                               aMakeShapeList,
                                               aResultShape);
      setResult(aResultBody, theResultIndex);
      ++theResultIndex;
    }

    FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
    aRBA.resultBody = aResultBody;
    aRBA.baseShape = theCompound;
    aRBA.makeShape = aMakeShapeList;
    theResultBaseAlgoList.push_back(aRBA);
    theResultShapesList.push_back(aResultShape);
  }
  return true;
}

//==================================================================================================
GeomShapePtr FeaturesPlugin_VersionedBoolean::keepUnusedSubsOfCompound(
    const GeomShapePtr& theResult,
    const ObjectHierarchy& theObjectsHierarchy,
    const ObjectHierarchy& theToolsHierarchy,
    std::shared_ptr<GeomAlgoAPI_MakeShapeList> theMakeShapeList)
{
  ListOfShape aCompounds;
  theObjectsHierarchy.CompoundsOfUnusedObjects(aCompounds);
  theToolsHierarchy.CompoundsOfUnusedObjects(aCompounds);

  GeomShapePtr aResultShape = theResult;
  if (!aCompounds.empty()) {
    aResultShape = aCompounds.front();
    aCompounds.pop_front();

    std::shared_ptr<GeomAlgoAPI_ShapeBuilder> aBuilder(new GeomAlgoAPI_ShapeBuilder);
    for (ListOfShape::iterator anIt = aCompounds.begin(); anIt != aCompounds.end(); ++anIt) {
      for (GeomAPI_ShapeIterator aSub(*anIt); aSub.more(); aSub.next())
        aBuilder->add(aResultShape, aSub.current());
    }

    if (theResult)
      aBuilder->add(aResultShape, theResult);

    theMakeShapeList->appendAlgo(aBuilder);
  }
  return aResultShape;
}

//=================================================================================================
void FeaturesPlugin_VersionedBoolean::resizePlanes(
    const ListOfShape& theObjects,
    ListOfShape& thePlanes,
    std::shared_ptr<GeomAlgoAPI_MakeShapeList>& theMakeShapeList)
{
  if (thePlanes.empty())
    return;

  std::list<std::shared_ptr<GeomAPI_Pnt> > aBoundingPoints =
      GeomAlgoAPI_ShapeTools::getBoundingBox(theObjects, 1.0);

  // Resize planes to fit in bounding box
  for (ListOfShape::iterator anIt = thePlanes.begin(); anIt != thePlanes.end(); ++anIt) {
    GeomShapePtr aPlane = *anIt;
    GeomShapePtr aTool = GeomAlgoAPI_ShapeTools::fitPlaneToBox(aPlane, aBoundingPoints);
    std::shared_ptr<GeomAlgoAPI_MakeShapeCustom> aMkShCustom(new GeomAlgoAPI_MakeShapeCustom);
    aMkShCustom->addModified(aPlane, aTool);
    theMakeShapeList->appendAlgo(aMkShCustom);
    *anIt = aTool;
  }
}

//=================================================================================================
int FeaturesPlugin_VersionedBoolean::version()
{
  AttributeIntegerPtr aVersionAttr = integer(VERSION_ID());
  int aVersion = 0;
  if (aVersionAttr && aVersionAttr->isInitialized())
    aVersion = aVersionAttr->value();
  return aVersion;
}

//=================================================================================================

void FeaturesPlugin_VersionedBoolean::ObjectHierarchy::AddObject(const GeomShapePtr& theObject)
{
  myObjects.push_back(theObject);
}

void FeaturesPlugin_VersionedBoolean::ObjectHierarchy::AddParent(const GeomShapePtr& theShape,
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

GeomShapePtr FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Parent(const GeomShapePtr& theShape,
                                                                      bool theMarkProcessed)
{
  MapShapeToParent::const_iterator aFound = myParent.find(theShape);
  GeomShapePtr aParent;
  if (aFound != myParent.end()) {
    aParent = aFound->second;
    if (theMarkProcessed) {
      // mark the parent and all its subs as processed by Boolean algorithm
      myProcessedObjects.insert(aParent);
      const ListOfShape& aSubs = mySubshapes[myParentIndices[aParent]].second;
      for (ListOfShape::const_iterator anIt = aSubs.begin(); anIt != aSubs.end(); ++anIt)
        myProcessedObjects.insert(*anIt);
    }
  }
  return aParent;
}

void FeaturesPlugin_VersionedBoolean::ObjectHierarchy::MarkProcessed(const GeomShapePtr& theShape)
{
  myProcessedObjects.insert(theShape);
}

void FeaturesPlugin_VersionedBoolean::ObjectHierarchy::MarkProcessed(const ListOfShape& theShapes)
{
  for (ListOfShape::const_iterator anIt = theShapes.begin(); anIt != theShapes.end(); ++anIt)
    MarkProcessed(*anIt);
}

void FeaturesPlugin_VersionedBoolean::ObjectHierarchy::ObjectsByType(
    ListOfShape& theShapesByType,
    ListOfShape& theOtherShapes,
    const GeomAPI_Shape::ShapeType theMinType,
    const GeomAPI_Shape::ShapeType theMaxType) const
{
  if (theMinType > theMaxType)
    return ObjectsByType(theShapesByType, theOtherShapes, theMaxType, theMinType);

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


void FeaturesPlugin_VersionedBoolean::ObjectHierarchy::SplitCompound(
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

bool FeaturesPlugin_VersionedBoolean::ObjectHierarchy::IsEmpty() const
{
  return myObjects.empty();
}

void FeaturesPlugin_VersionedBoolean::ObjectHierarchy::CompoundsOfUnusedObjects(
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

GeomShapePtr FeaturesPlugin_VersionedBoolean::ObjectHierarchy::collectUnusedSubs(
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
        GeomAlgoAPI_ShapeBuilder::add(aResult, aCurrent);
        isResultEmpty = false;
      }
    } else {
      GeomShapePtr aCompound = collectUnusedSubs(aCurrent, theUsed);
      if (aCompound) {
        GeomAlgoAPI_ShapeBuilder::add(aResult, aCompound);
        isResultEmpty = false;
      }
    }
  }
  return isResultEmpty ? GeomShapePtr() : aResult;
}


FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator
FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Begin()
{
  return Iterator(this);
}

FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator
FeaturesPlugin_VersionedBoolean::ObjectHierarchy::End()
{
  return Iterator(this, false);
}

FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator::Iterator(
    FeaturesPlugin_VersionedBoolean::ObjectHierarchy* theHierarchy, bool isBegin)
  : myHierarchy(theHierarchy)
{
  if (isBegin) {
    myObject = myHierarchy->myObjects.begin();
    SkipAlreadyProcessed();
  } else
    myObject = myHierarchy->myObjects.end();
}

void FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator::SkipAlreadyProcessed()
{
  while (myObject != myHierarchy->myObjects.end() &&
         myHierarchy->myProcessedObjects.find(*myObject) != myHierarchy->myProcessedObjects.end())
    ++myObject;
}

bool FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator::operator==(
    const Iterator& theOther) const
{
  return myObject == theOther.myObject;
}

bool FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator::operator!=(
    const Iterator& theOther) const
{
  return !operator==(theOther);
}

FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator&
FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator::operator++()
{
  ++myObject;
  SkipAlreadyProcessed();
  return *this;
}

FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator
FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator::operator++(int)
{
  Iterator aCurrent;
  aCurrent.myHierarchy = myHierarchy;
  aCurrent.myObject = myObject;

  // increase iterator
  operator++();

  return aCurrent;
}

GeomShapePtr FeaturesPlugin_VersionedBoolean::ObjectHierarchy::Iterator::operator*() const
{
  myHierarchy->myProcessedObjects.insert(*myObject);
  return *myObject;
}