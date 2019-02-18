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
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Trsf.h>

#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_Tools.h>

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
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  //Getting axis.
  static const std::string aSelectionError = "Error: The axis shape selection is bad.";
  AttributeSelectionPtr anObjRef = selection(AXIS_OBJECT_ID());
  GeomShapePtr aShape = anObjRef->value();
  if (!aShape.get()) {
    if (anObjRef->context().get()) {
      aShape = anObjRef->context()->shape();
    }
  }
  if (!aShape.get()) {
    setError(aSelectionError);
    return;
  }

  GeomEdgePtr anEdge;
  if (aShape->isEdge())
  {
    anEdge = aShape->edge();
  }
  else if (aShape->isCompound())
  {
    GeomAPI_ShapeIterator anIt(aShape);
    anEdge = anIt.current()->edge();
  }

  if (!anEdge.get())
  {
    setError(aSelectionError);
    return;
  }

  std::shared_ptr<GeomAPI_Ax1> anAxis(new GeomAPI_Ax1(anEdge->line()->location(),
                                                      anEdge->line()->direction()));


  // Getting distance.
  double aDistance = real(FeaturesPlugin_Translation::DISTANCE_ID())->value();

  // Moving each object.
  std::string anError;
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
      std::shared_ptr<GeomAlgoAPI_Translation> aTranslationAlgo(
        new GeomAlgoAPI_Translation(aBaseShape, anAxis, aDistance));

      if (!aTranslationAlgo->check()) {
        setError(aTranslationAlgo->getError());
        return;
      }

      aTranslationAlgo->build();

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aTranslationAlgo, getKind(), anError)) {
        setError(anError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

      ListOfShape aShapes;
      aShapes.push_back(aBaseShape);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               aShapes,
                                               ListOfShape(),
                                               aTranslationAlgo,
                                               aTranslationAlgo->shape(),
                                               "Translated");
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
  std::string anError;
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
      std::shared_ptr<GeomAlgoAPI_Translation> aTranslationAlgo(
        new GeomAlgoAPI_Translation(aBaseShape, aDX, aDY, aDZ));

      if (!aTranslationAlgo->check()) {
        setError(aTranslationAlgo->getError());
        return;
      }

      aTranslationAlgo->build();

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aTranslationAlgo, getKind(), anError)) {
        setError(anError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

      ListOfShape aShapes;
      aShapes.push_back(aBaseShape);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               aShapes,
                                               ListOfShape(),
                                               aTranslationAlgo,
                                               aTranslationAlgo->shape(),
                                               "Translated");
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
  std::string anError;
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
      std::shared_ptr<GeomAlgoAPI_Translation> aTranslationAlgo(
        new GeomAlgoAPI_Translation(aBaseShape, aFirstPoint, aSecondPoint));

      if (!aTranslationAlgo->check()) {
        setError(aTranslationAlgo->getError());
        return;
      }

      aTranslationAlgo->build();

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aTranslationAlgo, getKind(), anError)) {
        setError(anError);
        break;
      }

      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

      ListOfShape aShapes;
      aShapes.push_back(aBaseShape);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               aShapes,
                                               ListOfShape(),
                                               aTranslationAlgo,
                                               aTranslationAlgo->shape(),
                                               "Translated");
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}
