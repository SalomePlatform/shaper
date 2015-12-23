// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeValidator.cpp
// Created:     29 July 2015
// Author:      Sergey POKHODENKO

#include "Model_AttributeValidator.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

bool Model_AttributeValidator::isValid(const AttributePtr& theAttribute, 
                                       const std::list<std::string>& theArguments, 
                                       std::string& theError) const
{
  if (theAttribute->attributeType() == ModelAPI_AttributeInteger::typeId()) {
    AttributeIntegerPtr anAttribue =
        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttribute);
    if (!anAttribue->expressionError().empty()) {
      theError = anAttribue->expressionError();
      return false;
    }
  } else
  if (theAttribute->attributeType() == ModelAPI_AttributeDouble::typeId()) {
    AttributeDoublePtr anAttribue = 
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
    if (!anAttribue->expressionError().empty()) {
      theError = anAttribue->expressionError();
      return false;
    }
  } else
  if (theAttribute->attributeType() == GeomDataAPI_Point::typeId()) {
    AttributePointPtr anAttribue = 
        std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);

    const char* aComponent[] = {"X", "Y", "Z"};
    std::string anErrorMessage;
    for (int i = 0; i < 3; ++i) {
      if (!anAttribue->expressionError(i).empty())
        anErrorMessage.append("\n").append(aComponent[i]).append(": ").append(anAttribue->expressionError(i));
    }
    if (!anErrorMessage.empty()) {
      theError = anErrorMessage;
      return false;
    }
  } else
  if (theAttribute->attributeType() == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr anAttribue = 
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);

    const char* aComponent[] = {"X", "Y"};
    std::string anErrorMessage;
    for (int i = 0; i < 2; ++i) {
      if (!anAttribue->expressionError(i).empty())
        anErrorMessage.append("\n").append(aComponent[i]).append(": ").append(anAttribue->expressionError(i));
    }
    if (!anErrorMessage.empty()) {
      theError = anErrorMessage;
      return false;
    }
  }
  return true;
}

