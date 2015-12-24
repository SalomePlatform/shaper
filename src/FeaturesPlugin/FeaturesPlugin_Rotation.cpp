// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Rotation.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_Rotation.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

//=================================================================================================
FeaturesPlugin_Rotation::FeaturesPlugin_Rotation()
{
}

//=================================================================================================
void FeaturesPlugin_Rotation::initAttributes()
{
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Rotation::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // revolution works with faces always
  aSelection->setSelectionType("SOLID");

  data()->addAttribute(FeaturesPlugin_Rotation::AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Rotation::ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void FeaturesPlugin_Rotation::execute()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList = selectionList(FeaturesPlugin_Rotation::OBJECTS_LIST_ID());
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
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(FeaturesPlugin_Rotation::AXIS_OBJECT_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->value()));
  } else if (anObjRef && !anObjRef->value() && anObjRef->context() && 
             anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->context()->shape()));
  }
  if(anEdge) {
    anAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(), anEdge->line()->direction()));
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
      setResult(aResultPart);
    } else {
      GeomAlgoAPI_Rotation aRotationAlgo(aBaseShape, anAxis, anAngle);

      // Checking that the algorithm worked properly.
      if(!aRotationAlgo.isDone()) {
        static const std::string aFeatureError = "Rotation algorithm failed";
        setError(aFeatureError);
        break;
      }
      if(aRotationAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Resulting shape is Null";
        setError(aShapeError);
        break;
      }
      if(!aRotationAlgo.isValid()) {
        std::string aFeatureError = "Warning: resulting shape is not valid";
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

void FeaturesPlugin_Rotation::loadNamingDS(GeomAlgoAPI_Rotation& theRotaionAlgo,
                                           std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                           std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  // Store result.
  theResultBody->storeModified(theBaseShape, theRotaionAlgo.shape());

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theRotaionAlgo.mapOfSubShapes();

  int aRotatedTag = 1;
  std::string aRotatedName = "Rotated";
  theResultBody->loadAndOrientModifiedShapes(&theRotaionAlgo,
                                             theBaseShape, GeomAPI_Shape::FACE,
                                             aRotatedTag, aRotatedName, *aSubShapes.get());

}
