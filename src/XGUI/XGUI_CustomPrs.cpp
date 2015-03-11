// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_CustomPrs.cpp
// Created:     10 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <XGUI_CustomPrs.h>

#include <ModelAPI_AttributeIntArray.h>
#include <Config_PropManager.h>

#include <vector>


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

void getDefaultColor(ResultPtr theResult, AISObjectPtr thePrs, std::vector<int>& theColor)
{
  theColor.clear();
  // get default color from the preferences manager for the given result
  if (theColor.empty()) {
    std::string aSection, aName, aDefault;
    theResult->colorConfigInfo(aSection, aName, aDefault);
    if (!aSection.empty() && !aName.empty()) {
      theColor = Config_PropManager::color(aSection, aName, aDefault);
    }
  }
  if (theColor.empty()) // all AIS objects, where the color is not set, a white.
    // The color should be defined in XML or set in the attribute
    theColor = Config_PropManager::color("Visualization", "object_default_color", "#ffffff");
}

bool XGUI_CustomPrs::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                           std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs)
{
  std::vector<int> aColor;

  getColor(theResult, aColor);
  if (aColor.empty())
    getDefaultColor(theResult, thePrs, aColor);

  return !aColor.empty() && thePrs->setColor(aColor[0], aColor[1], aColor[2]);
}
