// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_ImportFeature.cpp
// Created: Aug 28, 2014
// Authors:  Sergey BELASH, Sergey POKHODENKO

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

#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
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
  data()->addAttribute(ExchangePlugin_ImportFeature::FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ImportFeature::FEATURES_ID(), ModelAPI_AttributeRefList::typeId());

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

  // Process groups
  std::shared_ptr<ModelAPI_AttributeRefList> aRefListOfGroups =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(data()->attribute(FEATURES_ID()));

  // Remove previous groups stored in RefList
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
    XAO::Dimension aGroupDimension = aXaoGroup->getDimension();
    std::string aDimensionString = XAO::XaoUtils::dimensionToString(aXaoGroup->getDimension());
    std::string aSelectionType = ExchangePlugin_Tools::xaoDimension2selectionType(aDimensionString);

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
//
//    aRefListOfGroups->append(aGroupFeature);
//
//    // hide the group in the history
//    document()->setCurrentFeature(aGroupFeature, false);
//    // groups features is internal part of the import
//    aGroupFeature->setInHistory(aGroupFeature, false);
  }

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
  // if the object is last, remove it from the list (needed to skip empty transaction on edit of sketch feature)
  if (aList->object(aList->size(true) - 1, true) == theFeature) {
    aList->remove(theFeature);
  } else {
    // to keep the persistent sub-elements indexing, do not remove elements from list,
    // but substitute by nulls
    aList->substitute(theFeature, ObjectPtr());
  }
}

int ExchangePlugin_ImportFeature::numberOfSubs(bool forTree) const
{
  return data()->reflist(FEATURES_ID())->size(false);
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
  theResultBody->loadFirstLevel(theGeomShape, aNameMS, aTag);
}
