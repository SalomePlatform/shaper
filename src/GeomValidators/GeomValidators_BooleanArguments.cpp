// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_BooleanArguments.cpp
// Created:     30 June 2015
// Author:      Dmitry Bobylev

#include <GeomValidators_BooleanArguments.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_Tools.h>

//=================================================================================================
bool GeomValidators_BooleanArguments::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                              const std::list<std::string>& theArguments,
                                              Events_InfoMessage& theError) const
{
  if(theArguments.size() != 3) {
    theError = "Wrong number of arguments (expected 3).";
    return false;
  }

  int anObjectsNb = 0, aToolsNb = 0;
  int anOperationType = 0;

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();

  bool isAllInSameCompSolid = true;
  ResultCompSolidPtr aCompSolid;

  AttributeSelectionListPtr anAttrSelList = theFeature->selectionList(*anIt);
  if(anAttrSelList) {
    anObjectsNb = anAttrSelList->size();
    for(int anIndex = 0; anIndex < anObjectsNb; ++anIndex) {
      AttributeSelectionPtr anAttr = anAttrSelList->value(anIndex);
      ResultPtr aContext = anAttr->context();
      ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
      if(aResCompSolidPtr.get()) {
        if(aCompSolid.get()) {
          isAllInSameCompSolid = aCompSolid == aResCompSolidPtr;
        } else {
          aCompSolid = aResCompSolidPtr;
        }
      } else {
        isAllInSameCompSolid = false;
      }
    }
  }
  anIt++;


  anAttrSelList = theFeature->selectionList(*anIt);
  if(anAttrSelList) {
    aToolsNb = anAttrSelList->size();
    for(int anIndex = 0; anIndex < aToolsNb; ++anIndex) {
      AttributeSelectionPtr anAttr = anAttrSelList->value(anIndex);
      ResultPtr aContext = anAttr->context();
      ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
      if(aResCompSolidPtr.get()) {
        if(aCompSolid.get()) {
          isAllInSameCompSolid = aCompSolid == aResCompSolidPtr;
        } else {
          aCompSolid = aResCompSolidPtr;
        }
      } else {
        isAllInSameCompSolid = false;
      }
    }
  }
  anIt++;

  std::shared_ptr<ModelAPI_AttributeInteger> anAttrInt = theFeature->integer(*anIt);
  if(anAttrInt) {
    anOperationType = anAttrInt->value();
  }

  if(anOperationType == 1) {
    // Fuse operation
    if(anObjectsNb + aToolsNb < 2) {
      theError = "Not enough arguments for Fuse operation.";
      return false;
    } else if(isAllInSameCompSolid) {
      theError = "Operations only between sub-shapes of the same shape not allowed.";
      return false;
    }
  } else {
    if(anObjectsNb < 1) {
      theError = "Objects not selected.";
      return false;
    }
    if(aToolsNb < 1) {
      theError = "Tools not selected.";
      return false;
    }
    if(isAllInSameCompSolid) {
      theError = "Operations only between sub-shapes of the same shape not allowed.";
      return false;
    }
  }

  return true;
}

//=================================================================================================
bool GeomValidators_BooleanArguments::isNotObligatory(std::string theFeature,
                                                      std::string theAttribute)
{
  if(theAttribute == "main_objects" || theAttribute == "tool_objects") {
    return true;
  }

  return false;
}
