// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <FeaturesPlugin_Translation.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Trsf.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <FeaturesPlugin_Tools.h>

//=================================================================================================
FeaturesPlugin_Translation::FeaturesPlugin_Translation()
{
}

//=================================================================================================
void FeaturesPlugin_Translation::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Translation::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Translation::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_Translation::AXIS_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Translation::DISTANCE_ID(),
                       ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_Translation::DX_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Translation::DY_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Translation::DZ_ID(),
                       ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_Translation::START_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Translation::END_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
}

//=================================================================================================
void FeaturesPlugin_Translation::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(FeaturesPlugin_Translation::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  if (aMethodType == CREATION_METHOD_BY_DISTANCE()) {
    performTranslationByAxisAndDistance();
  }

  if (aMethodType == CREATION_METHOD_BY_DIMENSIONS()) {
    performTranslationByDimensions();
  }

  if (aMethodType == CREATION_METHOD_BY_TWO_POINTS()) {
    performTranslationByTwoPoints();
  }
}

//=================================================================================================
void FeaturesPlugin_Translation::performTranslationByAxisAndDistance()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Translation::OBJECTS_LIST_ID());
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
    selection(FeaturesPlugin_Translation::AXIS_OBJECT_ID());
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

  // Getting distance.
  double aDistance = real(FeaturesPlugin_Translation::DISTANCE_ID())->value();

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      aTrsf->setTranslation(anAxis, aDistance);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(*aContext, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Translation aTranslationAlgo(aBaseShape, anAxis, aDistance);

      if (!aTranslationAlgo.check()) {
        setError(aTranslationAlgo.getError());
        return;
      }

      aTranslationAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aTranslationAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Translation algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aTranslationAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aTranslationAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aTranslationAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Translation::performTranslationByDimensions()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Translation::OBJECTS_LIST_ID());
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

  // Getting dimensions in X, in Y and in Z
  double aDX = real(FeaturesPlugin_Translation::DX_ID())->value();
  double aDY = real(FeaturesPlugin_Translation::DY_ID())->value();
  double aDZ = real(FeaturesPlugin_Translation::DZ_ID())->value();

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      aTrsf->setTranslation(aDX, aDY, aDZ);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(*aContext, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Translation aTranslationAlgo(aBaseShape, aDX, aDY, aDZ);

      if (!aTranslationAlgo.check()) {
        setError(aTranslationAlgo.getError());
        return;
      }

      aTranslationAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aTranslationAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Translation algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aTranslationAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aTranslationAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aTranslationAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Translation::performTranslationByTwoPoints()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Translation::OBJECTS_LIST_ID());
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

  // Getting the start point and the end point
  AttributeSelectionPtr aRef1 = data()->selection(FeaturesPlugin_Translation::START_POINT_ID());
  AttributeSelectionPtr aRef2 = data()->selection(FeaturesPlugin_Translation::END_POINT_ID());
  std::shared_ptr<GeomAPI_Pnt> aFirstPoint;
  std::shared_ptr<GeomAPI_Pnt> aSecondPoint;
  if ((aRef1.get() != NULL) && (aRef2.get() != NULL)) {
    GeomShapePtr aShape1 = aRef1->value();
    if (!aShape1.get()) //If we can't get the points directly, try getting them from the context
      aShape1 = aRef1->context()->shape();
    GeomShapePtr aShape2 = aRef2->value();
    if (!aShape2.get())
      aShape2 = aRef2->context()->shape();
    if (aShape1 && aShape2) {
      aFirstPoint = GeomAlgoAPI_PointBuilder::point(aShape1);
      aSecondPoint = GeomAlgoAPI_PointBuilder::point(aShape2);
    }
  }

  // Moving each object.
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      aTrsf->setTranslation(aFirstPoint, aSecondPoint);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(*aContext, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      GeomAlgoAPI_Translation aTranslationAlgo(aBaseShape, aFirstPoint, aSecondPoint);

      if (!aTranslationAlgo.check()) {
        setError(aTranslationAlgo.getError());
        return;
      }

      aTranslationAlgo.build();

      // Checking that the algorithm worked properly.
      if(!aTranslationAlgo.isDone()) {
        static const std::string aFeatureError = "Error: Translation algorithm failed.";
        setError(aFeatureError);
        break;
      }
      if(aTranslationAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        break;
      }
      if(!aTranslationAlgo.isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aTranslationAlgo, aResultBody, aBaseShape);
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Translation::loadNamingDS(GeomAlgoAPI_Translation& theTranslationAlgo,
                                              std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                              std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  // Store result.
  theResultBody->storeModified(theBaseShape, theTranslationAlgo.shape());

  std::string aTranslatedName = "Translated";
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theTranslationAlgo.mapOfSubShapes();

  FeaturesPlugin_Tools::storeModifiedShapes(theTranslationAlgo, theResultBody,
                                            theBaseShape, 1, 2, 3, aTranslatedName,
                                            *aSubShapes.get());
}
