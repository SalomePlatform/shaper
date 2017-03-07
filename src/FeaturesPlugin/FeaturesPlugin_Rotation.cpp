// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Rotation.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_Rotation.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

#include <FeaturesPlugin_Tools.h>

//=================================================================================================
FeaturesPlugin_Rotation::FeaturesPlugin_Rotation()
{
}

//=================================================================================================
void FeaturesPlugin_Rotation::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Rotation::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Rotation::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_Rotation::AXIS_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Rotation::ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_Rotation::CENTER_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Rotation::START_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Rotation::END_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
}

//=================================================================================================
void FeaturesPlugin_Rotation::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(FeaturesPlugin_Rotation::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  if (aMethodType == CREATION_METHOD_BY_ANGLE()) {
    performTranslationByAxisAndAngle();
  }

  if (aMethodType == CREATION_METHOD_BY_THREE_POINTS()) {
    performTranslationByThreePoints();
  }
}

//=================================================================================================
void FeaturesPlugin_Rotation::performTranslationByAxisAndAngle()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Rotation::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  //Getting axis.
  std::shared_ptr<GeomAPI_Ax1> anAxis;
  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_Rotation::AXIS_OBJECT_ID());
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

  // Getting angle.
  double anAngle = real(FeaturesPlugin_Rotation::ANGLE_ID())->value();

  // Rotating each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      aTrsf->setRotation(anAxis, anAngle);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(*aContext, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Rotation aRotationAlgo(aBaseShape, anAxis, anAngle);

      if (!aRotationAlgo.check()) {
        setError(aRotationAlgo.getError());
        return;
      }

      aRotationAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aRotationAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Rotation algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aRotationAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aRotationAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aRotationAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Rotation::performTranslationByThreePoints()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Rotation::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  // Getting the center point and two points (start and end)
  std::shared_ptr<GeomAPI_Pnt> aCenterPoint;
  std::shared_ptr<GeomAPI_Pnt> aStartPoint;
  std::shared_ptr<GeomAPI_Pnt> anEndPoint;
  std::shared_ptr<ModelAPI_AttributeSelection> aCenterRef =
    selection(FeaturesPlugin_Rotation::CENTER_POINT_ID());
  std::shared_ptr<ModelAPI_AttributeSelection> aStartPointRef =
    selection(FeaturesPlugin_Rotation::START_POINT_ID());
  std::shared_ptr<ModelAPI_AttributeSelection> anEndPointRef =
    selection(FeaturesPlugin_Rotation::END_POINT_ID());
  if ((aCenterRef.get() != NULL) && (aStartPointRef.get() != NULL)
      && (anEndPointRef.get() != NULL)) {
    GeomShapePtr aCenterShape = aCenterRef->value();
    if (!aCenterShape.get())
      aCenterShape = aCenterRef->context()->shape();
    GeomShapePtr aStartShape = aStartPointRef->value();
    if (!aStartShape.get())
      aStartShape = aStartPointRef->context()->shape();
      GeomShapePtr anEndShape = anEndPointRef->value();
    if (!anEndShape.get())
      anEndShape = anEndPointRef->context()->shape();
    if (aStartShape && anEndShape && aCenterShape) {
      aCenterPoint = GeomAlgoAPI_PointBuilder::point(aCenterShape);
      aStartPoint = GeomAlgoAPI_PointBuilder::point(aStartShape);
      anEndPoint = GeomAlgoAPI_PointBuilder::point(anEndShape);
    }
  }

  // Rotating each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
       std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
       aTrsf->setRotation(aCenterPoint, aStartPoint, anEndPoint);
       ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
       ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
       aResultPart->setTrsf(*aContext, aTrsf);
       setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Rotation aRotationAlgo(aBaseShape, aCenterPoint, aStartPoint, anEndPoint);

      if (!aRotationAlgo.check()) {
        setError(aRotationAlgo.getError());
        return;
      }

      aRotationAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aRotationAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Rotation algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aRotationAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error : Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aRotationAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aRotationAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }
}

//=================================================================================================
void FeaturesPlugin_Rotation::loadNamingDS(GeomAlgoAPI_Rotation& theRotaionAlgo,
                                           std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                           std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  // Store result.
  theResultBody->storeModified(theBaseShape, theRotaionAlgo.shape());

  std::string aRotatedName = "Rotated";
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theRotaionAlgo.mapOfSubShapes();

  FeaturesPlugin_Tools::storeModifiedShapes(theRotaionAlgo, theResultBody,
                                            theBaseShape, 1, 2, 3, aRotatedName,
                                            *aSubShapes.get());
}
