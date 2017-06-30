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

#include "FeaturesPlugin_Partition.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_MakeShapeCustom.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <iostream>
#include <sstream>

static GeomShapePtr findBase(const GeomShapePtr theObjectShape,
                             const GeomShapePtr theResultShape,
                             const GeomAPI_Shape::ShapeType theShapeType,
                             const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape);

//=================================================================================================
FeaturesPlugin_Partition::FeaturesPlugin_Partition()
{
}

//=================================================================================================
void FeaturesPlugin_Partition::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void FeaturesPlugin_Partition::execute()
{
  ListOfShape anObjects, aPlanes;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(BASE_OBJECTS_ID());
  for(int anIndex = 0; anIndex < anObjectsSelList->size(); ++anIndex) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anIndex);
    GeomShapePtr anObject = anObjectAttr->value();
    if(!anObject.get()) {
      // It could be a construction plane.
      ResultPtr aContext = anObjectAttr->context();
      aPlanes.push_back(anObjectAttr->context()->shape());
    } else {
      anObjects.push_back(anObject);
    }
  }

  if(anObjects.empty()) {
    static const std::string aFeatureError = "Error: No objects for partition.";
    setError(aFeatureError);
    return;
  }

  std::list<std::shared_ptr<GeomAPI_Pnt> > aBoundingPoints =
    GeomAlgoAPI_ShapeTools::getBoundingBox(anObjects, 1.0);

  // Resize planes.
  ListOfShape aTools;
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  for(ListOfShape::const_iterator anIt = aPlanes.cbegin(); anIt != aPlanes.cend(); ++anIt) {
    GeomShapePtr aPlane = *anIt;
    GeomShapePtr aTool = GeomAlgoAPI_ShapeTools::fitPlaneToBox(aPlane, aBoundingPoints);
    std::shared_ptr<GeomAlgoAPI_MakeShapeCustom> aMkShCustom(new GeomAlgoAPI_MakeShapeCustom);
    aMkShCustom->addModified(aPlane, aTool);
    aMakeShapeList->appendAlgo(aMkShCustom);
    aTools.push_back(aTool);
  }

  // Create single result.
  std::shared_ptr<GeomAlgoAPI_Partition> aPartitionAlgo(
    new GeomAlgoAPI_Partition(anObjects, aTools));

  // Checking that the algorithm worked properly.
  if (!aPartitionAlgo->isDone()) {
    static const std::string aFeatureError = "Error: Partition algorithm failed.";
    setError(aFeatureError);
    return;
  }
  if (aPartitionAlgo->shape()->isNull()) {
    static const std::string aShapeError = "Error: Resulting shape is Null.";
    setError(aShapeError);
    return;
  }
  if (!aPartitionAlgo->isValid()) {
    std::string aFeatureError = "Error: Resulting shape is not valid.";
    setError(aFeatureError);
    return;
  }
  aMakeShapeList->appendAlgo(aPartitionAlgo);
  GeomShapePtr aResultShape = aPartitionAlgo->shape();

  int aResultIndex = 0;
  if(aResultShape->shapeType() == GeomAPI_Shape::COMPOUND) {
    for(GeomAPI_ShapeIterator anIt(aResultShape); anIt.more(); anIt.next()) {
      storeResult(anObjects, aPlanes, anIt.current(), aMakeShapeList, aResultIndex);
      ++aResultIndex;
    }
  } else {
    storeResult(anObjects, aPlanes, aResultShape, aMakeShapeList, aResultIndex);
    ++aResultIndex;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Partition::storeResult(
  ListOfShape& theObjects, ListOfShape& thePlanes,
  const GeomShapePtr theResultShape,
  const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
  const int theIndex)
{
  // Find base. The most complicated is the real modified object (#1799 if box is partitioned by
  // two planes the box is the base, not planes, independently on the order in the list).
  GeomShapePtr aBaseShape;
  for(ListOfShape::const_iterator anIt = theObjects.cbegin(); anIt != theObjects.cend(); ++anIt) {
    GeomShapePtr anObjectShape = *anIt;
    GeomShapePtr aCandidate =
      findBase(anObjectShape, theResultShape, GeomAPI_Shape::VERTEX, theMakeShape);
    if(!aCandidate.get()) {
      aCandidate = findBase(anObjectShape, theResultShape, GeomAPI_Shape::EDGE, theMakeShape);
    }
    if (!aCandidate.get())
      aCandidate = findBase(anObjectShape, theResultShape, GeomAPI_Shape::FACE, theMakeShape);

    if(aCandidate.get()) {
      if (!aBaseShape.get() || aBaseShape->shapeType() > aCandidate->shapeType()) {
        aBaseShape = aCandidate;
      }
    }
  }

  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theIndex);

  // Store modified shape.
  if(!aBaseShape.get() || aBaseShape->isEqual(theResultShape)) {
    aResultBody->store(theResultShape);
    setResult(aResultBody, theIndex);
    return;
  }

  const int aDelTag = 1;
  /// sub solids will be placed at labels 3, 4, etc. if result is compound of solids
  const int aSubTag = 2;
  int aModTag = aSubTag + 10000;
  const std::string aModName = "Modified";

  aResultBody->storeModified(aBaseShape, theResultShape, aSubTag);

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfSubShapes = theMakeShape->mapOfSubShapes();
  theObjects.insert(theObjects.end(), thePlanes.begin(), thePlanes.end());
  int anIndex = 1;
  for(ListOfShape::const_iterator anIt = theObjects.cbegin(); anIt != theObjects.cend(); ++anIt) {
    GeomShapePtr aShape = *anIt;
    std::string aModEdgeName = aModName + "_Edge_" + std::to_string((long long)anIndex);
    std::string aModFaceName = aModName + "_Face_" + std::to_string((long long)anIndex++);
    aResultBody->loadAndOrientModifiedShapes(theMakeShape.get(), aShape, GeomAPI_Shape::EDGE,
                                             aModTag, aModEdgeName, *aMapOfSubShapes.get(), true);
    aModTag += 10000;
    aResultBody->loadAndOrientModifiedShapes(theMakeShape.get(), aShape, GeomAPI_Shape::FACE,
                                             aModTag, aModFaceName, *aMapOfSubShapes.get(), true);
    aModTag += 10000;
    aResultBody->loadDeletedShapes(theMakeShape.get(), aShape, GeomAPI_Shape::EDGE, aDelTag);
    aResultBody->loadDeletedShapes(theMakeShape.get(), aShape, GeomAPI_Shape::FACE, aDelTag);
  }

  setResult(aResultBody, theIndex);
}


//=================================================================================================
GeomShapePtr findBase(const GeomShapePtr theObjectShape,
                      const GeomShapePtr theResultShape,
                      const GeomAPI_Shape::ShapeType theShapeType,
                      const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape)
{
  GeomShapePtr aBaseShape;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfSubShapes = theMakeShape->mapOfSubShapes();
  for(GeomAPI_ShapeExplorer anObjectSubShapesExp(theObjectShape, theShapeType);
      anObjectSubShapesExp.more();
      anObjectSubShapesExp.next()) {
    GeomShapePtr anObjectSubShape = anObjectSubShapesExp.current();
    ListOfShape aModifiedShapes;
    theMakeShape->modified(anObjectSubShape, aModifiedShapes);
    for(ListOfShape::const_iterator
        aModIt = aModifiedShapes.cbegin(); aModIt != aModifiedShapes.cend(); ++aModIt) {
      GeomShapePtr aModShape = *aModIt;
      if(aMapOfSubShapes->isBound(aModShape)) {
        aModShape = aMapOfSubShapes->find(aModShape);
      }
      if(theResultShape->isSubShape(aModShape)) {
        aBaseShape = theObjectShape;
        break;
      }
    }
    if(aBaseShape.get()) {
      break;
    }
  }

  return aBaseShape;
}
