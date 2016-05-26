// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_MinObjectsSelected.cpp
// Created:     30 June 2015
// Author:      Dmitry Bobylev

#include <GeomValidators_MinObjectsSelected.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>

//=================================================================================================
bool GeomValidators_MinObjectsSelected::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                const std::list<std::string>& theArguments,
                                                std::string& theError) const
{
  if(theArguments.size() != 2) {
    theError = "Error: Wrong number of arguments (expected 2): selection list id and min number of objects";
    return false;
  }

  std::string aSelectionListId = theArguments.front();
  AttributeSelectionListPtr anAttrSelList = theFeature->selectionList(aSelectionListId);
  if(!anAttrSelList.get()) {
    theError = "Error: Could not get attribute \"" + aSelectionListId + "\".";
    return false;
  }
  int anObjectsNb = anAttrSelList->size();

  int aMinObjectsNb = atoi(theArguments.back().c_str());

  if(anObjectsNb < aMinObjectsNb) {
    theError = "Error: Attribute \"" + aSelectionListId + "\" should contain at least "
      + theArguments.back() + " items.";
    return false;
  }

  return true;
}

//=================================================================================================
bool GeomValidators_MinObjectsSelected::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  return false;
}
