// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <FeaturesPlugin_MultiTranslation.h>
#include <FeaturesPlugin_Tools.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Trsf.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

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
    setError("Error: empty selection list");
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
  AttributeSelectionPtr anObjRef = selection(AXIS_FIRST_DIR_ID());
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

  std::shared_ptr<GeomAPI_Ax1> anAxis (new GeomAPI_Ax1(anEdge->line()->location(),
                                                       anEdge->line()->direction()));

  // Getting step.
  double aStep = real(FeaturesPlugin_MultiTranslation::STEP_FIRST_DIR_ID())->value();

  // Getting number of copies.
  int nbCopies =
    integer(FeaturesPlugin_MultiTranslation::NB_COPIES_FIRST_DIR_ID())->value();

  if (nbCopies <=0) {
    std::string aFeatureError = "Multitranslation builder ";
    aFeatureError+=":: the number of copies for the first direction is null or negative.";
    setError(aFeatureError);
    return;
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
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      for (int i=0; i<nbCopies; i++) {
        aTrsf->setTranslation(anAxis, i*aStep);
        ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
        aResultPart->setTrsf(*aContext, aTrsf);
        setResult(aResultPart, aResultIndex);
        aResultIndex++;
      }
    } else {
      std::string anError;
      ListOfShape aListOfShape;
      ListOfMakeShape aMakeShapeList;

      for (int i=0; i<nbCopies; i++) {
        std::shared_ptr<GeomAlgoAPI_Translation> aTranslationAlgo(
          new GeomAlgoAPI_Translation(aBaseShape, anAxis, i*aStep));

        if (!aTranslationAlgo->check()) {
          setError(aTranslationAlgo->getError());
          break;
        }

        aTranslationAlgo->build();

        // Checking that the algorithm worked properly.
        if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(
            aTranslationAlgo, getKind(), anError)) {
          setError(anError);
          break;
        }
        aListOfShape.push_back(aTranslationAlgo->shape());
        aMakeShapeList.push_back(aTranslationAlgo);
      }

      std::shared_ptr<GeomAlgoAPI_MakeShapeList>
          aListOfTranslationAlgo(new GeomAlgoAPI_MakeShapeList(aMakeShapeList));

      std::shared_ptr<GeomAPI_Shape> aCompound =
        GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

      ListOfShape aBaseShapes;
      aBaseShapes.push_back(aBaseShape);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, aBaseShapes, ListOfShape(),
                                               aListOfTranslationAlgo, aCompound, "Translated");

      setResult(aResultBody, aResultIndex);
    }
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
    setError("Error: empty selection list");
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
  AttributeSelectionPtr anObjRef = selection(AXIS_FIRST_DIR_ID());
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

  std::shared_ptr<GeomAPI_Ax1> aFirstAxis(new GeomAPI_Ax1(anEdge->line()->location(),
                                                          anEdge->line()->direction()));

  //Getting axis.
  anObjRef = selection(AXIS_SECOND_DIR_ID());
  aShape = anObjRef->value();
  if (!aShape.get()) {
    if (anObjRef->context().get()) {
      aShape = anObjRef->context()->shape();
    }
  }
  if (!aShape.get()) {
    setError(aSelectionError);
    return;
  }

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

  std::shared_ptr<GeomAPI_Ax1> aSecondAxis(new GeomAPI_Ax1(anEdge->line()->location(),
                                                           anEdge->line()->direction()));

  // Getting step.
  double aFirstStep = real(FeaturesPlugin_MultiTranslation::STEP_FIRST_DIR_ID())->value();
  double aSecondStep = real(FeaturesPlugin_MultiTranslation::STEP_SECOND_DIR_ID())->value();

  // Getting number of copies.
  int aFirstNbCopies =
    integer(FeaturesPlugin_MultiTranslation::NB_COPIES_FIRST_DIR_ID())->value();
  int aSecondNbCopies =
    integer(FeaturesPlugin_MultiTranslation::NB_COPIES_SECOND_DIR_ID())->value();

  if (aFirstNbCopies <=0) {
    std::string aFeatureError = "Multitranslation builder ";
    aFeatureError+=":: the number of copies for the first direction is null or negative.";
    setError(aFeatureError);
    return;
  }

  if (aSecondNbCopies <=0) {
    std::string aFeatureError = "Multitranslation builder ";
    aFeatureError+=":: the number of copies for the second direction is null or negative.";
    setError(aFeatureError);
    return;
  }

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
    bool isPart = aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      for (int j=0; j<aSecondNbCopies; j++) {
        for (int i=0; i<aFirstNbCopies; i++) {
          double dx = i*aFirstStep*x1/norm1+j*aSecondStep*x2/norm2;
          double dy = i*aFirstStep*y1/norm1+j*aSecondStep*y2/norm2;
          double dz = i*aFirstStep*z1/norm1+j*aSecondStep*z2/norm2;
          aTrsf->setTranslation(dx, dy, dz);
          ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
          aResultPart->setTrsf(*aContext, aTrsf);
          setResult(aResultPart, aResultIndex);
          aResultIndex++;
        }
      }
    } else {
      std::string anError;
      ListOfShape aListOfShape;
      ListOfMakeShape aMakeShapeList;

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
          if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(
              aTranslationAlgo, getKind(), anError)) {
            setError(anError);
            break;
          }
          aListOfShape.push_back(aTranslationAlgo->shape());
          aMakeShapeList.push_back(aTranslationAlgo);
        }
      }

      std::shared_ptr<GeomAlgoAPI_MakeShapeList>
          aListOfTranslationAlgo(new GeomAlgoAPI_MakeShapeList(aMakeShapeList));

      std::shared_ptr<GeomAPI_Shape> aCompound =
        GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

      ListOfShape aBaseShapes;
      aBaseShapes.push_back(aBaseShape);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, aBaseShapes, ListOfShape(),
                                               aListOfTranslationAlgo, aCompound, "Translated");

      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}
