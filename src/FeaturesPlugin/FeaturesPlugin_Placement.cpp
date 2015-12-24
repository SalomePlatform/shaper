// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Placement.cpp
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#include "FeaturesPlugin_Placement.h"

#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_BodyBuilder.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAlgoAPI_Placement.h>
#include <GeomAlgoAPI_Transform.h>

#define _MODIFIEDF_TAG 1
#define _MODIFIEDE_TAG 2
#define _MODIFIEDV_TAG 3
#define _FACE 4
FeaturesPlugin_Placement::FeaturesPlugin_Placement()
{
}

void FeaturesPlugin_Placement::initAttributes()
{

  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("SOLID");

  data()->addAttribute(START_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(END_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(REVERSE_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(CENTERING_ID(), ModelAPI_AttributeBoolean::typeId());
}

void FeaturesPlugin_Placement::execute()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECTS_LIST_ID());
  if(anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) { // may be for not-activated parts
      eraseResults();
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  // Verify the start shape
  AttributeSelectionPtr anObjRef = selection(START_SHAPE_ID());
  if(!anObjRef) {
    return;
  }
  std::shared_ptr<GeomAPI_Shape> aStartShape = anObjRef->value();
  if(!aStartShape) {
    static const std::string aSelectionError = "The start shape selection is bad";
    setError(aSelectionError);
    return;
  }


  std::shared_ptr<GeomAPI_Shape> aStartShapeContext;
  ResultPtr aContextRes = anObjRef->context();
  if (aContextRes.get()) {
    aStartShapeContext = aContextRes->shape();
  }
  if(!aStartShapeContext.get()) {
    static const std::string aContextError = "The start shape selection context is bad";
    setError(aContextError);
    return;
  }

  // Verify the end shape
  anObjRef = selection(END_SHAPE_ID());
  std::shared_ptr<GeomAPI_Shape> anEndShape = anObjRef->value();
  if(!anEndShape) {
    static const std::string aSelectionError = "The end shape selection is bad";
    setError(aSelectionError);
    return;
  }

  std::shared_ptr<GeomAPI_Shape> anEndShapeContext;
  aContextRes = anObjRef->context();
  if(aContextRes.get()) {
    anEndShapeContext = aContextRes->shape();
  }
  if(!anEndShapeContext.get()) {
    static const std::string aContextError = "The end shape selection context is bad";
    setError(aContextError);
    return;
  }

  // Verify planarity of faces and linearity of edges
  std::shared_ptr<GeomAPI_Shape> aShapes[2] = {aStartShape, anEndShape};
  for (int i = 0; i < 2; i++) {
    if (aShapes[i]->isFace()) {
      std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aShapes[i]));
      if (!aFace->isPlanar()) {
        static const std::string aPlanarityError = "One of selected faces is not planar";
        setError(aPlanarityError);
        return;
      }
    }
    else if (aShapes[i]->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aShapes[i]));
      if (!anEdge->isLine()) {
        static const std::string aLinearityError = "One of selected endges is not linear";
        setError(aLinearityError);
        return;
      }
    }
  }

  // Flags of the Placement
  bool isReverse = boolean(REVERSE_ID())->value();
  bool isCentering = boolean(CENTERING_ID())->value();

  // Getting transformation.
  GeomAlgoAPI_Placement aPlacementAlgo(
    aStartShapeContext, anEndShapeContext, aStartShape, anEndShape, isReverse, isCentering, true);
  if(!aPlacementAlgo.isDone()) {
    static const std::string aFeatureError = "Placement algorithm failed";
    setError(aFeatureError);
    return;
  }
  std::shared_ptr<GeomAPI_Trsf> aTrsf = aPlacementAlgo.transformation();

  // Applying transformation to each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
      anObjectsIt++, aContext++) {

    if ((*aContext)->groupName() == ModelAPI_ResultPart::group()) { // for part results just set transformation
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(aContextRes, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
      GeomAlgoAPI_Transform aTransformAlgo(aBaseShape, aTrsf);

      // Checking that the algorithm worked properly.
      if(!aTransformAlgo.isDone()) {
        static const std::string aFeatureError = "Transform algorithm failed";
        setError(aFeatureError);
        break;
      }
      if(aTransformAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Resulting shape is Null";
        setError(aShapeError);
        break;
      }
      if(!aTransformAlgo.isValid()) {
        std::string aFeatureError = "Warning: resulting shape is not valid";
        setError(aFeatureError);
        break;
      }

      //LoadNamingDS
      std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
      LoadNamingDS(aTransformAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//============================================================================
void FeaturesPlugin_Placement::LoadNamingDS(GeomAlgoAPI_Transform& theTransformAlgo,
                                            std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                            std::shared_ptr<GeomAPI_Shape> theSlaveObject)
{
  //load result
  theResultBody->storeModified(theSlaveObject, theTransformAlgo.shape()); // the initial Slave, the resulting Slave

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theTransformAlgo.mapOfSubShapes();

    // put modifed faces in DF
  std::string aModName = "Modified";
  theResultBody->loadAndOrientModifiedShapes(&theTransformAlgo,
                                             theSlaveObject, _FACE,
                                             _MODIFIEDF_TAG, aModName, *aSubShapes.get());
}
