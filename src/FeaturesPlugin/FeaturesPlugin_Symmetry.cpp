// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Symmetry.cpp
// Created:     30 Nov 2016
// Author:      Clarisse Genrault (CEA)

#include <FeaturesPlugin_Symmetry.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_ResultPart.h>

//=================================================================================================
FeaturesPlugin_Symmetry::FeaturesPlugin_Symmetry()
{
}

//=================================================================================================
void FeaturesPlugin_Symmetry::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Symmetry::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Symmetry::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_Symmetry::POINT_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Symmetry::AXIS_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Symmetry::PLANE_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());
}

//=================================================================================================
void FeaturesPlugin_Symmetry::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(FeaturesPlugin_Symmetry::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  if (aMethodType == CREATION_METHOD_BY_POINT()) {
    performSymmetryByPoint();
  }

  if (aMethodType == CREATION_METHOD_BY_AXIS()) {
    performSymmetryByAxis();
  }

  if (aMethodType == CREATION_METHOD_BY_PLANE()) {
    performSymmetryByPlane();
  }
}

//=================================================================================================
void FeaturesPlugin_Symmetry::performSymmetryByPoint()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Symmetry::OBJECTS_LIST_ID());
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

  //Getting point.
  std::shared_ptr<GeomAPI_Pnt> aPoint;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_Symmetry::POINT_OBJECT_ID());
  if (anObjRef.get() != NULL) {
    GeomShapePtr aShape1 = anObjRef->value();
    if (!aShape1.get()) {
      aShape1 = anObjRef->context()->shape();
    }
    if (aShape1) {
      aPoint = GeomAlgoAPI_PointBuilder::point(aShape1);
    }
  }

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
      aTrsf->setSymmetry(aPoint);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(*aContext, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Symmetry aSymmetryAlgo(aBaseShape, aPoint);

      if (!aSymmetryAlgo.check()) {
        setError(aSymmetryAlgo.getError());
        return;
      }

      aSymmetryAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aSymmetryAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Symmetry algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aSymmetryAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aSymmetryAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aSymmetryAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Symmetry::performSymmetryByAxis()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Symmetry::OBJECTS_LIST_ID());
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
    selection(FeaturesPlugin_Symmetry::AXIS_OBJECT_ID());
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
      aTrsf->setSymmetry(anAxis);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(*aContext, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Symmetry aSymmetryAlgo(aBaseShape, anAxis);

      if (!aSymmetryAlgo.check()) {
        setError(aSymmetryAlgo.getError());
        return;
      }

      aSymmetryAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aSymmetryAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Symmetry algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aSymmetryAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aSymmetryAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aSymmetryAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Symmetry::performSymmetryByPlane()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Symmetry::OBJECTS_LIST_ID());
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
  std::shared_ptr<GeomAPI_Ax2> aPlane;
  std::shared_ptr<GeomAPI_Pln> aPln;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_Symmetry::PLANE_OBJECT_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isFace()) {
    aPln = std::shared_ptr<GeomAPI_Face>(new GeomAPI_Face(anObjRef->value()))->getPlane();
  }
  else if (anObjRef && !anObjRef->value() && anObjRef->context() &&
             anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
    aPln =
      std::shared_ptr<GeomAPI_Face>(new GeomAPI_Face(anObjRef->context()->shape()))->getPlane();
  }
  if(aPln) {
    aPlane = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(aPln->location(),
                                                          aPln->direction()));
  }

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
      aTrsf->setSymmetry(aPlane);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(*aContext, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Symmetry aSymmetryAlgo(aBaseShape, aPlane);

      if (!aSymmetryAlgo.check()) {
        setError(aSymmetryAlgo.getError());
        return;
      }

      aSymmetryAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aSymmetryAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Symmetry algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aSymmetryAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aSymmetryAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aSymmetryAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Symmetry::loadNamingDS(GeomAlgoAPI_Symmetry& theSymmetryAlgo,
                                           std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                           std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  // Store and name the result.
  theResultBody->storeModified(theBaseShape, theSymmetryAlgo.shape());

  // Name the faces
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theSymmetryAlgo.mapOfSubShapes();
  int aReflectedTag = 1;
  std::string aReflectedName = "Symmetried";
  theResultBody->loadAndOrientModifiedShapes(&theSymmetryAlgo,
                                              theBaseShape, GeomAPI_Shape::FACE,
                                              aReflectedTag, aReflectedName, *aSubShapes.get());
}
