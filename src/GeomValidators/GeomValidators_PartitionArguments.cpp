// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_PartitionArguments.cpp
// Created:     17 September 2015
// Author:      Dmitry Bobylev

#include <GeomValidators_PartitionArguments.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>

//=================================================================================================
bool GeomValidators_PartitionArguments::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                              const std::list<std::string>& theArguments,
                                              std::string& theError) const
{
  if(theArguments.size() != 3) {
    theError = "Wrong number of arguments (expected 3).";
    return false;
  }

  int anObjectsNb = 0, aToolsNb = 0;
  bool isCombine = true;

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

  std::shared_ptr<ModelAPI_AttributeBoolean> anAttrBool = theFeature->boolean(*anIt);
  if(anAttrBool) {
    isCombine = anAttrBool->value();
  }

  if((anObjectsNb > 0 && aToolsNb > 0) || (isCombine && anObjectsNb != 0 && (anObjectsNb + aToolsNb > 1))) {
    return true;
  }

  theError = "Not enough arguments";
  return false;
}

//=================================================================================================
bool GeomValidators_PartitionArguments::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  if(theAttribute == "tool_objects") {
    return true;
  }

  return false;
}
