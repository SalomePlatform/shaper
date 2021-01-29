// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
#include <GeomAlgoAPI_Translation.h>

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
#include <ModelAPI_Tools.h>

#include <math.h>

static const std::string MULTITRANSLATION_VERSION_1("v9.5");

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

  if (!aSelection->isInitialized()) {
    // new feature, not read from file
    data()->setVersion(MULTITRANSLATION_VERSION_1);
  }
}

//=================================================================================================
void FeaturesPlugin_MultiTranslation::execute()
{
  bool isKeepSubShapes = data()->version() == MULTITRANSLATION_VERSION_1;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    setError("Error: empty selection list");
    return;
  }

  GeomAPI_ShapeHierarchy anObjects;
  std::list<ResultPtr> aParts;
  std::string theTextureFile;
  if (!FeaturesPlugin_Tools::shapesFromSelectionList(
       anObjectsSelList, isKeepSubShapes, anObjects, aParts, theTextureFile))
    return;

  std::shared_ptr<GeomAPI_Dir> aFirstDir, aSecondDir;
  double aFirstStep, aSecondStep;
  int aFirstNbCopies, aSecondNbCopies;
  if (!paramsAlongDirection(0, aFirstDir, aFirstStep, aFirstNbCopies))
    return;

  bool useSecondDir = !string(USE_SECOND_DIR_ID())->value().empty();
  if (useSecondDir) {
    if (!paramsAlongDirection(1, aSecondDir, aSecondStep, aSecondNbCopies))
      return;
  }
  else {
    aSecondDir = aFirstDir; // direction does not matter
    aSecondStep = 0.0;
    aSecondNbCopies = 1;
  }

  std::string anError;
  int aResultIndex = 0;
  // Moving each part.
  for (std::list<ResultPtr>::iterator aPRes = aParts.begin(); aPRes != aParts.end(); ++aPRes) {
    ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aPRes);
    std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
    for (int j = 0; j < aSecondNbCopies; j++) {
      for (int i = 0; i < aFirstNbCopies; i++) {
        double dx = i * aFirstStep * aFirstDir->x() + j * aSecondStep * aSecondDir->x();
        double dy = i * aFirstStep * aFirstDir->y() + j * aSecondStep * aSecondDir->y();
        double dz = i * aFirstStep * aFirstDir->z() + j * aSecondStep * aSecondDir->z();
        aTrsf->setTranslation(dx, dy, dz);

        ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
        aResultPart->setTrsf(anOrigin, aTrsf);
        setResult(aResultPart, aResultIndex);
        aResultIndex++;
      }
    }
  }

  // Collect transformations for each object in a part.
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);
  for (GeomAPI_ShapeHierarchy::iterator anObjectsIt = anObjects.begin();
       anObjectsIt != anObjects.end(); anObjectsIt++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    ListOfShape aListOfShape;

    for (int j = 0; j < aSecondNbCopies; j++) {
      for (int i = 0; i < aFirstNbCopies; i++) {
        double dx = i * aFirstStep * aFirstDir->x() + j * aSecondStep * aSecondDir->x();
        double dy = i * aFirstStep * aFirstDir->y() + j * aSecondStep * aSecondDir->y();
        double dz = i * aFirstStep * aFirstDir->z() + j * aSecondStep * aSecondDir->z();
        std::shared_ptr<GeomAlgoAPI_Translation> aTranslationAlgo(
            new GeomAlgoAPI_Translation(aBaseShape, dx, dy, dz));

        // Checking that the algorithm worked properly.
        if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(
            aTranslationAlgo, getKind(), anError)) {
          setError(anError);
          break;
        }
        aListOfShape.push_back(aTranslationAlgo->shape());
        aMakeShapeList->appendAlgo(aTranslationAlgo);
      }
    }
    GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
    anObjects.markModified(aBaseShape, aCompound);
  }

  // Build results of the operation.
  const ListOfShape& anOriginalShapes = anObjects.objects();
  ListOfShape aTopLevel;
  anObjects.topLevelObjects(aTopLevel);
  for (ListOfShape::iterator anIt = aTopLevel.begin(); anIt != aTopLevel.end(); ++anIt) {
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    ModelAPI_Tools::loadModifiedShapes(aResultBody, anOriginalShapes, ListOfShape(),
                                       aMakeShapeList, *anIt, "Translated");
    aResultBody->setTextureFile(theTextureFile);
    setResult(aResultBody, aResultIndex++);
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
bool FeaturesPlugin_MultiTranslation::paramsAlongDirection(const int theIndex,
                                                           std::shared_ptr<GeomAPI_Dir>& theDir,
                                                           double& theDistance,
                                                           int& theQuantity)
{
  static std::string THE_AXIS_DIR[2] = { AXIS_FIRST_DIR_ID(), AXIS_SECOND_DIR_ID() };
  static std::string THE_STEP[2] = { STEP_FIRST_DIR_ID(), STEP_SECOND_DIR_ID() };
  static std::string THE_COPIES[2] = { NB_COPIES_FIRST_DIR_ID(), NB_COPIES_SECOND_DIR_ID() };
  static std::string THE_INDEX_ID[2] = { "first", "second" };

  //Getting axis.
  static const std::string aSelectionError = "Error: The axis shape selection is bad.";
  AttributeSelectionPtr anObjRef = selection(THE_AXIS_DIR[theIndex]);
  GeomShapePtr aShape = anObjRef->value();
  if (!aShape.get() && anObjRef->context().get())
    aShape = anObjRef->context()->shape();
  if (!aShape.get()) {
    setError(aSelectionError);
    return false;
  }

  GeomEdgePtr anEdge;
  if (aShape->isEdge())
    anEdge = aShape->edge();
  else if (aShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(aShape);
    anEdge = anIt.current()->edge();
  }

  if (!anEdge.get()) {
    setError(aSelectionError);
    return false;
  }

  theDir = anEdge->line()->direction();
  theDistance = real(THE_STEP[theIndex])->value();
  theQuantity = integer(THE_COPIES[theIndex])->value();
  if (theQuantity <= 0) {
    std::string aFeatureError = "Multitranslation builder :: the number of copies for the ";
    aFeatureError += THE_INDEX_ID[theIndex] + " direction is null or negative.";
    setError(aFeatureError);
    return false;
  }
  return true;
}
