// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_MinObjectsSelected.cpp
// Created:     30 June 2015
// Author:      Dmitry Bobylev

#include <GeomValidators_MinObjectsSelected.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>

//=================================================================================================
bool GeomValidators_MinObjectsSelected::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                const std::list<std::string>& theArguments,
                                                Events_InfoMessage& theError) const
{
  if(theArguments.size() != 2) {
    theError =
      "Error: Wrong number of arguments (expected 2): selection list id and min number of objects";
    return false;
  }

  std::string aSelectionListId = theArguments.front();
  AttributeSelectionListPtr anAttrSelList = theFeature->selectionList(aSelectionListId);
  if(!anAttrSelList.get()) {
    theError = "Error: Could not get attribute \"%1\".";
    theError.arg(aSelectionListId);
    return false;
  }
  int anObjectsNb = anAttrSelList->size();

  int aMinObjectsNb = atoi(theArguments.back().c_str());

  if(anObjectsNb < aMinObjectsNb) {
    theError = "Error: Attribute \"%1\" should contain at least %2 items.";
    theError.arg(aSelectionListId).arg(theArguments.back());
    return false;
  }

  return true;
}

//================================================================================================
bool GeomValidators_MinObjectsSelected::isNotObligatory(std::string theFeature,
                                                        std::string theAttribute)
{
  return false;
}
