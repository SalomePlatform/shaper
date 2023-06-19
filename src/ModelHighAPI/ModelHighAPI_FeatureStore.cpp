// Copyright (C) 2014-2023  CEA, EDF
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

#include <ModelHighAPI_FeatureStore.h>

#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeTables.h>
#include <ModelAPI_Validator.h>

#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Pnt.h>

#include <Locale_Convert.h>

#include <ios>
#include <cmath>
#include <sstream>
#include <iomanip>

#define PRECISION 6
#define TOLERANCE (1.e-7)

ModelHighAPI_FeatureStore::ModelHighAPI_FeatureStore(ObjectPtr theObject) {
  storeData(theObject->data(), myAttrs);

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (aFeature) {
    // iterate results to store
    std::list<ResultPtr> allResults;
    ModelAPI_Tools::allResults(aFeature, allResults);
    std::list<ResultPtr>::iterator aRes = allResults.begin();
    for(; aRes != allResults.end(); aRes++) {
      std::map<std::string, std::string> aResDump;
      storeData((*aRes)->data(), aResDump);
      myRes.push_back(aResDump);
    }
  }
}

std::string ModelHighAPI_FeatureStore::compare(ObjectPtr theObject) {
  std::string anError = compareData(theObject->data(), myAttrs);
  if (!anError.empty()) {
    return "Features '" + Locale::Convert::toString(theObject->data()->name()) +
      "' differ:" + anError;
  }

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (aFeature) {
    std::list<ResultPtr> allResults;
    ModelAPI_Tools::allResults(aFeature, allResults);
    std::list<ResultPtr>::iterator aRes = allResults.begin();
    std::list<std::map<std::string, std::string> >::iterator aResIter = myRes.begin();
    for(; aRes != allResults.end() && aResIter != myRes.end(); aRes++, aResIter++) {
      anError = compareData((*aRes)->data(), *aResIter);
      if (!anError.empty())
        return "Results of feature '" + Locale::Convert::toString(aFeature->name()) +
        "' '" + Locale::Convert::toString((*aRes)->data()->name()) +
        "' differ:" + anError;
    }
    if (aRes != allResults.end()) {
      return "Current model has more results '" +
        Locale::Convert::toString((*aRes)->data()->name()) + "'";
    }
    if (aResIter != myRes.end()) {
      return "Original model had more results '" + (*aResIter)["__name__"] + "'";
    }
  }
  return ""; // ok
}

void ModelHighAPI_FeatureStore::storeData(std::shared_ptr<ModelAPI_Data> theData,
  std::map<std::string, std::string>& theAttrs)
{
  // store name to keep also this information and output if needed
  theAttrs["__name__"] = Locale::Convert::toString(theData->name());
  std::list<std::shared_ptr<ModelAPI_Attribute> > allAttrs = theData->attributes("");
  std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = allAttrs.begin();
  for(; anAttr != allAttrs.end(); anAttr++) {
    theAttrs[(*anAttr)->id()] = dumpAttr(*anAttr);
  }
  ResultPtr aShapeOwner = std::dynamic_pointer_cast<ModelAPI_Result>(theData->owner());
  if (aShapeOwner.get() && aShapeOwner->shape().get()) {
    std::shared_ptr<GeomAPI_Shape> aShape = aShapeOwner->shape();
    theAttrs["__shape__"] = dumpShape(aShape);
  }
}

std::string ModelHighAPI_FeatureStore::compareData(std::shared_ptr<ModelAPI_Data> theData,
  std::map<std::string, std::string>& theAttrs)
{
  std::map<std::string, std::string> aThis;
  storeData(theData, aThis);
  std::map<std::string, std::string>::iterator aThisIter = aThis.begin();
  for(; aThisIter != aThis.end(); aThisIter++) {
    if (theAttrs.find(aThisIter->first) == theAttrs.end()) {
      return "original model had no attribute '" + aThisIter->first + "'";
    }
    if (theAttrs[aThisIter->first] != aThisIter->second) {
      return "attribute '" + aThisIter->first + "' is different (original != current) '" +
        theAttrs[aThisIter->first] + "' != '" + aThisIter->second + "'";
    }
  }
  // iterate back to find lack attribute in the current model
  std::map<std::string, std::string>::iterator anOrigIter = theAttrs.begin();
  for(; anOrigIter != theAttrs.end(); anOrigIter++) {
    if (aThis.find(anOrigIter->first) == aThis.end()) {
      return "current model had no attribute '" + anOrigIter->first + "'";
    }
  }
  return "";
}

static void dumpArray(std::ostringstream& theOutput, const double theArray[],
                      int theSize, int thePrecision = PRECISION)
{
  for (int i = 0; i < theSize; ++i) {
    if (i > 0)
      theOutput << " ";
    theOutput << std::fixed << std::setprecision(thePrecision)
              << (fabs(theArray[i]) < TOLERANCE ? 0.0 : theArray[i]);
  }
}

std::string ModelHighAPI_FeatureStore::dumpAttr(const AttributePtr& theAttr) {
  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
  FeaturePtr aFeatOwner = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttr->owner());
  if (aFeatOwner.get() && !aFactory->isCase(aFeatOwner, theAttr->id())) {
    return "__notcase__";
  }
  std::string aType = theAttr->attributeType();

  // do not check selection of the filter,
  // because there is neither parametric update nor dump support yet.
  FiltersFeaturePtr aFilter = std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(aFeatOwner);
  if (aFilter && (aType == ModelAPI_AttributeSelection::typeId() ||
                  aType == ModelAPI_AttributeSelectionList::typeId()))
    return "__filter_selection__";

  std::ostringstream aResult;
  if (!theAttr->isInitialized()) {
    if (aType == ModelAPI_AttributeBoolean::typeId()) {
      // special case for Boolean attribute (mean it false if not initialized)
      aResult << false;
      return aResult.str();
    } else if (aType == ModelAPI_AttributeString::typeId()) {
      // special case for attribute "SolverError"
      if (theAttr->id() == "SolverError" &&
          std::dynamic_pointer_cast<ModelAPI_Feature>(theAttr->owner())->getKind() == "Sketch")
        return "";
    }

    return "__notinitialized__";
  }
  if (aType == ModelAPI_AttributeDocRef::typeId()) {
    AttributeDocRefPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDocRef>(theAttr);
    DocumentPtr aDoc = ModelAPI_Session::get()->moduleDocument();
    if (anAttr->value() != aDoc) {
      ResultPtr aRes = ModelAPI_Tools::findPartResult(aDoc, anAttr->value());
      if (aRes.get()) {
        // Part result name (the same as saved file name)
        aResult<< Locale::Convert::toString(aRes->data()->name());
      }
    } else {
      aResult<<aDoc->kind(); // PartSet
    }
  } else if (aType == ModelAPI_AttributeInteger::typeId()) {
    AttributeIntegerPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttr);
    // do not dump a type of ConstraintAngle, because it can be changed due dumping
    if (anAttr->id() == "AngleType" || anAttr->id() == "AngleTypePrevious") {
      return "";
    } else if (anAttr->id() == "LocationType") {
      return "__notinitialized__";
    }
    if (anAttr->text().empty())
      aResult<<anAttr->value();
    else
      aResult << Locale::Convert::toString(anAttr->text());
  } else if (aType == ModelAPI_AttributeDouble::typeId()) {
    AttributeDoublePtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttr);
    if (anAttr->id() == "ConstraintValue") {
      // do not dump a value of constraint if it is ConstraintAngle,
      // because this value depends on the angle type
      FeaturePtr anOwner = ModelAPI_Feature::feature(anAttr->owner());
      if (anOwner && anOwner->getKind() == "SketchConstraintAngle")
        return "";
    }
    int aPrecision = PRECISION;
    // Special case - precision for the arc angle. It is calculated with tolerance 1e-4,
    // so the value has only 4 correct digits
    if (anAttr->id() == "ArcAngle")
      aPrecision = 1;
    if (anAttr->text().empty()) {
      double aVal = anAttr->value();
      dumpArray(aResult, &aVal, 1, aPrecision);
    } else
      aResult << Locale::Convert::toString(anAttr->text());
  } else if (aType == ModelAPI_AttributeBoolean::typeId()) {
    AttributeBooleanPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(theAttr);
    // do not dump internal flags of ConstraintAngle
    if (anAttr->id() == "AngleReversedLine1" || anAttr->id() == "AngleReversedLine2") {
      return "";
    }
    aResult<<anAttr->value();
  } else if (aType == ModelAPI_AttributeString::typeId()) {
    AttributeStringPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttr);
    // do not dump solver DOF for sketch as it may be changed unexpectedly
    if (anAttr->id() == "SolverDOF") {
      return "";
    }
    // do not dump file path for Image as it is changed after DumpPython
    if (aFeatOwner->getKind() == "ImportImage" &&
        anAttr->id() == "file_path") {
      return "";
    }
    aResult<<anAttr->value();
  } else if (aType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttr);
    if (anAttr->value().get()) {
      aResult<< Locale::Convert::toString(anAttr->value()->data()->name());
    } else {
      aResult<<"__empty__";
    }
  } else if (aType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttr);
    if (anAttr->context().get())
      aResult<< Locale::Convert::toString(anAttr->namingName());
    else
      aResult<<"__notinitialized__";
  } else if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttr);
    for(int a = 0; a < anAttr->size(); a++) {
      if (a != 0)
        aResult<<" ";
      aResult<< Locale::Convert::toString(anAttr->value(a)->namingName());
    }
  } else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttr);
    ObjectPtr anObj = anAttr->isObject() ? anAttr->object() : anAttr->attr()->owner();
    if (anObj.get()) {
      aResult<< Locale::Convert::toString(anObj->data()->name());
      if (!anAttr->isObject()) {
        aResult<<" "<<anAttr->attr()->id();
      }
    } else {
      aResult<<"__empty__";
    }
  } else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr anAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttr);
    // for sketch sub-features the empty values may be skipped and order is not important
    bool isSketchFeatures = anAttr->id() == "Features" &&
      std::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->owner())->getKind() == "Sketch";
    std::list<ObjectPtr> aList = anAttr->list();
    std::list<std::string> aResList; // list of resulting strings
    for(std::list<ObjectPtr>::iterator aL = aList.begin(); aL != aList.end(); aL++) {
      if (aL->get()) {
        if (isSketchFeatures) {
          // do not control construction features of an ellipse and other
          FeaturePtr aFeature = ModelAPI_Feature::feature(*aL);
          //if (aFeature->getKind() == "SketchConstraintCoincidenceInternal")
          //  continue; // skip internal constraints
          std::string aStr = aFeature->getKind().substr(0, 16);
          if (aStr == "SketchConstraint")
            continue; // no need to dump and check constraints
        }
        aResList.push_back(Locale::Convert::toString((*aL)->data()->name()));
      } else if (!isSketchFeatures) {
        aResList.push_back("__empty__");
      }
    }
    if (isSketchFeatures)
      aResList.sort();
    for(std::list<std::string>::iterator aR = aResList.begin(); aR != aResList.end(); aR++) {
      aResult<<*aR<<" ";
    }
  } else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theAttr);
    std::list<std::pair<ObjectPtr, AttributePtr> > aList = anAttr->list();
    std::list<std::pair<ObjectPtr, AttributePtr> >::iterator aL = aList.begin();
    for(; aL != aList.end(); aL++) {
      if (aL != aList.begin())
        aResult<<" ";
      ObjectPtr anObj = aL->second.get() ? aL->second->owner() : aL->first;
      if (anObj.get()) {
        aResult<< Locale::Convert::toString(anObj->data()->name());
        if (aL->second.get()) {
          aResult<<" "<<aL->second->id();
        }
      } else {
        aResult<<"__empty__";
      }
    }
  } else if (aType == ModelAPI_AttributeIntArray::typeId()) {
    if (theAttr->id() == "Color") {
      ResultConstructionPtr aResConstr =
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theAttr->owner());
      if (aResConstr.get())
        return "__notinitialized__";
    }
    AttributeIntArrayPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeIntArray>(theAttr);
    for(int a = 0; a < anAttr->size(); a++)
      aResult<<anAttr->value(a)<<" ";
  } else if (aType == ModelAPI_AttributeDoubleArray::typeId()) {
    AttributeDoubleArrayPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(theAttr);
    for(int a = 0; a < anAttr->size(); a++)
      aResult<<anAttr->value(a)<<" ";
  } else if (aType == ModelAPI_AttributeStringArray::typeId()) {
    AttributeStringArrayPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeStringArray>(theAttr);
    for(int a = 0; a < anAttr->size(); a++)
      aResult<<"'"<<anAttr->value(a)<<"'"<<" ";
  } else if (aType == ModelAPI_AttributeTables::typeId()) {
    AttributeTablesPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeTables>(theAttr);
    aResult<<anAttr->tables()<<"x"<<anAttr->rows()<<"x"<<anAttr->columns()<<" ";
    for(int aTab = 0; aTab < anAttr->tables(); aTab++) {
      for(int aRow = 0; aRow < anAttr->rows(); aRow++) {
        for( int aCol = 0; aCol < anAttr->columns(); aCol++) {
          switch(anAttr->type()) {
          case ModelAPI_AttributeTables::BOOLEAN:
            aResult<<anAttr->value(aRow, aCol, aTab).myBool<<" ";
            break;
          case ModelAPI_AttributeTables::INTEGER:
            aResult<<anAttr->value(aRow, aCol, aTab).myInt<<" ";
            break;
          case ModelAPI_AttributeTables::DOUBLE:
            aResult<<anAttr->value(aRow, aCol, aTab).myDouble<<" ";
            break;
          case ModelAPI_AttributeTables::STRING:
            aResult<<"'"<<anAttr->value(aRow, aCol, aTab).myStr.c_str()<<"' ";
            break;
          }
        }
      }
    }
  } else if (aType == GeomDataAPI_Point::typeId()) {
    AttributePointPtr anAttr = std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttr);
    double aValues[3] = {anAttr->x(), anAttr->y(), anAttr->z()};
    dumpArray(aResult, aValues, 3);
  } else if (aType == GeomDataAPI_Dir::typeId()) {
    if (theAttr->id() == "DistanceDirection")
      return "__notcase__";
    AttributeDirPtr anAttr = std::dynamic_pointer_cast<GeomDataAPI_Dir>(theAttr);
    double aValues[3] = {anAttr->x(), anAttr->y(), anAttr->z()};
    dumpArray(aResult, aValues, 3);
  } else if (aType == GeomDataAPI_Point2D::typeId()) {
    // do not dump flyout point for constraints as it may be changed unexpectedly,
    // also do not dump selection points controlled by GUI
    if (theAttr->id() == "ConstraintFlyoutValuePnt" ||
        theAttr->id() == "SelectedPointA" || theAttr->id() == "SelectedPointB")
      return "__notinitialized__";
    AttributePoint2DPtr anAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttr);
    double aValues[2] = {anAttr->x(), anAttr->y()};
    dumpArray(aResult, aValues, 2);
  } else {
    aResult<<"__unknownattribute__";
  }
  return aResult.str();
}

std::string ModelHighAPI_FeatureStore::dumpShape(std::shared_ptr<GeomAPI_Shape>& theShape) {
  if (theShape->isNull()) {
    return "null";
  }
  std::ostringstream aResult;
  // output the number of shapes of different types
  GeomAPI_Shape::ShapeType aType = GeomAPI_Shape::COMPOUND;
  for (; aType <= GeomAPI_Shape::VERTEX; aType = GeomAPI_Shape::ShapeType((int)aType + 1)) {
    GeomAPI_ShapeExplorer anExp(theShape, aType);
    int aCount = 0;
    for (; anExp.more(); anExp.next()) aCount++;
    aResult << anExp.current()->shapeTypeStr().c_str() <<  ": " << aCount << std::endl;
  }
  // output the main characteristics
  double aVolume = GeomAlgoAPI_ShapeTools::volume(theShape);
  if (aVolume > 1.e-5) {
    aResult<<"Volume: ";
    // volumes of too huge shapes write in the scientific format
    if (aVolume >= 1.e5)
      aResult<<std::scientific<<std::setprecision(7);
    else
      aResult<<std::fixed<<std::setprecision(3);
    aResult<<aVolume<<std::endl;
  }
  double anArea = GeomAlgoAPI_ShapeTools::area(theShape);
  if (anArea > 1.e-5) {
    aResult << "Area: ";
    // volumes of too huge shapes write in the scientific format
    if (anArea >= 1.e5)
      aResult << std::scientific << std::setprecision(7);
    else
      aResult << std::fixed << std::setprecision(3);
    aResult << anArea << std::endl;
  }
  std::shared_ptr<GeomAPI_Pnt> aCenter = GeomAlgoAPI_ShapeTools::centreOfMass(theShape);
  aResult<<"Center of mass: ";
  double aCenterVals[3] = {aCenter->x(), aCenter->y(), aCenter->z()};
  dumpArray(aResult, aCenterVals, 3, 5);
  aResult<<std::endl;
  return aResult.str();
}
