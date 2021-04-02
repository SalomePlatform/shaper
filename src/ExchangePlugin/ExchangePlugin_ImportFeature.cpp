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

#include <ExchangePlugin_ImportFeature.h>

#include <algorithm>
#include <string>

#include <Config_Common.h>
#include <Config_PropManager.h>

#include <GeomAlgoAPI_BREPImport.h>
#include <GeomAlgoAPI_IGESImport.h>
#include <GeomAlgoAPI_STEPImport.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_XAOImport.h>
#include <GeomAlgoAPI_STLImport.h>
#include <GeomAlgoAPI_ImageImport.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <Locale_Convert.h>

#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeTables.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <XAO_Xao.hxx>
#include <XAO_Group.hxx>
#include <XAO_Field.hxx>
#include <XAO_Step.hxx>

#include <ExchangePlugin_Tools.h>


/*
 * Request for initialization of data model of the feature: adding all attributes
 */
void ExchangePlugin_ImportFeatureBase::initAttributes()
{
  data()->addAttribute(ExchangePlugin_ImportFeatureBase::FILE_PATH_ID(),
                       ModelAPI_AttributeString::typeId());
  AttributePtr aFeaturesAttribute =
    data()->addAttribute(ExchangePlugin_ImportFeatureBase::FEATURES_ID(),
                         ModelAPI_AttributeRefList::typeId());
  aFeaturesAttribute->setIsArgument(false);

  ModelAPI_Session::get()->validators()->registerNotObligatory(
      getKind(), ExchangePlugin_ImportFeatureBase::FEATURES_ID());
}

void ExchangePlugin_ImportFeature::initAttributes()
{
  ExchangePlugin_ImportFeatureBase::initAttributes();

  data()->addAttribute(STEP_FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(IMPORT_TYPE_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(STEP_MATERIALS_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(STEP_COLORS_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(STEP_SCALE_INTER_UNITS_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(
      getKind(), ExchangePlugin_ImportFeature::STEP_COLORS_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
      getKind(), ExchangePlugin_ImportFeature::STEP_MATERIALS_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
      getKind(), ExchangePlugin_ImportFeature::STEP_SCALE_INTER_UNITS_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
      getKind(), ExchangePlugin_ImportFeature::STEP_FILE_PATH_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
      getKind(), ExchangePlugin_ImportFeature::FILE_PATH_ID());
}
/*
 * Computes or recomputes the results
 */
void ExchangePlugin_ImportFeature::execute()
{
  AttributeStringPtr aImportTypeAttr = string(ExchangePlugin_ImportFeature::IMPORT_TYPE_ID());
  std::string aFormat = aImportTypeAttr->value();
  AttributeStringPtr aFilePathAttr;
  if (aFormat == "STEP" || aFormat == "STP")
  {
    aFilePathAttr = string(ExchangePlugin_ImportFeature::STEP_FILE_PATH_ID());
  } else {
    aFilePathAttr = string(ExchangePlugin_ImportFeature::FILE_PATH_ID());
  }
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty()) {
    setError("File path is empty.");
    return;
  }

  importFile(aFilePath);
}

void ExchangePlugin_Import_ImageFeature::execute()
{
  AttributeStringPtr aFilePathAttr = string(ExchangePlugin_Import_ImageFeature::FILE_PATH_ID());
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty()) {
    setError("File path is empty.");
    return;
  }
  importFile(aFilePath);
}

std::shared_ptr<ModelAPI_ResultBody> ExchangePlugin_ImportFeatureBase::createResultBody(
    std::shared_ptr<GeomAPI_Shape> aGeomShape)
{
  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
  //LoadNamingDS of the imported shape
  loadNamingDS(aGeomShape, aResultBody);
  return aResultBody;
}

void ExchangePlugin_ImportFeature::importFile(const std::string& theFileName)
{
  // "*.brep" -> "BREP"
  std::string anExtension = GeomAlgoAPI_Tools::File_Tools::extension(theFileName);

  if (anExtension == "XAO") {
    importXAO(theFileName);
    return;
  }

  // Perform the import
  std::string anError;
  std::shared_ptr<GeomAPI_Shape> aGeomShape;
  std::map<std::wstring, std::list<std::wstring>> theMaterialShape;

  std::string anObjectName = GeomAlgoAPI_Tools::File_Tools::name(theFileName);
  data()->setName(Locale::Convert::toWString(anObjectName));

  ResultBodyPtr aResult = document()->createBody(data());

  bool anColorGroupSelected = boolean(ExchangePlugin_ImportFeature::STEP_COLORS_ID())->value();
  bool anMaterialsGroupSelected =
                        boolean(ExchangePlugin_ImportFeature::STEP_MATERIALS_ID())->value();
  if (anExtension == "BREP" || anExtension == "BRP") {
    aGeomShape = BREPImport(theFileName, anExtension, anError);
  } else if (anExtension == "STEP" || anExtension == "STP") {
    bool anScalInterUnits =
            boolean(ExchangePlugin_ImportFeature::STEP_SCALE_INTER_UNITS_ID())->value();

    // Process groups/fields
    std::shared_ptr<ModelAPI_AttributeRefList> aRefListOfGroups =
    std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(data()->attribute(FEATURES_ID()));

    // Remove previous groups/fields stored in RefList
    std::list<ObjectPtr> anGroupList = aRefListOfGroups->list();
    std::list<ObjectPtr>::iterator anGroupIt = anGroupList.begin();
    for (; anGroupIt != anGroupList.end(); ++anGroupIt) {
      std::shared_ptr<ModelAPI_Feature> aFeature = ModelAPI_Feature::feature(*anGroupIt);
      if (aFeature)
        document()->removeFeature(aFeature);
    }

    aGeomShape = STEPImportAttributs(theFileName, aResult, anScalInterUnits,
                                     anMaterialsGroupSelected, anColorGroupSelected,
                                     theMaterialShape, anError);
  } else if (anExtension == "IGES" || anExtension == "IGS") {
    aGeomShape = IGESImport(theFileName, anExtension, anError);
  } else if (anExtension == "STL") {
    aGeomShape = STLImport(theFileName, anError);
  }  else {
    anError = "Unsupported format: " + anExtension;
  }

  // Check if shape is valid
  if (!anError.empty()) {
    setError("An error occurred while importing " + theFileName + ": " + anError);
    return;
  }

  // Pass the results into the model
  loadNamingDS(aGeomShape, aResult);

  // create color group
  if (anColorGroupSelected)
  {
    setColorGroups(aResult);
  }

  // create Materiel group
  if (anMaterialsGroupSelected){
    setMaterielGroup(aResult,theMaterialShape);
  }

  setResult(aResult);
  aResult->clearShapeNameAndColor();

}

void ExchangePlugin_ImportFeature::setColorGroups(
                                    std::shared_ptr<ModelAPI_ResultBody> theResultBody)
{
  std::vector<int> aColor;
  int anIndice = 1;
  std::list<std::vector<int>> aColorsRead;

  ModelAPI_Tools::getColor(theResultBody, aColor);
  if (!aColor.empty() ){
    std::wstringstream aColorName;
    aColorName <<L"Color_"<< anIndice;
    setColorGroup(theResultBody, aColor, aColorName.str());
    anIndice++;
    aColorsRead.push_back(aColor);
  }

  std::list<ResultPtr> allRes;
  ModelAPI_Tools::allSubs(theResultBody, allRes);
  for(std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); ++aRes) {
    ModelAPI_Tools::getColor(*aRes, aColor);
    if (!aColor.empty() ){
      auto it = std::find(aColorsRead.begin(), aColorsRead.end(), aColor);
      if ( it == aColorsRead.end() ){
        std::wstringstream aColorName;
        aColorName<<L"Color_"<< anIndice;
        setColorGroup(theResultBody, aColor, aColorName.str());
        anIndice++;
        aColorsRead.push_back(aColor);
      }
    }
  }
}

void ExchangePlugin_ImportFeature::setColorGroup(
                                        std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                        std::vector<int> &theColor,
                                        const std::wstring& theName )
{
  std::vector<int> aColor;
  std::shared_ptr<ModelAPI_Feature> aGroupFeature = addFeature("Group");

   // group name
  aGroupFeature->data()->setName(theName);

  // fill selection
  AttributeSelectionListPtr aSelectionList = aGroupFeature->selectionList("group_list");

  ModelAPI_Tools::getColor(theResultBody, aColor);
  if (!aColor.empty()){
    if (aColor == theColor) {
      GeomShapePtr aShape = theResultBody->shape();
      aSelectionList->setSelectionType(aShape->shapeTypeStr());
      aSelectionList->append(theResultBody,aShape);
    }
  }
  // add element with the same color
  std::list<ResultPtr> allRes;
  ModelAPI_Tools::allSubs(theResultBody, allRes);
  for(std::list<ResultPtr>::iterator aRes = allRes.begin();
      aRes != allRes.end(); ++aRes) {
    ModelAPI_Tools::getColor(*aRes, aColor);
    GeomShapePtr aShape = (*aRes)->shape();

    if (!aColor.empty()){
      if (aRes->get() &&  aColor == theColor) {
        if (aShape->isCompound() || aShape->isCompSolid()) {
          GeomAPI_ShapeIterator anIt(aShape);
          for (; anIt.more(); anIt.next()) {
            aSelectionList->setSelectionType(anIt.current()->shapeTypeStr());
            aSelectionList->append(theResultBody,anIt.current());
          }
        } else {
          aSelectionList->setSelectionType(aShape->shapeTypeStr());
          aSelectionList->append(theResultBody,aShape);
        }
      }
    }
  }

  // Create the group in the document to be able to set its color
  ResultPtr aGroup = document()->createGroup(aGroupFeature->data());
  aGroupFeature->setResult(aGroup);

  ModelAPI_Tools::setColor(aGroupFeature->lastResult(),theColor);

  if (aSelectionList->size() == 0) {
    document()->removeFeature(aGroupFeature);
  }
}

void ExchangePlugin_ImportFeature::setMaterielGroup(
                                std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                std::map< std::wstring,std::list<std::wstring>>& theMaterialShape)
{
  std::map< std::wstring, std::list<std::wstring>>::iterator anIt;
  for (anIt = theMaterialShape.begin(); anIt != theMaterialShape.end(); ++anIt) {

    std::shared_ptr<ModelAPI_Feature> aGroupFeature = addFeature("Group");
    // group name
    aGroupFeature->data()->setName((*anIt).first);

    // fill selection
    AttributeSelectionListPtr aSelectionList = aGroupFeature->selectionList("group_list");

    std::list<ResultPtr> allRes;
    ModelAPI_Tools::allSubs(theResultBody, allRes);
    for (std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); ++aRes) {

      GeomShapePtr aShape = (*aRes)->shape();
      for (std::list<std::wstring>::iterator aResMat = anIt->second.begin();
                                 aResMat != anIt->second.end(); ++aResMat) {
        if (aRes->get() && ((*aRes)->data()->name() == (*aResMat)))
        {
          if (aShape->isCompound() || aShape->isCompSolid()) {
            GeomAPI_ShapeIterator anIt(aShape);
            for (; anIt.more(); anIt.next()) {
              aSelectionList->setSelectionType(anIt.current()->shapeTypeStr());
              aSelectionList->append(theResultBody,anIt.current());
            }
          } else {
            aSelectionList->setSelectionType(aShape->shapeTypeStr());
            aSelectionList->append(theResultBody,aShape);
          }
          break;
        }
      }
    }
    if (aSelectionList->size() == 0){
      document()->removeFeature(aGroupFeature);
    }
  }
}

void ExchangePlugin_ImportFeature::importXAO(const std::string& theFileName)
{
  try {
  std::string anError;

  XAO::Xao aXao;
  std::shared_ptr<GeomAPI_Shape> aGeomShape = XAOImport(theFileName, anError, &aXao);

  if (!anError.empty()) {
    setError("An error occurred while importing " + theFileName + ": " + anError);
    return;
  }

  XAO::Geometry* aXaoGeometry = aXao.getGeometry();

  // use the geometry name or the file name for the feature
  std::string aBodyName = aXaoGeometry->getName();
  if (aBodyName.empty())
    aBodyName = GeomAlgoAPI_Tools::File_Tools::name(theFileName);
  data()->setName(Locale::Convert::toWString(aBodyName));

  ResultBodyPtr aResultBody = createResultBody(aGeomShape);
  setResult(aResultBody);

  // Process groups/fields
  std::shared_ptr<ModelAPI_AttributeRefList> aRefListOfGroups =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(data()->attribute(FEATURES_ID()));

  // Remove previous groups/fields stored in RefList
  std::list<ObjectPtr> anGroupList = aRefListOfGroups->list();
  std::list<ObjectPtr>::iterator anGroupIt = anGroupList.begin();
  for (; anGroupIt != anGroupList.end(); ++anGroupIt) {
    std::shared_ptr<ModelAPI_Feature> aFeature = ModelAPI_Feature::feature(*anGroupIt);
    if (aFeature)
      document()->removeFeature(aFeature);
  }

  // Create new groups
  for (int aGroupIndex = 0; aGroupIndex < aXao.countGroups(); ++aGroupIndex) {
    XAO::Group* aXaoGroup = aXao.getGroup(aGroupIndex);

    std::shared_ptr<ModelAPI_Feature> aGroupFeature = addFeature("Group");

    // group name
    if (!aXaoGroup->getName().empty())
      aGroupFeature->data()->setName(Locale::Convert::toWString(aXaoGroup->getName()));

    // fill selection
    AttributeSelectionListPtr aSelectionList = aGroupFeature->selectionList("group_list");

    // conversion of dimension
    std::string aDimensionString = XAO::XaoUtils::dimensionToString(aXaoGroup->getDimension());
    std::string aSelectionType =
      ExchangePlugin_Tools::xaoDimension2selectionType(aDimensionString);

    aSelectionList->setSelectionType(aSelectionType);
    for (int anElementIndex = 0; anElementIndex < aXaoGroup->count(); ++anElementIndex) {
      aSelectionList->append(aResultBody, GeomShapePtr());
      // complex conversion of element index to reference id
      int anElementID = aXaoGroup->get(anElementIndex);
      std::string aReferenceString =
        aXaoGeometry->getElementReference(aXaoGroup->getDimension(), anElementID);
      int aReferenceID = XAO::XaoUtils::stringToInt(aReferenceString);

      aSelectionList->value(anElementIndex)->setId(aReferenceID);
    }
  }
  // Create new fields
  for (int aFieldIndex = 0; aFieldIndex < aXao.countFields(); ++aFieldIndex) {
    XAO::Field* aXaoField = aXao.getField(aFieldIndex);

    std::shared_ptr<ModelAPI_Feature> aFieldFeature = addFeature("Field");

    // group name
    if (!aXaoField->getName().empty())
      aFieldFeature->data()->setName(Locale::Convert::toWString(aXaoField->getName()));

    // fill selection
    AttributeSelectionListPtr aSelectionList = aFieldFeature->selectionList("selected");

    // conversion of dimension
    std::string aDimensionString = XAO::XaoUtils::dimensionToString(aXaoField->getDimension());
    std::string aSelectionType =
      ExchangePlugin_Tools::xaoDimension2selectionType(aDimensionString);
    aSelectionList->setSelectionType(aSelectionType);
    // limitation: now in XAO fields are related to everything, so, iterate all sub-shapes to fill
    int aCountSelected = aXaoField->countElements();
    std::list<ResultPtr>::const_iterator aResIter = results().begin();
    for(; aResIter != results().end() && aCountSelected; aResIter++) {
      ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aResIter);
      if (!aBody.get())
        continue;
      // check that only results that were created before this field are used
      FeaturePtr aResultFeature = document()->feature(aBody);
      if (!aResultFeature.get())
        continue;
      GeomShapePtr aShape = aBody->shape();
      if (!aShape.get() || aShape->isNull())
        continue;
      GeomAPI_ShapeExplorer anExp(aShape, GeomAPI_Shape::shapeTypeByStr(aSelectionType));
      for(; anExp.more(); anExp.next()) {
        aSelectionList->append(aBody, anExp.current());
        aCountSelected--;
        if (aCountSelected == 0)
          break;
      }
    }

    // conversion of type
    XAO::Type aFieldType = aXaoField->getType();
    std::string aTypeString = XAO::XaoUtils::fieldTypeToString(aFieldType);
    ModelAPI_AttributeTables::ValueType aType =
      ExchangePlugin_Tools::xaoType2valuesType(aTypeString);
    // set components names
    AttributeStringArrayPtr aComponents = aFieldFeature->stringArray("components_names");
    aComponents->setSize(aXaoField->countComponents());
    for(int aComp = 0; aComp < aXaoField->countComponents(); aComp++) {
      aComponents->setValue(aComp, aXaoField->getComponentName(aComp));
    }

    AttributeIntArrayPtr aStamps = aFieldFeature->intArray("stamps");
    aStamps->setSize(aXaoField->countSteps());
    std::shared_ptr<ModelAPI_AttributeTables> aTables = aFieldFeature->tables("values");
    aTables->setSize(
      aXaoField->countElements() + 1, aXaoField->countComponents(), aXaoField->countSteps());
    aTables->setType(aType);
    // iterate steps
    XAO::stepIterator aStepIter = aXaoField->begin();
    for(int aStepIndex = 0; aStepIter != aXaoField->end(); aStepIter++, aStepIndex++) {
      aStamps->setValue(aStepIndex, (*aStepIter)->getStamp());
      for(int aRow = 1; aRow <= aXaoField->countElements(); aRow++) {
        for(int aCol = 0; aCol < aXaoField->countComponents(); aCol++) {
          ModelAPI_AttributeTables::Value aVal;
          std::string aValStr = (*aStepIter)->getStringValue(aRow - 1, aCol);
          switch(aType) {
          case ModelAPI_AttributeTables::BOOLEAN:
            aVal.myBool = aValStr == "true";
            break;
          case ModelAPI_AttributeTables::INTEGER:
            aVal.myInt = atoi(aValStr.c_str());
            break;
          case ModelAPI_AttributeTables::DOUBLE:
            aVal.myDouble = atof(aValStr.c_str());
            break;
          case ModelAPI_AttributeTables::STRING:
            aVal.myStr = aValStr;
            break;
          }
          aTables->setValue(aVal, aRow, aCol, aStepIndex);
        }
      }
    }
    // remove everything with zero-values: zeroes are treated as defaults
    std::set<int> aRowsToRemove;
    for(int aRow = 1; aRow < aTables->rows(); aRow++) {
      bool isZero = true;
      for(int aCol = 0; aCol < aTables->columns() && isZero; aCol++) {
        for(int aStepIndex = 0; aStepIndex != aTables->tables() && isZero; aStepIndex++) {
          if (aTables->valueStr(aRow, aCol, aStepIndex) != aTables->valueStr(0, aCol, aStepIndex))
            isZero = false;
        }
      }
      if (isZero)
        aRowsToRemove.insert(aRow - 1); // -1 to make prepared for remove from SelectionList
    }
    if (!aRowsToRemove.empty()) { // move usefull rows on bottom to the up of the tables
      // number of rows passed during going through: the current rows will
      // be moved up for this value
      int aRemovedPassed = 0;
      for(int aRow = 1; aRow < aTables->rows(); aRow++) {
        if (aRowsToRemove.find(aRow - 1) != aRowsToRemove.end()) {
          aRemovedPassed++;
        } else if (aRemovedPassed != 0) { // copy the line up
          for(int aCol = 0; aCol < aTables->columns(); aCol++) {
            for(int aTable = 0; aTable != aTables->tables(); aTable++) {
              aTables->setValue(
                aTables->value(aRow, aCol, aTable), aRow - aRemovedPassed, aCol, aTable);
            }
          }
        }
      }
      aTables->setSize(aTables->rows() - aRemovedPassed, aTables->columns(), aTables->tables());
      aSelectionList->remove(aRowsToRemove); // remove also selected elements
    }
  }
  // Top avoid problems in Object Browser update: issue #1647.
  ModelAPI_EventCreator::get()->sendReordered(
    std::dynamic_pointer_cast<ModelAPI_Feature>(aRefListOfGroups->owner()));

// LCOV_EXCL_START
  } catch (XAO::XAO_Exception& e) {
    std::string anError = e.what();
    setError("An error occurred while importing " + theFileName + ": " + anError);
    return;
  }
// LCOV_EXCL_STOP
}

//============================================================================
std::shared_ptr<ModelAPI_Feature> ExchangePlugin_ImportFeatureBase::addFeature(
    std::string theID)
{
  std::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID, false);
  if (aNew)
    data()->reflist(FEATURES_ID())->append(aNew);
  // set as current also after it becomes sub to set correctly enabled for other subs
  //document()->setCurrentFeature(aNew, false);
  return aNew;
}

// LCOV_EXCL_START
void ExchangePlugin_ImportFeatureBase::removeFeature(
    std::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (!data()->isValid())
    return;
  AttributeRefListPtr aList = reflist(FEATURES_ID());
  aList->remove(theFeature);
}
// LCOV_EXCL_STOP

int ExchangePlugin_ImportFeatureBase::numberOfSubs(bool /*forTree*/) const
{
  return data()->reflist(FEATURES_ID())->size(true);
}

std::shared_ptr<ModelAPI_Feature> ExchangePlugin_ImportFeatureBase::subFeature(
    const int theIndex, bool /*forTree*/)
{
  ObjectPtr anObj = data()->reflist(FEATURES_ID())->object(theIndex, false);
  FeaturePtr aRes = std::dynamic_pointer_cast<ModelAPI_Feature>(anObj);
  return aRes;
}

// LCOV_EXCL_START
int ExchangePlugin_ImportFeatureBase::subFeatureId(const int theIndex) const
{
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(FEATURES_ID()));
  std::list<ObjectPtr> aFeatures = aRefList->list();
  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin();
  int aResultIndex = 1; // number of the counted (created) features, started from 1
  int aFeatureIndex = -1; // number of the not-empty features in the list
  for (; anIt != aFeatures.end(); anIt++) {
    if (anIt->get())
      aFeatureIndex++;
    if (aFeatureIndex == theIndex)
      break;
    aResultIndex++;
  }
  return aResultIndex;
}
// LCOV_EXCL_STOP

bool ExchangePlugin_ImportFeatureBase::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (aFeature)
    return data()->reflist(FEATURES_ID())->isInList(aFeature);
  return false;
}

//============================================================================
void ExchangePlugin_ImportFeatureBase::loadNamingDS(
    std::shared_ptr<GeomAPI_Shape> theGeomShape,
    std::shared_ptr<ModelAPI_ResultBody> theResultBody)
{
  //load result
  theResultBody->store(theGeomShape);

  std::string aNameMS = "Shape";
  theResultBody->loadFirstLevel(theGeomShape, aNameMS);
}

void ExchangePlugin_Import_ImageFeature::importFile(const std::string& theFileName)
{

  std::string anExtension = GeomAlgoAPI_Tools::File_Tools::extension(theFileName);
  std::string theTextureFileName = "";
  // Perform the import
  std::string anError;
  std::shared_ptr<GeomAPI_Shape> aGeomShape;
   if (anExtension == "PNG" || anExtension == "GIF" ||
             anExtension == "TIFF" || anExtension == "JPE" ||
             anExtension == "JPG" || anExtension == "JPEG" ||
             anExtension == "BMP"|| anExtension == "PPM"
             ) {
     aGeomShape = ImageImport(theFileName, anError);
     if(anError == "")
       theTextureFileName = theFileName;
    } else {
    anError = "Unsupported format: " + anExtension;
  }

  // Check if shape is valid
  if (!anError.empty()) {
    setError("An error occurred while importing " + theFileName + ": " + anError);
    return;
  }

  // Pass the results into the model
  std::string anObjectName = GeomAlgoAPI_Tools::File_Tools::name(theFileName);
  data()->setName(Locale::Convert::toWString(anObjectName));

  auto resultBody = createResultBody(aGeomShape);
  resultBody->setTextureFile(theTextureFileName);
  setResult(resultBody);
}
