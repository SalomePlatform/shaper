// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Movement.cpp
// Created:     8 June 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_Movement.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

//=================================================================================================
FeaturesPlugin_Movement::FeaturesPlugin_Movement()
{
}

//=================================================================================================
void FeaturesPlugin_Movement::initAttributes()
{
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Movement::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // revolution works with faces always
  aSelection->setSelectionType("SOLID");

  data()->addAttribute(FeaturesPlugin_Movement::AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Movement::DISTANCE_ID(), ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void FeaturesPlugin_Movement::execute()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList = selectionList(FeaturesPlugin_Movement::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr = anObjectsSelList->value(anObjectsIndex);
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
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(FeaturesPlugin_Movement::AXIS_OBJECT_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->value()));
  } else if (anObjRef && !anObjRef->value() && anObjRef->context() && 
             anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->context()->shape()));
  }
  if(anEdge) {
    anAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(), anEdge->line()->direction()));
  }

  // Getting distance.
  double aDistance = real(FeaturesPlugin_Movement::DISTANCE_ID())->value();

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = (*aContext)->groupName() == ModelAPI_ResultPart::group();
    GeomAlgoAPI_Movement aMovementAlgo(aBaseShape, anAxis, aDistance, isPart);

    // Checking that the algorithm worked properly.
    if(!aMovementAlgo.isDone()) {
      static const std::string aFeatureError = "Movement algorithm failed";
      setError(aFeatureError);
      break;
    }
    if(aMovementAlgo.shape()->isNull()) {
      static const std::string aShapeError = "Resulting shape is Null";
      setError(aShapeError);
      break;
    }
    if(!aMovementAlgo.isValid()) {
      std::string aFeatureError = "Warning: resulting shape is not valid";
      setError(aFeatureError);
      break;
    }

    // Setting result.
    if (isPart) {
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);

      ResultPartPtr aCurrentResult;
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter = aResults.begin();
      for(int a = 0; aResIter != aResults.end(); aResIter++, a++)  {
        if (a == aResultIndex) {
          aCurrentResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aResIter);
          break;
        }
      }
      ResultPartPtr aResultPart = document()->copyPart(aCurrentResult, anOrigin, aResultIndex);
      aResultPart->setTrsf(*aContext, aMovementAlgo.transformation());
      setResult(aResultPart);
    } else {
      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      LoadNamingDS(aMovementAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

void FeaturesPlugin_Movement::LoadNamingDS(const GeomAlgoAPI_Movement& theMovementAlgo,
                                           std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                           std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  ModelAPI_BodyBuilder* aResultBuilder = theResultBody->getBodyBuilder();
  // Store result.
  aResultBuilder->storeModified(theBaseShape, theMovementAlgo.shape());

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theMovementAlgo.mapOfShapes();

  int aMovedTag = 1;
  std::string aMovedName = "Moved";
  aResultBuilder->loadAndOrientModifiedShapes(theMovementAlgo.makeShape().get(),
                                              theBaseShape, GeomAPI_Shape::FACE,
                                              aMovedTag, aMovedName, *aSubShapes.get());

}
