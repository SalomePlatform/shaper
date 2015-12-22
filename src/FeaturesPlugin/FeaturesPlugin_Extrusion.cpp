// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Extrusion.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Extrusion.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_ShapeTools.h>

//=================================================================================================
FeaturesPlugin_Extrusion::FeaturesPlugin_Extrusion()
{
}

//=================================================================================================
void FeaturesPlugin_Extrusion::initAttributes()
{
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("FACE");

  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TO_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FROM_OBJECT_ID());
}

//=================================================================================================
void FeaturesPlugin_Extrusion::execute()
{
  // Getting faces.
  ListOfShape aFacesList;
  AttributeSelectionListPtr aFacesSelectionList = selectionList(LIST_ID());
  for(int anIndex = 0; anIndex < aFacesSelectionList->size(); anIndex++) {
    AttributeSelectionPtr aFaceSel = aFacesSelectionList->value(anIndex);
    std::shared_ptr<GeomAPI_Shape> aFaceShape = aFaceSel->value();
    if(aFaceShape.get() && !aFaceShape->isNull()) { // Getting face.
      aFacesList.push_back(aFaceShape);
    } else { // This may be the whole sketch result selected, check and get faces.
      ResultPtr aContext = aFaceSel->context();
      std::shared_ptr<GeomAPI_Shape> aContextShape = aContext->shape();
      if(!aContextShape.get()) {
        static const std::string aContextError = "The selection context is bad";
        setError(aContextError);
        return;
      }
      ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
      if(!aConstruction.get()) {
        static const std::string aFaceError = "Can not find basis for extrusion";
        setError(aFaceError);
        return;
      }
      int aFacesNum = aConstruction->facesNum();
      for(int aFaceIndex = 0; aFaceIndex < aFacesNum || aFacesNum == -1; aFaceIndex++) {
        aFaceShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
        aFacesList.push_back(aFaceShape);
      }
    }
  }

  // Getting sizes.
  double aToSize = 0.0;
  double aFromSize = 0.0;

  if(string(CREATION_METHOD())->value() == "BySizes") {
    aToSize = real(TO_SIZE_ID())->value();
    aFromSize =  real(FROM_SIZE_ID())->value();
  } else {
    aToSize = real(TO_OFFSET_ID())->value();
    aFromSize =  real(FROM_OFFSET_ID())->value();
  }

  // Getting bounding planes.
  std::shared_ptr<GeomAPI_Shape> aToShape;
  std::shared_ptr<GeomAPI_Shape> aFromShape;

  if(string(CREATION_METHOD())->value() == "ByPlanesAndOffsets") {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(TO_OBJECT_ID());
    if(anObjRef.get() != NULL) {
      aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
      if(aToShape.get() == NULL && anObjRef->context().get() != NULL) {
        aToShape =  anObjRef->context()->shape();
      }
    }
    anObjRef = selection(FROM_OBJECT_ID());
    if(anObjRef.get() != NULL) {
      aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
      if(aFromShape.get() == NULL && anObjRef->context().get() != NULL) {
        aFromShape = anObjRef->context()->shape();
      }
    }
  }

  // Searching faces with common edges.
  ListOfShape aShells;
  ListOfShape aFreeFaces;
  std::shared_ptr<GeomAPI_Shape> aFacesCompound = GeomAlgoAPI_CompoundBuilder::compound(aFacesList);
  GeomAlgoAPI_ShapeTools::combineShapes(aFacesCompound, GeomAPI_Shape::SHELL, aShells, aFreeFaces);
  aShells.insert(aShells.end(), aFreeFaces.begin(), aFreeFaces.end());

  // Generating result for each shell and face.
  int aResultIndex = 0;
  for(ListOfShape::const_iterator anIter = aShells.cbegin(); anIter != aShells.cend(); anIter++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anIter;

    GeomAlgoAPI_Prism aPrismAlgo(aBaseShape, aToShape, aToSize, aFromShape, aFromSize);
    if(!aPrismAlgo.isDone()) {
      static const std::string aPrismAlgoError = "Extrusion algorithm failed";
      setError(aPrismAlgoError);
      aResultIndex = 0;
      break;
    }

    // Check if shape is valid
    if(!aPrismAlgo.shape().get() || aPrismAlgo.shape()->isNull()) {
      static const std::string aShapeError = "Resulting shape is Null";
      setError(aShapeError);
      aResultIndex = 0;
      break;
    }
    if(!aPrismAlgo.isValid()) {
      std::string aPrismAlgoError = "Warning: resulting shape is not valid";
      setError(aPrismAlgoError);
      aResultIndex = 0;
      break;
    }

    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    loadNamingDS(aPrismAlgo, aResultBody, aBaseShape);
    setResult(aResultBody, aResultIndex);
    aResultIndex++;
  }

  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Extrusion::loadNamingDS(GeomAlgoAPI_Prism& thePrismAlgo,
                                            std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                            std::shared_ptr<GeomAPI_Shape> theBasis)
{
  //load result
  theResultBody->storeGenerated(theBasis, thePrismAlgo.shape());

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = thePrismAlgo.mapOfSubShapes();

  //Insert lateral face : Face from Edge
  const std::string aLatName = "LateralFace";
  const int aLatTag = 1;
  theResultBody->loadAndOrientGeneratedShapes(&thePrismAlgo, theBasis, GeomAPI_Shape::EDGE, aLatTag, aLatName, *aSubShapes);

  //Insert to faces
  const std::string aToName = "ToFace";
  int aToTag = 2;
  const ListOfShape& aToFaces = thePrismAlgo.toFaces();
  for(ListOfShape::const_iterator anIt = aToFaces.cbegin(); anIt != aToFaces.cend(); anIt++) {
    std::shared_ptr<GeomAPI_Shape> aToFace = *anIt;
    if(aSubShapes->isBound(aToFace)) {
      aToFace = aSubShapes->find(aToFace);
    }
    theResultBody->generated(aToFace, aToName, aToTag++);
  }

  //Insert from faces
  const std::string aFromName = "FromFace";
  int aFromTag = aToTag > 10000 ? aToTag : 10000;
  const ListOfShape& aFromFaces = thePrismAlgo.fromFaces();
  for(ListOfShape::const_iterator anIt = aFromFaces.cbegin(); anIt != aFromFaces.cend(); anIt++) {
    std::shared_ptr<GeomAPI_Shape> aFromFace = *anIt;
    if(aSubShapes->isBound(aFromFace)) {
      aFromFace = aSubShapes->find(aFromFace);
    }
    theResultBody->generated(aFromFace, aFromName, aFromTag++);
  }
}
