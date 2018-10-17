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

#include <GeomAPI_Shape.h>
#include <GeomAPI_ShapeExplorer.h>

#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeTables.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <XAO_Xao.hxx>
#include <XAO_Group.hxx>
#include <XAO_Field.hxx>
#include <XAO_Step.hxx>

#include <ExchangePlugin_Tools.h>

ExchangePlugin_ImportFeature::ExchangePlugin_ImportFeature()
{
}

ExchangePlugin_ImportFeature::~ExchangePlugin_ImportFeature()
{
  // TODO Auto-generated destructor stub
}

/*
 * Request for initialization of data model of the feature: adding all attributes
 */
void ExchangePlugin_ImportFeature::initAttributes()
{
  data()->addAttribute(ExchangePlugin_ImportFeature::FILE_PATH_ID(),
                       ModelAPI_AttributeString::typeId());
  AttributePtr aFeaturesAttribute =
    data()->addAttribute(ExchangePlugin_ImportFeature::FEATURES_ID(),
                         ModelAPI_AttributeRefList::typeId());
  aFeaturesAttribute->setIsArgument(false);

  ModelAPI_Session::get()->validators()->registerNotObligatory(
      getKind(), ExchangePlugin_ImportFeature::FEATURES_ID());
}

/*
 * Computes or recomputes the results
 */
void ExchangePlugin_ImportFeature::execute()
{
  AttributeStringPtr aFilePathAttr = string(ExchangePlugin_ImportFeature::FILE_PATH_ID());
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty()) {
    setError("File path is empty.");
    return;
  }

  importFile(aFilePath);
}

std::shared_ptr<ModelAPI_ResultBody> ExchangePlugin_ImportFeature::createResultBody(
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
  if (anExtension == "BREP" || anExtension == "BRP") {
    aGeomShape = BREPImport(theFileName, anExtension, anError);
  } else if (anExtension == "STEP" || anExtension == "STP") {
    aGeomShape = STEPImport(theFileName, anExtension, anError);
  } else if (anExtension == "IGES" || anExtension == "IGS") {
    aGeomShape = IGESImport(theFileName, anExtension, anError);
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
  data()->setName(anObjectName);

  setResult(createResultBody(aGeomShape));
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
  data()->setName(aBodyName);

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
      aGroupFeature->data()->setName(aXaoGroup->getName());

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
      aFieldFeature->data()->setName(aXaoField->getName());

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

  } catch (XAO::XAO_Exception& e) {
    std::string anError = e.what();
    setError("An error occurred while importing " + theFileName + ": " + anError);
    return;
  }
}

//============================================================================
std::shared_ptr<ModelAPI_Feature> ExchangePlugin_ImportFeature::addFeature(
    std::string theID)
{
  std::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID, false);
  if (aNew)
    data()->reflist(FEATURES_ID())->append(aNew);
  // set as current also after it becomes sub to set correctly enabled for other subs
  //document()->setCurrentFeature(aNew, false);
  return aNew;
}

void ExchangePlugin_ImportFeature::removeFeature(
    std::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (!data()->isValid())
    return;
  AttributeRefListPtr aList = reflist(FEATURES_ID());
  aList->remove(theFeature);
}

int ExchangePlugin_ImportFeature::numberOfSubs(bool forTree) const
{
  return data()->reflist(FEATURES_ID())->size(true);
}

std::shared_ptr<ModelAPI_Feature> ExchangePlugin_ImportFeature::subFeature(
    const int theIndex, bool forTree)
{
  ObjectPtr anObj = data()->reflist(FEATURES_ID())->object(theIndex, false);
  FeaturePtr aRes = std::dynamic_pointer_cast<ModelAPI_Feature>(anObj);
  return aRes;
}

int ExchangePlugin_ImportFeature::subFeatureId(const int theIndex) const
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

bool ExchangePlugin_ImportFeature::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (aFeature)
    return data()->reflist(FEATURES_ID())->isInList(aFeature);
  return false;
}

//============================================================================
void ExchangePlugin_ImportFeature::loadNamingDS(
    std::shared_ptr<GeomAPI_Shape> theGeomShape,
    std::shared_ptr<ModelAPI_ResultBody> theResultBody)
{
  //load result
  theResultBody->store(theGeomShape);

  int aTag(1);
  std::string aNameMS = "Shape";
  theResultBody->loadFirstLevel(theGeomShape, aNameMS);
}
