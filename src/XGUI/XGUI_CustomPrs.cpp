// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_CustomPrs.cpp
// Created:     10 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <XGUI_CustomPrs.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_IModule.h>

#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_Session.h>
#include <Config_PropManager.h>

#include <Events_Error.h>

#include <vector>
#include <QColor>


void getColor(ResultPtr theResult, std::vector<int>& theColor)
{
  theColor.clear();
  // get color from the attribute of the result
  if (theResult.get() != NULL && theResult->data()->attribute(ModelAPI_Result::COLOR_ID()).get() != NULL) {
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
      theColor = Config_PropManager::color(aSection, aName, aDefault);
    }
  }
  if (!isEmptyColorValid && theColor.empty()) {
    // all AIS objects, where the color is not set, are in black.
    // The color should be defined in XML or set in the attribute
    theColor = Config_PropManager::color("Visualization", "object_default_color", "#000000");
    Events_Error::send("A default color is not defined in the preferences for this kind of result");
  }
}

XGUI_CustomPrs::XGUI_CustomPrs(XGUI_Workshop* theWorkshop)
: myWorkshop(theWorkshop)
{
}

void XGUI_CustomPrs::getResultColor(ResultPtr theResult, std::vector<int>& theColor)
{
  getColor(theResult, theColor);
  if (theColor.empty())
    getDefaultColor(theResult, false, theColor);
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
      QColor aNewColor = QColor::fromHsvF(aQColor.hueF(), aQColor.saturationF()/3., aQColor.valueF());
      aColor[0] = aNewColor.red();
      aColor[1] = aNewColor.green();
      aColor[2] = aNewColor.blue();
    }
    aCustomized = !aColor.empty() && thePrs->setColor(aColor[0], aColor[1], aColor[2]);
  }
  else {
   if (!aCustomized) {
      ModuleBase_IModule* aModule = myWorkshop->module();
      aCustomized = aModule->customisePresentation(theResult, thePrs, theCustomPrs);
    }
  }
  return aCustomized;
}
