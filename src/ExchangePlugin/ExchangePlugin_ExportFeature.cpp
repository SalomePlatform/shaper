// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_ExportFeature.cpp
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#include <ExchangePlugin_ExportFeature.h>

#include <algorithm>
#include <iterator>
#include <string>
#ifdef _DEBUG
#include <iostream>
#include <ostream>
#endif

#include <Config_Common.h>
#include <Config_PropManager.h>

#include <GeomAlgoAPI_BREPExport.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_IGESExport.h>
#include <GeomAlgoAPI_STEPExport.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_XAOExport.h>

#include <GeomAPI_Shape.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeTables.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultField.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <XAO_Group.hxx>
#include <XAO_Field.hxx>
#include <XAO_Xao.hxx>

#include <ExchangePlugin_Tools.h>

ExchangePlugin_ExportFeature::ExchangePlugin_ExportFeature()
{
}

ExchangePlugin_ExportFeature::~ExchangePlugin_ExportFeature()
{
  // TODO Auto-generated destructor stub
}

/*
 * Request for initialization of data model of the feature: adding all attributes
 */
void ExchangePlugin_ExportFeature::initAttributes()
{
  data()->addAttribute(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID(),
    ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::FILE_PATH_ID(),
    ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::XAO_FILE_PATH_ID(),
    ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::FILE_FORMAT_ID(),
    ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::SELECTION_LIST_ID(),
    ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID(),
    ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID(),
    ModelAPI_AttributeString::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
    ExchangePlugin_ExportFeature::XAO_FILE_PATH_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
    ExchangePlugin_ExportFeature::XAO_AUTHOR_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
    ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID());
}

void ExchangePlugin_ExportFeature::attributeChanged(const std::string& theID)
{
  if (theID == XAO_FILE_PATH_ID()) {
    string(ExchangePlugin_ExportFeature::FILE_PATH_ID())->setValue(
      string(ExchangePlugin_ExportFeature::XAO_FILE_PATH_ID())->value());
  }
}

/*
 * Computes or recomputes the results
 */
void ExchangePlugin_ExportFeature::execute()
{
  AttributeStringPtr aFormatAttr =
      this->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID());
  std::string aFormat = aFormatAttr->value();

  AttributeStringPtr aFilePathAttr =
      this->string(ExchangePlugin_ExportFeature::FILE_PATH_ID());
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty())
    return;

  exportFile(aFilePath, aFormat);
}

void ExchangePlugin_ExportFeature::exportFile(const std::string& theFileName,
                                              const std::string& theFormat)
{
  std::string aFormatName = theFormat;

  if (aFormatName.empty()) { // get default format for the extension
    // ".brep" -> "BREP"
    std::string anExtension = GeomAlgoAPI_Tools::File_Tools::extension(theFileName);
    if (anExtension == "BREP" || anExtension == "BRP") {
      aFormatName = "BREP";
    } else if (anExtension == "STEP" || anExtension == "STP") {
      aFormatName = "STEP";
    } else if (anExtension == "IGES" || anExtension == "IGS") {
      aFormatName = "IGES-5.1";
    } else if (anExtension == "XAO") {
      aFormatName = "XAO";
    } else {
      aFormatName = anExtension;
    }
  }

  if (aFormatName == "XAO") {
    exportXAO(theFileName);
    return;
  }

  // make shape for export from selected shapes
  AttributeSelectionListPtr aSelectionListAttr =
      this->selectionList(ExchangePlugin_ExportFeature::SELECTION_LIST_ID());
  std::list<GeomShapePtr> aShapes;
  for (int i = 0, aSize = aSelectionListAttr->size(); i < aSize; ++i) {
    AttributeSelectionPtr anAttrSelection = aSelectionListAttr->value(i);
    std::shared_ptr<GeomAPI_Shape> aCurShape = anAttrSelection->value();
    if (aCurShape.get() == NULL)
      aCurShape = anAttrSelection->context()->shape();
    if (aCurShape.get() != NULL)
      aShapes.push_back(aCurShape);
  }

  // Store compound if we have more than one shape.
  std::shared_ptr<GeomAPI_Shape> aShape;
  if(aShapes.size() == 1) {
    aShape = aShapes.front();
  } else {
    aShape = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  }

  // Perform the export
  std::string anError;
  bool aResult = false;
  if (aFormatName == "BREP") {
    aResult = BREPExport(theFileName, aFormatName, aShape, anError);
  } else if (aFormatName == "STEP") {
    aResult = STEPExport(theFileName, aFormatName, aShape, anError);
  } else if (aFormatName.substr(0, 4) == "IGES") {
    aResult = IGESExport(theFileName, aFormatName, aShape, anError);
  } else {
    anError = "Unsupported format: " + aFormatName;
  }

  if (!anError.empty()) {
    setError("An error occurred while exporting " + theFileName + ": " + anError);
    return;
  }
}

void ExchangePlugin_ExportFeature::exportXAO(const std::string& theFileName)
{
  try {

  std::string anError;
  XAO::Xao aXao;

  // author

  std::string anAuthor = string(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID())->value();
  aXao.setAuthor(anAuthor);

  // make shape for export from all results
  std::list<GeomShapePtr> aShapes;
  int aBodyCount = document()->size(ModelAPI_ResultBody::group());
  for (int aBodyIndex = 0; aBodyIndex < aBodyCount; ++aBodyIndex) {
    ResultBodyPtr aResultBody =
        std::dynamic_pointer_cast<ModelAPI_ResultBody>(
            document()->object(ModelAPI_ResultBody::group(), aBodyIndex));
    if (!aResultBody.get())
      continue;
    aShapes.push_back(aResultBody->shape());
  }
  GeomShapePtr aShape = (aShapes.size() == 1)
      ? *aShapes.begin()
      : GeomAlgoAPI_CompoundBuilder::compound(aShapes);

  SetShapeToXAO(aShape, &aXao, anError);

  if (!anError.empty()) {
    setError("An error occurred while exporting " + theFileName + ": " + anError);
    return;
  }

  // geometry name

  std::string aGeometryName = string(ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID())->value();
  aXao.getGeometry()->setName(aGeometryName);

  // groups

  int aGroupCount = document()->size(ModelAPI_ResultGroup::group());
  for (int aGroupIndex = 0; aGroupIndex < aGroupCount; ++aGroupIndex) {
    ResultGroupPtr aResultGroup =
        std::dynamic_pointer_cast<ModelAPI_ResultGroup>(
            document()->object(ModelAPI_ResultGroup::group(), aGroupIndex));

    FeaturePtr aGroupFeature = document()->feature(aResultGroup);

    AttributeSelectionListPtr aSelectionList =
        aGroupFeature->selectionList("group_list");

    // conversion of dimension
    std::string aSelectionType = aSelectionList->selectionType();
    std::string aDimensionString =
      ExchangePlugin_Tools::selectionType2xaoDimension(aSelectionType);
    XAO::Dimension aGroupDimension = XAO::XaoUtils::stringToDimension(aDimensionString);

    XAO::Group* aXaoGroup = aXao.addGroup(aGroupDimension,
                                          aResultGroup->data()->name());

    for (int aSelectionIndex = 0; aSelectionIndex < aSelectionList->size(); ++aSelectionIndex) {
      AttributeSelectionPtr aSelection = aSelectionList->value(aSelectionIndex);

      // complex conversion of reference id to element index
      int aReferenceID = aSelection->Id();
      std::string aReferenceString = XAO::XaoUtils::intToString(aReferenceID);
      int anElementID =
        aXao.getGeometry()->getElementIndexByReference(aGroupDimension, aReferenceString);

      aXaoGroup->add(anElementID);
    }
  }

  // fields
  int aFieldCount = document()->size(ModelAPI_ResultField::group());
  for (int aFieldIndex = 0; aFieldIndex < aFieldCount; ++aFieldIndex) {
    ResultFieldPtr aResultField =
        std::dynamic_pointer_cast<ModelAPI_ResultField>(
            document()->object(ModelAPI_ResultField::group(), aFieldIndex));

    FeaturePtr aFieldFeature = document()->feature(aResultField);

    AttributeSelectionListPtr aSelectionList =
        aFieldFeature->selectionList("selected");

    // conversion of dimension
    std::string aSelectionType = aSelectionList->selectionType();
    std::string aDimensionString =
      ExchangePlugin_Tools::selectionType2xaoDimension(aSelectionType);
    XAO::Dimension aFieldDimension = XAO::XaoUtils::stringToDimension(aDimensionString);
    bool isWholePart = aSelectionType == "part";
    // get tables and their type
    std::shared_ptr<ModelAPI_AttributeTables> aTables = aFieldFeature->tables("values");
    std::string aTypeString = ExchangePlugin_Tools::valuesType2xaoType(aTables->type());
    XAO::Type aFieldType = XAO::XaoUtils::stringToFieldType(aTypeString);

    XAO::Field* aXaoField = aXao.addField(aFieldType, aFieldDimension, aTables->columns(),
                                          aResultField->data()->name());
    // set components names
    AttributeStringArrayPtr aComponents = aFieldFeature->stringArray("components_names");
    for(int aComp = 0; aComp < aComponents->size(); aComp++) {
      std::string aName = aComponents->value(aComp);
      aXaoField->setComponentName(aComp, aName);
    }

    AttributeIntArrayPtr aStamps = aFieldFeature->intArray("stamps");
    for (int aStepIndex = 0; aStepIndex < aTables->tables(); aStepIndex++) {
      XAO::Step* aStep = aXaoField->addNewStep(aStepIndex);
      aStep->setStep(aStepIndex);
      int aStampIndex = aStamps->value(aStepIndex);
      aStep->setStamp(aStampIndex);
      int aNumElements = isWholePart ? aXaoField->countElements() : aTables->rows();
      int aNumComps = aTables->columns();
      // omit default values first row
      for(int aRow = isWholePart ? 0 : 1; aRow < aNumElements; aRow++) {
        for(int aCol = 0; aCol < aNumComps; aCol++) {
          int anElementID = 0;
          if (!isWholePart) {
            // element index actually is the ID of the selection
            AttributeSelectionPtr aSelection = aSelectionList->value(aRow - 1);

            // complex conversion of reference id to element index
            int aReferenceID = aSelection->Id();
            std::string aReferenceString = XAO::XaoUtils::intToString(aReferenceID);
            int anElementID =
              aXao.getGeometry()->getElementIndexByReference(aFieldDimension, aReferenceString);
          }

          ModelAPI_AttributeTables::Value aVal = aTables->value(
            isWholePart ? 0 : aRow, aCol, aStepIndex);
          std::ostringstream aStr; // string value
          switch(aTables->type()) {
          case ModelAPI_AttributeTables::BOOLEAN:
            aStr<<(aVal.myBool ? "True" : "False");
            break;
          case ModelAPI_AttributeTables::INTEGER:
            aStr<<aVal.myInt;
            break;
          case ModelAPI_AttributeTables::DOUBLE:
            aStr<<aVal.myDouble;
            break;
          case ModelAPI_AttributeTables::STRING:
            aStr<<aVal.myStr;
            break;
          }
          std::string aStrVal = aStr.str();
          aStep->setStringValue(isWholePart ? aRow : anElementID, aCol, aStrVal);
        }
      }
    }
  }


  // exporting

  XAOExport(theFileName, &aXao, anError);

  if (!anError.empty()) {
    setError("An error occurred while exporting " + theFileName + ": " + anError);
    return;
  }

  } catch (XAO::XAO_Exception& e) {
    std::string anError = e.what();
    setError("An error occurred while exporting " + theFileName + ": " + anError);
    return;
  }
}
