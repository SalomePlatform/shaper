// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_BooleanArguments.cpp
// Created:     30 June 2015
// Author:      Dmitry Bobylev

#include <GeomValidators_BooleanArguments.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>

//=================================================================================================
bool GeomValidators_BooleanArguments::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                              const std::list<std::string>& theArguments,
                                              std::string& theError) const
{
  if(theArguments.size() != 3) {
    return false;
  }

  int anObjectsNb = 0, aToolsNb = 0;
  int anOperationType = 0;

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();

  std::shared_ptr<ModelAPI_AttributeSelectionList> anAttrSelList = theFeature->selectionList(*anIt);
  if(anAttrSelList) {
    anObjectsNb = anAttrSelList->size();
  }
  anIt++;

  anAttrSelList = theFeature->selectionList(*anIt);
  if(anAttrSelList) {
    aToolsNb = anAttrSelList->size();
  }
  anIt++;

  std::shared_ptr<ModelAPI_AttributeInteger> anAttrInt = theFeature->integer(*anIt);
  if(anAttrInt) {
    anOperationType = anAttrInt->value();
  }

  if(anOperationType == 1 && (anObjectsNb + aToolsNb > 1)) {
    return true;
  } else if (anOperationType != 1 && anObjectsNb > 0 && aToolsNb > 0) {
    return true;
  }

  theError = "Not enough arguments";
  return false;
}

//=================================================================================================
bool GeomValidators_BooleanArguments::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  if(theAttribute == "main_objects" || theAttribute == "tool_objects") {
    return true;
  }

  return false;
}
