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
void FeaturesPlugin_VersionedBoolean::initVersion(const std::string& theVersion,
                                                  const AttributePtr theObjectsAttr,
                                                  const AttributePtr theToolsAttr)
{
  AttributeIntegerPtr anOldVersionAttr = integer("version");
  if (anOldVersionAttr && anOldVersionAttr->isInitialized() && data()->version().empty()) {
    // move value to the common version interface in ModelAPI_Data
    data()->setVersion(BOP_VERSION_9_4());
  }
  else if ((!theObjectsAttr || !theObjectsAttr->isInitialized()) &&
           (!theToolsAttr || !theToolsAttr->isInitialized())) {
    // this is a newly created feature (not read from file),
    // so, initialize the latest version
    data()->setVersion(theVersion);
  }
}

//=================================================================================================
bool FeaturesPlugin_VersionedBoolean::processAttribute(const std::string& theAttributeName,
                                              GeomAPI_ShapeHierarchy& theObjects,
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

    theObjects.addObject(anObject);

    ResultPtr aContext = anObjectAttr->context();
    ModelAPI_Tools::fillShapeHierarchy(anObject, aContext, theObjects);
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
    GeomAPI_ShapeHierarchy& theCompsolidHierarchy,
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
  theCompsolidHierarchy.splitCompound(theCompsolid, aUsedInOperationSolids, aNotUsedSolids);

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
    theCompsolidHierarchy.markProcessed(aNotUsedSolids);

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
    GeomAPI_ShapeHierarchy& theCompoundHierarchy,
    const GeomShapePtr& theCompound,
    const ListOfShape& theTools,
    int& theResultIndex,
    std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
    ListOfShape& theResultShapesList,
    GeomShapePtr theResultCompound)
{
  ListOfShape aUsedInOperationShapes;
  ListOfShape aNotUsedShapes;
  theCompoundHierarchy.splitCompound(theCompound, aUsedInOperationShapes, aNotUsedShapes);
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
    theCompoundHierarchy.markProcessed(aNotUsedShapes);

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
    const GeomAPI_ShapeHierarchy& theObjectsHierarchy,
    const GeomAPI_ShapeHierarchy& theToolsHierarchy,
    std::shared_ptr<GeomAlgoAPI_MakeShapeList> theMakeShapeList)
{
  ListOfShape aCompounds;
  theObjectsHierarchy.compoundsOfUnusedObjects(aCompounds);
  theToolsHierarchy.compoundsOfUnusedObjects(aCompounds);

  GeomShapePtr aResultShape = theResult;
  if (!aCompounds.empty()) {
    aResultShape = GeomAlgoAPI_CompoundBuilder::compound(aCompounds);
    if (theResult) {
      std::shared_ptr<GeomAlgoAPI_ShapeBuilder> aBuilder(new GeomAlgoAPI_ShapeBuilder);
      aBuilder->add(aResultShape, theResult);
      theMakeShapeList->appendAlgo(aBuilder);
    }
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
