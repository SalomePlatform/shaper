// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Partition.cpp
// Created:     31 Jul 2015
// Author:      Natalia ERMOLAEVA

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

#include <GeomAPI_ShapeIterator.h>

#include <sstream>

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

  std::list<std::shared_ptr<GeomAPI_Pnt> > aBoundingPoints = GeomAlgoAPI_ShapeTools::getBoundingBox(anObjects, 1.0);

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
  std::shared_ptr<GeomAlgoAPI_Partition> aPartitionAlgo(new GeomAlgoAPI_Partition(anObjects, aTools));

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
  anObjects.insert(anObjects.end(), aPlanes.begin(), aPlanes.end());
  if(aResultShape->shapeType() == GeomAPI_Shape::COMPOUND) {
    for(GeomAPI_ShapeIterator anIt(aResultShape); anIt.more(); anIt.next()) {
      storeResult(anObjects, anIt.current(), aMakeShapeList, aResultIndex);
      ++aResultIndex;
    }
  } else {
    storeResult(anObjects, aResultShape, aMakeShapeList, aResultIndex);
    ++aResultIndex;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Partition::storeResult(const ListOfShape& theObjects,
                                           const GeomShapePtr theResultShape,
                                           const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                           const int theIndex)
{
  // Find base.
  GeomShapePtr aBaseShape;
  for(ListOfShape::const_iterator anIt = theObjects.cbegin(); anIt != theObjects.cend(); ++anIt) {
    GeomShapePtr anObjectShape = *anIt;
    ListOfShape aModifiedShapes;
    theMakeShape->modified(anObjectShape, aModifiedShapes);
    for(ListOfShape::const_iterator aModIt = aModifiedShapes.cbegin(); aModIt != aModifiedShapes.cend(); ++aModIt) {
      GeomShapePtr aModShape = *aModIt;
      if(theResultShape->isSubShape(aModShape)) {
        aBaseShape = anObjectShape;
        break;
      }
    }
    if(aBaseShape.get()) {
      break;
    }
  }

  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theIndex);

  // Store modified shape.
  if(aBaseShape->isEqual(theResultShape)) {
    aResultBody->store(theResultShape);
    setResult(aResultBody, theIndex);
    return;
  }

  const int aDelTag = 1;
  const int aSubTag = 2; /// sub solids will be placed at labels 3, 4, etc. if result is compound of solids
  int aModTag = aSubTag + 10000;
  const std::string aModName = "Modified";

  aResultBody->storeModified(aBaseShape, theResultShape, aSubTag);

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfSubShapes = theMakeShape->mapOfSubShapes();
  int anIndex = 1;
  for(ListOfShape::const_iterator anIt = theObjects.cbegin(); anIt != theObjects.cend(); ++anIt) {
    std::ostringstream aStream;
    aStream << aModName << "_" << anIndex++;
    aResultBody->loadAndOrientModifiedShapes(theMakeShape.get(), *anIt, GeomAPI_Shape::EDGE,
                                             aModTag, aStream.str(), *aMapOfSubShapes.get(), true);
    aResultBody->loadAndOrientModifiedShapes(theMakeShape.get(), *anIt, GeomAPI_Shape::FACE,
                                             aModTag, aStream.str(), *aMapOfSubShapes.get(), true);
    aResultBody->loadDeletedShapes(theMakeShape.get(), *anIt, GeomAPI_Shape::EDGE, aDelTag);
    aResultBody->loadDeletedShapes(theMakeShape.get(), *anIt, GeomAPI_Shape::FACE, aDelTag);
    aModTag += 10000;
  }

  setResult(aResultBody, theIndex);
}
