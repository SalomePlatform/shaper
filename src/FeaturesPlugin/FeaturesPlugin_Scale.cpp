// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Scale.cpp
// Created:     13 Jan 2017
// Author:      Clarisse Genrault (CEA)

#include <FeaturesPlugin_Scale.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

//=================================================================================================
FeaturesPlugin_Scale::FeaturesPlugin_Scale()
{
}

//=================================================================================================
void FeaturesPlugin_Scale::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Scale::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Scale::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_Scale::CENTER_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Scale::SCALE_FACTOR_ID(),
                       ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_Scale::SCALE_FACTOR_X_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Scale::SCALE_FACTOR_Y_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Scale::SCALE_FACTOR_Z_ID(),
                       ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void FeaturesPlugin_Scale::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(FeaturesPlugin_Scale::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  if (aMethodType == FeaturesPlugin_Scale::CREATION_METHOD_BY_FACTOR()) {
    performScaleByFactor();
  }

  if (aMethodType == FeaturesPlugin_Scale::CREATION_METHOD_BY_DIMENSIONS()) {
    performScaleByDimensions();
  }
}

//=================================================================================================
void FeaturesPlugin_Scale::performScaleByFactor()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Scale::OBJECTS_LIST_ID());
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

  // Getting the center point
  std::shared_ptr<GeomAPI_Pnt> aCenterPoint;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_Scale::CENTER_POINT_ID());
  if (anObjRef.get() != NULL) {
    GeomShapePtr aShape = anObjRef->value();
    if (!aShape.get()) {
      aShape = anObjRef->context()->shape();
    }
    if (aShape) {
      aCenterPoint = GeomAlgoAPI_PointBuilder::point(aShape);
    }
  }

  // Getting scale factor
  double aScaleFactor = real(FeaturesPlugin_Scale::SCALE_FACTOR_ID())->value();

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      //aTrsf->setSymmetry(anAxis);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(*aContext, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Scale aScaleAlgo(aBaseShape, aCenterPoint, aScaleFactor);

      if (!aScaleAlgo.check()) {
        setError(aScaleAlgo.getError());
        return;
      }

      aScaleAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aScaleAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Symmetry algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aScaleAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aScaleAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aScaleAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Scale::performScaleByDimensions()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Scale::OBJECTS_LIST_ID());
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

  // Getting the center point
  std::shared_ptr<GeomAPI_Pnt> aCenterPoint;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_Scale::CENTER_POINT_ID());
  if (anObjRef.get() != NULL) {
    GeomShapePtr aShape = anObjRef->value();
    if (!aShape.get()) {
      aShape = anObjRef->context()->shape();
    }
    if (aShape) {
      aCenterPoint = GeomAlgoAPI_PointBuilder::point(aShape);
    }
  }

  // Getting dimensions
  double aScaleFactorX = real(FeaturesPlugin_Scale::SCALE_FACTOR_X_ID())->value();
  double aScaleFactorY = real(FeaturesPlugin_Scale::SCALE_FACTOR_Y_ID())->value();
  double aScaleFactorZ = real(FeaturesPlugin_Scale::SCALE_FACTOR_Z_ID())->value();

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      //aTrsf->setSymmetry(anAxis);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(*aContext, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Scale aScaleAlgo(aBaseShape, aCenterPoint,
                                   aScaleFactorX, aScaleFactorY, aScaleFactorZ);

      if (!aScaleAlgo.check()) {
        setError(aScaleAlgo.getError());
        return;
      }

      aScaleAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aScaleAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Symmetry algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aScaleAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aScaleAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aScaleAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Scale::loadNamingDS(GeomAlgoAPI_Scale& theScaleAlgo,
                                        std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                        std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  // Store and name the result.
  theResultBody->storeModified(theBaseShape, theScaleAlgo.shape());

  // Name the faces
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theScaleAlgo.mapOfSubShapes();
  int aReflectedTag = 1;
  std::string aReflectedName = "Scaled";
  theResultBody->loadAndOrientModifiedShapes(&theScaleAlgo,
                                              theBaseShape, GeomAPI_Shape::FACE,
                                              aReflectedTag, aReflectedName, *aSubShapes.get());
}