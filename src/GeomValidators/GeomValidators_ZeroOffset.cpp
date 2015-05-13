// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_ZeroOffset.h
// Created:     13 May 2015
// Author:      Dmitry Bobylev

#include <GeomValidators_ZeroOffset.h>

#include <GeomAPI_Shape.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>

//=================================================================================================
bool GeomValidators_ZeroOffset::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                        const std::list<std::string>& theArguments) const
{
  if(theArguments.size() < 4) {
    return false;
  }

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();

  std::shared_ptr<GeomAPI_Shape> aFromShape;
  std::shared_ptr<GeomAPI_Shape> aToShape;

  std::shared_ptr<ModelAPI_AttributeSelection> anAttrSel = theFeature->selection(*anIt);
  if(anAttrSel) {
    aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anAttrSel->value());
  }
  anIt++;
  anAttrSel = theFeature->selection(*anIt);
  if(anAttrSel) {
    aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anAttrSel->value());
  }
  anIt++;

  double aFromOffset = 0.0;
  double aToOffset = 0.0;

  std::shared_ptr<ModelAPI_AttributeDouble> anAttrDouble = theFeature->real(*anIt);
  if(anAttrDouble) {
    aFromOffset = anAttrDouble->value();
  }
  anIt++;
  anAttrDouble = theFeature->real(*anIt);
  if(anAttrDouble) {
    aToOffset = anAttrDouble->value();
  }

  if(((!aFromShape && !aToShape) || ((aFromShape && aToShape) && aFromShape->isEqual(aToShape)))
    && (aFromOffset == 0.0 && aToOffset == 0.0)) {
    return false;
  }

  return true;
}

//=================================================================================================
bool GeomValidators_ZeroOffset::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  if(theAttribute == "from_object" || theAttribute == "to_object") {
    return true;
  }

  return false;
}
