// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_ZeroOffset.cpp
// Created:     13 May 2015
// Author:      Dmitry Bobylev

#include <GeomValidators_ZeroOffset.h>

#include <GeomAPI_Shape.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>

//=================================================================================================
bool GeomValidators_ZeroOffset::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                        const std::list<std::string>& theArguments,
                                        std::string& theError) const
{
  if(theArguments.size() != 8) {
    return false;
  }

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();

  std::string aSelectedMethod;
  if(theFeature->string(*anIt)) {
    aSelectedMethod = theFeature->string(*anIt)->value();
  }
  anIt++;
  std::string aCreationMethod = *anIt;
  anIt++;
  
  double aToSize = 0.0;
  double aFromSize = 0.0;

  if(theFeature->real(*anIt)) {
    aToSize = theFeature->real(*anIt)->value();
  }
  anIt++;
  if(theFeature->real(*anIt)) {
    aFromSize = theFeature->real(*anIt)->value();
  }
  anIt++;

  if(aSelectedMethod == aCreationMethod) {
    if(aToSize == 0.0 && aFromSize == 0.0) {
      return false;
    } else {
      return true;
    }
  }

  std::shared_ptr<GeomAPI_Shape> aToShape;
  std::shared_ptr<GeomAPI_Shape> aFromShape;

  std::shared_ptr<ModelAPI_AttributeSelection> anAttrSel = theFeature->selection(*anIt);
  if(anAttrSel) {
    aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anAttrSel->value());
    if(aToShape.get() == NULL && anAttrSel->context().get() != NULL) {
      aToShape =  anAttrSel->context()->shape();
    }
  }
  anIt++;

  std::shared_ptr<ModelAPI_AttributeDouble> anAttrDouble = theFeature->real(*anIt);
  if(anAttrDouble) {
    aToSize = anAttrDouble->value();
  }
  anIt++;

  anAttrSel = theFeature->selection(*anIt);
  if(anAttrSel) {
    aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anAttrSel->value());
    if(aFromShape.get() == NULL && anAttrSel->context().get() != NULL) {
      aFromShape = anAttrSel->context()->shape();
    }
  }
  anIt++;

  anAttrDouble = theFeature->real(*anIt);
  if(anAttrDouble) {
    aFromSize = anAttrDouble->value();
  }

  if(((!aFromShape && !aToShape) || ((aFromShape && aToShape) && aFromShape->isEqual(aToShape)))
    && (aFromSize == 0.0 && aToSize == 0.0)) {
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
