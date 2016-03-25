// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Validators.cpp
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_Validators.h"

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>

//=================================================================================================
bool FeaturesPlugin_PipeLocationsValidator::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                    const std::list<std::string>& theArguments,
                                                    std::string& theError) const
{
  static const std::string aCreationMethodID = "creation_method";
  static const std::string aBaseObjectsID = "base_objects";
  static const std::string aLocationsID = "locations_objects";


  if(theFeature->getKind() != "Pipe") {
    theError = "Feature \"" + theFeature->getKind() + "\" does not supported by this validator.";
    return false;
  }

  AttributeStringPtr aCreationMethodAttr = theFeature->string(aCreationMethodID);
  if(!aCreationMethodAttr.get()) {
    theError = "Could not get \"" + aCreationMethodID + "\" attribute.";
    return false;
  }

  if(aCreationMethodAttr->value() != "locations") {
    return true;
  }

  AttributeSelectionListPtr aBaseObjectsSelectionList = theFeature->selectionList(aBaseObjectsID);
  if(!aBaseObjectsSelectionList.get()) {
    theError = "Could not get \"" + aBaseObjectsID + "\" attribute.";
    return false;
  }

  AttributeSelectionListPtr aLocationsSelectionList = theFeature->selectionList(aLocationsID);
  if(!aLocationsSelectionList.get()) {
    theError = "Could not get \"" + aBaseObjectsID + "\" attribute.";
    return false;
  }

  if(aLocationsSelectionList->size() > 0 && aLocationsSelectionList->size() != aBaseObjectsSelectionList->size()) {
    theError = "Number of locations should be the same as base objects.";
    return false;
  }

  return true;
}

//=================================================================================================
bool FeaturesPlugin_PipeLocationsValidator::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  if(theFeature == "Pipe" && theAttribute == "locations") {
    return true;
  }
  return false;
}