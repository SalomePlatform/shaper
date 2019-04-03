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
#include <ModelAPI_Tools.h>

#include <Events_InfoMessage.h>

#include <XAO_Group.hxx>
#include <XAO_Field.hxx>
#include <XAO_Xao.hxx>
#include <XAO_Geometry.hxx>

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

/// Returns XAO string by the value from the table
static std::string valToString(const ModelAPI_AttributeTables::Value& theVal,
  const ModelAPI_AttributeTables::ValueType& theType) {
  std::ostringstream aStr; // the resulting string value
  switch(theType) {
  case ModelAPI_AttributeTables::BOOLEAN:
    aStr<<(theVal.myBool ? "true" : "false");
    break;
  case ModelAPI_AttributeTables::INTEGER:
    aStr<<theVal.myInt;
    break;
  case ModelAPI_AttributeTables::DOUBLE:
    aStr<<theVal.myDouble;
    break;
  case ModelAPI_AttributeTables::STRING:
    aStr<<theVal.myStr;
    break;
  }
  return aStr.str();
}

/// Returns true if something in selection is presented in the results list
static bool isInResults(AttributeSelectionListPtr theSelection,
                        const std::list<ResultBodyPtr>& theResults,
                        std::set<ResultBodyPtr>& theCashedResults)
{
  // collect all results into a cashed set
  if (theCashedResults.empty()) {
    std::list<ResultPtr> aResults;
    std::list<ResultBodyPtr>::const_iterator aRes = theResults.cbegin();
    for(; aRes != theResults.cend(); aRes++) {
      if (theCashedResults.count(*aRes))
        continue;
      else
        theCashedResults.insert(*aRes);
      std::list<ResultPtr> aResults;
      ModelAPI_Tools::allSubs(*aRes, aResults, false);
      for(std::list<ResultPtr>::iterator aR = aResults.begin(); aR != aResults.end(); aR++) {
        theCashedResults.insert(std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aR));
      }
    }
  }
  // if context is in results, return true
  for(int a = 0; a < theSelection->size(); a++) {
    AttributeSelectionPtr anAttr = theSelection->value(a);
    ResultBodyPtr aSelected= std::dynamic_pointer_cast<ModelAPI_ResultBody>(anAttr->context());
    if (aSelected.get() && theCashedResults.count(aSelected))
      return true;
  }
  return false;
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
  std::list<ResultBodyPtr> aResults;

  AttributeSelectionListPtr aSelection = selectionList(SELECTION_LIST_ID());
  bool aIsSelection = aSelection->isInitialized() && aSelection->size() > 0;
  if (aIsSelection) { // a mode for export to geom result by result
    for(int a = 0; a < aSelection->size(); a++) {
      AttributeSelectionPtr anAttr = aSelection->value(a);
      ResultBodyPtr aBodyContext =
        std::dynamic_pointer_cast<ModelAPI_ResultBody>(anAttr->context());
      if (aBodyContext.get() && !aBodyContext->isDisabled() && aBodyContext->shape().get()) {
        aResults.push_back(aBodyContext);
        GeomShapePtr aShape = anAttr->value();
        if (!aShape.get())
          aShape = aBodyContext->shape();
        aShapes.push_back(aShape);
      }
    }
  } else {
    int aBodyCount = document()->size(ModelAPI_ResultBody::group());
    for (int aBodyIndex = 0; aBodyIndex < aBodyCount; ++aBodyIndex) {
      ResultBodyPtr aResultBody =
          std::dynamic_pointer_cast<ModelAPI_ResultBody>(
              document()->object(ModelAPI_ResultBody::group(), aBodyIndex));
      if (!aResultBody.get())
        continue;
      aShapes.push_back(aResultBody->shape());
      aResults.push_back(aResultBody);
    }
  }
  if (aShapes.empty()) {
    setError("No shapes to export");
    return;
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
  if (aGeometryName.empty() && aResults.size() == 1) {
    // get the name from the first result
    ResultBodyPtr aResultBody = *aResults.begin();
    aGeometryName = aResultBody->data()->name();
  }

  aXao.getGeometry()->setName(aGeometryName);

  std::set<ResultBodyPtr> allResultsCashed; // cash to speed up searching in all results selected

  // groups
  int aGroupCount = document()->size(ModelAPI_ResultGroup::group());
  for (int aGroupIndex = 0; aGroupIndex < aGroupCount; ++aGroupIndex) {
    ResultGroupPtr aResultGroup =
        std::dynamic_pointer_cast<ModelAPI_ResultGroup>(
            document()->object(ModelAPI_ResultGroup::group(), aGroupIndex));

    FeaturePtr aGroupFeature = document()->feature(aResultGroup);

    AttributeSelectionListPtr aSelectionList =
        aGroupFeature->selectionList("group_list");

    if (!isInResults(aSelectionList, aResults, allResultsCashed))// skip group not used in results
      continue;

    // conversion of dimension
    std::string aSelectionType = aSelectionList->selectionType();
    std::string aDimensionString =
      ExchangePlugin_Tools::selectionType2xaoDimension(aSelectionType);
    XAO::Dimension aGroupDimension = XAO::XaoUtils::stringToDimension(aDimensionString);

    XAO::Group* aXaoGroup = aXao.addGroup(aGroupDimension,
                                          aResultGroup->data()->name());

    try {
      for (int aSelectionIndex = 0; aSelectionIndex < aSelectionList->size(); ++aSelectionIndex) {
        AttributeSelectionPtr aSelection = aSelectionList->value(aSelectionIndex);

        // complex conversion of reference id to element index
        // gives bad id in case the selection is done from python script
        // => using GeomAlgoAPI_CompoundBuilder::id instead
        // int aReferenceID_old = aSelection->Id();

        int aReferenceID = GeomAlgoAPI_CompoundBuilder::id(aShape, aSelection->value());

        if (aReferenceID == 0) // selected value does not found in the exported shape
          continue;

        std::string aReferenceString = XAO::XaoUtils::intToString(aReferenceID);
        int anElementID =
          aXao.getGeometry()->getElementIndexByReference(aGroupDimension, aReferenceString);

        aXaoGroup->add(anElementID);
      }
    } catch (XAO::XAO_Exception& e) {
      // LCOV_EXCL_START
      std::string msg = "An error occurred while exporting group " + aResultGroup->data()->name();
      msg += ".\n";
      msg += e.what();
      msg += "\n";
      msg += "=> skipping this group from XAO export.";
      Events_InfoMessage("ExportFeature", msg, this).send();
      aXao.removeGroup(aXaoGroup);
      // LCOV_EXCL_STOP
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
    std::string aSelectionType = aSelectionList->selectionType();
    bool isWholePart = aSelectionType == "part";

    if (!isWholePart &&
        !isInResults(aSelectionList, aResults, allResultsCashed))// skip field not used in results
      continue;

    // conversion of dimension
    std::string aDimensionString =
      ExchangePlugin_Tools::selectionType2xaoDimension(aSelectionType);
    XAO::Dimension aFieldDimension = XAO::XaoUtils::stringToDimension(aDimensionString);
    // get tables and their type
    std::shared_ptr<ModelAPI_AttributeTables> aTables = aFieldFeature->tables("values");
    std::string aTypeString = ExchangePlugin_Tools::valuesType2xaoType(aTables->type());
    XAO::Type aFieldType = XAO::XaoUtils::stringToFieldType(aTypeString);

    XAO::Field* aXaoField = aXao.addField(aFieldType, aFieldDimension, aTables->columns(),
                                          aResultField->data()->name());


    try {
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
        std::set<int> aFilledIDs; // to fill the rest by defaults
        // omit default values first row
        for(int aRow = isWholePart ? 0 : 1; aRow < aNumElements; aRow++) {
          for(int aCol = 0; aCol < aNumComps; aCol++) {
            int anElementID = 0;
            if (!isWholePart) {
              // element index actually is the ID of the selection
              AttributeSelectionPtr aSelection = aSelectionList->value(aRow - 1);
              int aReferenceID = GeomAlgoAPI_CompoundBuilder::id(aShape, aSelection->value());
              if (aReferenceID == 0) // selected value does not found in the exported shape
                continue;

              std::string aReferenceString = XAO::XaoUtils::intToString(aReferenceID);
              anElementID =
                aXao.getGeometry()->getElementIndexByReference(aFieldDimension, aReferenceString);
            }

            ModelAPI_AttributeTables::Value aVal = aTables->value(
              isWholePart ? 0 : aRow, aCol, aStepIndex);
            std::string aStrVal = valToString(aVal, aTables->type());
            aStep->setStringValue(isWholePart ? aRow : anElementID, aCol, aStrVal);
            aFilledIDs.insert(anElementID);
          }
        }
        if (!isWholePart) { // fill the rest values by default ones
          XAO::GeometricElementList::iterator allElem = aXao.getGeometry()->begin(aFieldDimension);
          for(; allElem != aXao.getGeometry()->end(aFieldDimension); allElem++) {
            if (aFilledIDs.find(allElem->first) != aFilledIDs.end())
              continue;
            for(int aCol = 0; aCol < aNumComps; aCol++) {
              ModelAPI_AttributeTables::Value aVal = aTables->value(0, aCol, aStepIndex); // default
              std::string aStrVal = valToString(aVal, aTables->type());
              aStep->setStringValue(allElem->first, aCol, aStrVal);
            }
          }
        }
      }
    } catch (XAO::XAO_Exception& e) {
      // LCOV_EXCL_START
      std::string msg = "An error occurred while exporting field " + aResultField->data()->name();
      msg += ".\n";
      msg += e.what();
      msg += "\n";
      msg += "=> skipping this field from XAO export.";
      Events_InfoMessage("ExportFeature", msg, this).send();
      aXao.removeField(aXaoField);
      // LCOV_EXCL_STOP
    }
  }

  // exporting
  XAOExport(theFileName, &aXao, anError);

  if (!anError.empty()) {
    setError("An error occurred while exporting " + theFileName + ": " + anError);
    return;
  }

// LCOV_EXCL_START
  } catch (XAO::XAO_Exception& e) {
    std::string anError = e.what();
    setError("An error occurred while exporting " + theFileName + ": " + anError);
    return;
  }
// LCOV_EXCL_STOP
}
