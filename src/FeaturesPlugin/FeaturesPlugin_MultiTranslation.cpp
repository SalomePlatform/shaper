// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        FeaturesPlugin_MultiTranslation.cpp
// Created:     30 Jan 2017
// Author:      Clarisse Genrault (CEA)

#include <FeaturesPlugin_MultiTranslation.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>

#include <math.h>

//=================================================================================================
FeaturesPlugin_MultiTranslation::FeaturesPlugin_MultiTranslation()
{
}

//=================================================================================================
void FeaturesPlugin_MultiTranslation::initAttributes()
{
  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_MultiTranslation::OBJECTS_LIST_ID(),
    ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_MultiTranslation::AXIS_FIRST_DIR_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_MultiTranslation::STEP_FIRST_DIR_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_MultiTranslation::NB_COPIES_FIRST_DIR_ID(),
                       ModelAPI_AttributeInteger::typeId());

  data()->addAttribute(FeaturesPlugin_MultiTranslation::USE_SECOND_DIR_ID(),
                       ModelAPI_AttributeString::typeId());
  data()->addAttribute(FeaturesPlugin_MultiTranslation::AXIS_SECOND_DIR_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_MultiTranslation::STEP_SECOND_DIR_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_MultiTranslation::NB_COPIES_SECOND_DIR_ID(),
                       ModelAPI_AttributeInteger::typeId());
}

//=================================================================================================
void FeaturesPlugin_MultiTranslation::execute()
{
  std::string useSecondDir = string(FeaturesPlugin_MultiTranslation::USE_SECOND_DIR_ID())->value();
  if(!useSecondDir.empty()) {
    performTwoDirection();
  } else {
    performOneDirection();
  }
}

//=================================================================================================
void FeaturesPlugin_MultiTranslation::performOneDirection()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_MultiTranslation::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) { // may be for not-activated parts
      eraseResults();
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  //Getting axis.
  std::shared_ptr<GeomAPI_Ax1> anAxis;
  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_MultiTranslation::AXIS_FIRST_DIR_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->value()));
  } else if (anObjRef && !anObjRef->value() && anObjRef->context() &&
             anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->context()->shape()));
  }
  if(anEdge) {
    anAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(),
                                                          anEdge->line()->direction()));
  }

  // Getting step.
  double aStep = real(FeaturesPlugin_MultiTranslation::STEP_FIRST_DIR_ID())->value();

  // Getting number of copies.
  int nbCopies =
    integer(FeaturesPlugin_MultiTranslation::NB_COPIES_FIRST_DIR_ID())->value();

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;

    ListOfShape aListOfShape;
    std::list<std::shared_ptr<GeomAlgoAPI_Translation> > aListOfTranslationAlgo;

    for (int i=0; i<nbCopies; i++) {
      std::shared_ptr<GeomAlgoAPI_Translation> aTranslationAlgo(
        new GeomAlgoAPI_Translation(aBaseShape, anAxis, i*aStep));

      if (!aTranslationAlgo->check()) {
        setError(aTranslationAlgo->getError());
        break;
      }

      aTranslationAlgo->build();

      // Checking that the algorithm worked properly.
      if (!aTranslationAlgo->isDone()) {
        static const std::string aFeatureError = "Error : Multitranslation algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if (aTranslationAlgo->shape()->isNull()) {
        static const std::string aShapeError = "Error : Resulting shape is null.";
        setError(aShapeError);
        break;
      }
      if (!aTranslationAlgo->isValid()) {
        static const std::string aFeatureError = "Error : Resulting shape in not valid.";
        setError(aFeatureError);
        break;
      }
      aListOfShape.push_back(aTranslationAlgo->shape());
      aListOfTranslationAlgo.push_back(aTranslationAlgo);
    }
    std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->storeModified(aBaseShape, aCompound);
    loadNamingDS(aListOfTranslationAlgo, aResultBody, aBaseShape);

    setResult(aResultBody, aResultIndex);
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_MultiTranslation::performTwoDirection()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_MultiTranslation::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) { // may be for not-activated parts
      eraseResults();
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  //Getting axis.
  std::shared_ptr<GeomAPI_Ax1> aFirstAxis;
  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_MultiTranslation::AXIS_FIRST_DIR_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->value()));
  } else if (anObjRef && !anObjRef->value() && anObjRef->context() &&
             anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->context()->shape()));
  }
  if(anEdge) {
    aFirstAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(),
                                                              anEdge->line()->direction()));
  }
  std::shared_ptr<GeomAPI_Ax1> aSecondAxis;
  anObjRef = selection(FeaturesPlugin_MultiTranslation::AXIS_SECOND_DIR_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->value()));
  } else if (anObjRef && !anObjRef->value() && anObjRef->context() &&
             anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->context()->shape()));
  }
  if(anEdge) {
    aSecondAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(),
                                                               anEdge->line()->direction()));
  }

  // Getting step.
  double aFirstStep = real(FeaturesPlugin_MultiTranslation::STEP_FIRST_DIR_ID())->value();
  double aSecondStep = real(FeaturesPlugin_MultiTranslation::STEP_SECOND_DIR_ID())->value();

  // Getting number of copies.
  int aFirstNbCopies =
    integer(FeaturesPlugin_MultiTranslation::NB_COPIES_FIRST_DIR_ID())->value();
  int aSecondNbCopies =
    integer(FeaturesPlugin_MultiTranslation::NB_COPIES_SECOND_DIR_ID())->value();

  // Coord aFirstAxis
  double x1 = aFirstAxis->dir()->x();
  double y1 = aFirstAxis->dir()->y();
  double z1 = aFirstAxis->dir()->z();
  double norm1 = sqrt(x1*x1 + y1*y1 + z1*z1);

  // Coord aSecondAxis
  double x2 = aSecondAxis->dir()->x();
  double y2 = aSecondAxis->dir()->y();
  double z2 = aSecondAxis->dir()->z();
  double norm2 = sqrt(x2*x2 + y2*y2 + z2*z2);

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;

    ListOfShape aListOfShape;
    std::list<std::shared_ptr<GeomAlgoAPI_Translation> > aListOfTranslationAlgo;

    for (int j=0; j<aSecondNbCopies; j++) {
      for (int i=0; i<aFirstNbCopies; i++) {
        double dx = i*aFirstStep*x1/norm1+j*aSecondStep*x2/norm2;
        double dy = i*aFirstStep*y1/norm1+j*aSecondStep*y2/norm2;
        double dz = i*aFirstStep*z1/norm1+j*aSecondStep*z2/norm2;
        std::shared_ptr<GeomAlgoAPI_Translation> aTranslationAlgo(
          new GeomAlgoAPI_Translation(aBaseShape, dx, dy, dz));

        if (!aTranslationAlgo->check()) {
          setError(aTranslationAlgo->getError());
          break;
        }

        aTranslationAlgo->build();

        // Checking that the algorithm worked properly.
        if (!aTranslationAlgo->isDone()) {
          static const std::string aFeatureError = "Error : Multitranslation algorithm failed.";
          setError(aFeatureError);
          break;
        }
        if (aTranslationAlgo->shape()->isNull()) {
          static const std::string aShapeError = "Error : Resulting shape is null.";
          setError(aShapeError);
          break;
        }
        if (!aTranslationAlgo->isValid()) {
          static const std::string aFeatureError = "Error : Resulting shape in not valid.";
          setError(aFeatureError);
          break;
        }
        aListOfShape.push_back(aTranslationAlgo->shape());
        aListOfTranslationAlgo.push_back(aTranslationAlgo);
      }
    }
    std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->storeModified(aBaseShape, aCompound);
    loadNamingDS(aListOfTranslationAlgo, aResultBody, aBaseShape);
    setResult(aResultBody, aResultIndex);
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_MultiTranslation::loadNamingDS(
    std::list<std::shared_ptr<GeomAlgoAPI_Translation> > theListOfTranslationAlgo,
    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
    std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  int aTag = 1;
  int anIndex = 1;
  std::string aTranslatedName;

  for (std::list<std::shared_ptr<GeomAlgoAPI_Translation> >::const_iterator anIt =
    theListOfTranslationAlgo.begin(); anIt != theListOfTranslationAlgo.cend(); ++anIt) {
    std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = (*anIt)->mapOfSubShapes();

    // naming of faces
    aTranslatedName = "Translated_Face_" + std::to_string((long long) anIndex);
    theResultBody->loadAndOrientModifiedShapes((*anIt).get(), theBaseShape, GeomAPI_Shape::FACE,
                                               aTag++, aTranslatedName, *aSubShapes.get(),
                                               false, true);

    // naming of edges
    aTranslatedName = "Translated_Edge_" + std::to_string((long long) anIndex);
    theResultBody->loadAndOrientModifiedShapes((*anIt).get(), theBaseShape, GeomAPI_Shape::EDGE,
                                               aTag++, aTranslatedName, *aSubShapes.get(),
                                               false, true);

    // naming of vertex
    aTranslatedName = "Translated_Vertex_" + std::to_string((long long) anIndex);
    theResultBody->loadAndOrientModifiedShapes((*anIt).get(), theBaseShape, GeomAPI_Shape::VERTEX,
                                               aTag++, aTranslatedName, *aSubShapes.get(),
                                               false, true);

    ++anIndex;
  }
}
