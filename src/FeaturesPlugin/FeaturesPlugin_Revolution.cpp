// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Revolution.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_Revolution.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

//=================================================================================================
FeaturesPlugin_Revolution::FeaturesPlugin_Revolution()
{
}

//=================================================================================================
void FeaturesPlugin_Revolution::initAttributes()
{
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // revolution works with faces always
  aSelection->setSelectionType("FACE");

  data()->addAttribute(AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(TO_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FROM_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TO_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FROM_OBJECT_ID());
}

//=================================================================================================
void FeaturesPlugin_Revolution::execute()
{
  // Getting faces.
  ListOfShape aFacesList;
  AttributeSelectionListPtr aFacesSelectionList = selectionList(LIST_ID());
  for(int anIndex = 0; anIndex < aFacesSelectionList->size(); anIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aFaceSel = aFacesSelectionList->value(anIndex);
    ResultPtr aContext = aFaceSel->context();
    std::shared_ptr<GeomAPI_Shape> aContextShape = aContext->shape();
    if(!aContextShape.get()) {
      static const std::string aContextError = "The selection context is bad";
      setError(aContextError);
      break;
    }

    std::shared_ptr<GeomAPI_Shape> aFaceShape = aFaceSel->value();
    int aFacesNum = -1; // this mean that "aFace" is used
    ResultConstructionPtr aConstruction = 
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(!aFaceShape.get()) { // this may be the whole sketch result selected, check and get faces
      if (aConstruction.get()) {
        aFacesNum = aConstruction->facesNum();
      } else {
        static const std::string aFaceError = "Can not find basis for revolution";
        setError(aFaceError);
        break;
      }
    }
    for(int aFaceIndex = 0; aFaceIndex < aFacesNum || aFacesNum == -1; aFaceIndex++) {
      std::shared_ptr<GeomAPI_Shape> aBaseShape;
      if (aFacesNum == -1) {
        aFacesList.push_back(aFaceShape);
        break;
      } else {
        aFaceShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
        aFacesList.push_back(aFaceShape);
      }
    }
  }

  //Getting axis.
  std::shared_ptr<GeomAPI_Ax1> anAxis;
  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(AXIS_OBJECT_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->value()));
  } else if(anObjRef->context() && anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->context()->shape()));
  }
  if(anEdge) {
    anAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(), anEdge->line()->direction()));
  }

  // Getting angles.
  double aToAngle = real(TO_ANGLE_ID())->value();
  double aFromAngle = real(FROM_ANGLE_ID())->value();

  if(string(CREATION_METHOD())->value() == "ByAngles") {
    aToAngle = real(TO_ANGLE_ID())->value();
    aFromAngle =  real(FROM_ANGLE_ID())->value();
  } else {
    aToAngle = real(TO_OFFSET_ID())->value();
    aFromAngle =  real(FROM_OFFSET_ID())->value();
  }

  // Getting bounding planes.
  std::shared_ptr<GeomAPI_Shape> aToShape;
  std::shared_ptr<GeomAPI_Shape> aFromShape;

  if(string(CREATION_METHOD())->value() == "ByPlanesAndOffsets") {
    anObjRef = selection(TO_OBJECT_ID());
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

    GeomAlgoAPI_Revolution aRevolAlgo(aBaseShape, anAxis, aToShape, aToAngle, aFromShape, aFromAngle);
    if(!aRevolAlgo.isDone()) {
      static const std::string aPrismAlgoError = "Revolution algorithm failed";
      setError(aPrismAlgoError);
      aResultIndex = 0;
      break;
    }

    // Check if shape is valid
    if(!aRevolAlgo.shape().get() || aRevolAlgo.shape()->isNull()) {
      static const std::string aShapeError = "Resulting shape is Null";
      setError(aShapeError);
      aResultIndex = 0;
      break;
    }
    if(!aRevolAlgo.isValid()) {
      std::string aPrismAlgoError = "Warning: resulting shape is not valid";
      setError(aPrismAlgoError);
      aResultIndex = 0;
      break;
    }

    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    loadNamingDS(aRevolAlgo, aResultBody, aBaseShape);
    setResult(aResultBody, aResultIndex);
    aResultIndex++;
  }

  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Revolution::loadNamingDS(GeomAlgoAPI_Revolution& theRevolAlgo,
                                             std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                             std::shared_ptr<GeomAPI_Shape> theBasis)
{
  //load result
  theResultBody->storeGenerated(theBasis, theRevolAlgo.shape());

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theRevolAlgo.mapOfShapes();

  //Insert lateral face : Face from Edge
  const std::string aLatName = "LateralFace";
  const int aLatTag = 1;
  theResultBody->loadAndOrientGeneratedShapes(theRevolAlgo.makeShape().get(), theBasis, GeomAPI_Shape::EDGE, aLatTag, aLatName, *aSubShapes);

  //Insert to faces
  const std::string aToName = "ToFace";
  int aToTag = 2;
  const ListOfShape& aToFaces = theRevolAlgo.toFaces();
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
  const ListOfShape& aFromFaces = theRevolAlgo.fromFaces();
  for(ListOfShape::const_iterator anIt = aFromFaces.cbegin(); anIt != aFromFaces.cend(); anIt++) {
    std::shared_ptr<GeomAPI_Shape> aFromFace = *anIt;
    if(aSubShapes->isBound(aFromFace)) {
      aFromFace = aSubShapes->find(aFromFace);
    }
    theResultBody->generated(aFromFace, aFromName, aFromTag++);
  }
}
