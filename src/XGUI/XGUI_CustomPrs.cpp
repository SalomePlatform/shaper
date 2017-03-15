// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_CustomPrs.cpp
// Created:     10 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <XGUI_CustomPrs.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_IModule.h>

#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_ShapeExplorer.h>

#include <Config_PropManager.h>

#include <Events_InfoMessage.h>

#include <vector>
#include <QColor>

double getDeflection(const ResultPtr& theResult)
{
  double aDeflection = -1;
  // get color from the attribute of the result
  if (theResult.get() != NULL &&
      theResult->data()->attribute(ModelAPI_Result::DEFLECTION_ID()).get() != NULL) {
    AttributeDoublePtr aDoubleAttr = theResult->data()->real(ModelAPI_Result::DEFLECTION_ID());
    if (aDoubleAttr.get() && aDoubleAttr->isInitialized()) {
      double aValue = aDoubleAttr->value();
      if (aValue > 0) /// zero value should not be used as a deflection(previous studies)
        aDeflection = aDoubleAttr->value();
    }
  }
  return aDeflection;
}

void getColor(const ResultPtr& theResult, std::vector<int>& theColor)
{
  theColor.clear();
  // get color from the attribute of the result
  if (theResult.get() != NULL &&
      theResult->data()->attribute(ModelAPI_Result::COLOR_ID()).get() != NULL) {
    AttributeIntArrayPtr aColorAttr = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
    if (aColorAttr.get() && aColorAttr->size()) {
      theColor.push_back(aColorAttr->value(0));
      theColor.push_back(aColorAttr->value(1));
      theColor.push_back(aColorAttr->value(2));
    }
  }
}

void XGUI_CustomPrs::getDefaultColor(ObjectPtr theObject, const bool isEmptyColorValid,
                                     std::vector<int>& theColor)
{
  theColor.clear();
  // get default color from the preferences manager for the given result
  if (theColor.empty()) {
    std::string aSection, aName, aDefault;
    theObject->colorConfigInfo(aSection, aName, aDefault);
    if (!aSection.empty() && !aName.empty()) {
      theColor = Config_PropManager::color(aSection, aName);
    }
  }
  if (!isEmptyColorValid && theColor.empty()) {
    // all AIS objects, where the color is not set, are in black.
    // The color should be defined in XML or set in the attribute
    theColor = Config_PropManager::color("Visualization", "object_default_color");
    Events_InfoMessage("XGUI_CustomPrs",
      "A default color is not defined in the preferences for this kind of result").send();
  }
}

double XGUI_CustomPrs::getDefaultDeflection(const ObjectPtr& theObject)
{
  double aDeflection = -1;
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (aResult.get()) {
    bool isConstruction = false;

    std::string aResultGroup = aResult->groupName();
    if (aResultGroup == ModelAPI_ResultConstruction::group())
      isConstruction = true;
    else if (aResultGroup == ModelAPI_ResultBody::group()) {
      GeomShapePtr aGeomShape = aResult->shape();
      if (aGeomShape.get()) {
        // if the shape could not be exploded on faces, it contains only wires, edges, and vertices
        // correction of deviation for them should not influence to the application performance
        GeomAPI_ShapeExplorer anExp(aGeomShape, GeomAPI_Shape::FACE);
        isConstruction = !anExp.more();
      }
    }
    if (isConstruction)
      aDeflection = Config_PropManager::real("Visualization", "construction_deflection");
    else
      aDeflection = Config_PropManager::real("Visualization", "body_deflection");
  }
  return aDeflection;
}

XGUI_CustomPrs::XGUI_CustomPrs(XGUI_Workshop* theWorkshop)
: myWorkshop(theWorkshop)
{
}

void XGUI_CustomPrs::getResultColor(const ResultPtr& theResult, std::vector<int>& theColor)
{
  getColor(theResult, theColor);
  if (theColor.empty())
    getDefaultColor(theResult, false, theColor);
}

double XGUI_CustomPrs::getResultDeflection(const ResultPtr& theResult)
{
  double aDeflection = getDeflection(theResult);
  if (aDeflection < 0)
    aDeflection = getDefaultDeflection(theResult);
  return aDeflection;
}

bool XGUI_CustomPrs::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                           std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs)
{
  bool aCustomized = false;
  if (theResult.get()) {
    std::vector<int> aColor;
    getResultColor(theResult, aColor);

    SessionPtr aMgr = ModelAPI_Session::get();
    if (aMgr->activeDocument() != theResult->document()) {
      QColor aQColor(aColor[0], aColor[1], aColor[2]);
      QColor aNewColor =
        QColor::fromHsvF(aQColor.hueF(), aQColor.saturationF()/3., aQColor.valueF());
      aColor[0] = aNewColor.red();
      aColor[1] = aNewColor.green();
      aColor[2] = aNewColor.blue();
    }
    aCustomized = !aColor.empty() && thePrs->setColor(aColor[0], aColor[1], aColor[2]);

    aCustomized = thePrs->setDeflection(getResultDeflection(theResult)) | aCustomized;
  }
  ModuleBase_IModule* aModule = myWorkshop->module();
  aCustomized = aModule->customisePresentation(theResult, thePrs, theCustomPrs) || aCustomized;
  return aCustomized;
}
