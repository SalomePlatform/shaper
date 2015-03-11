// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultConstruction.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultConstruction.h>

#include <ModelAPI_AttributeIntArray.h>
#include <Config_PropManager.h>

void Model_ResultConstruction::initAttributes()
{
  // append the color attribute. It is empty, the attribute will be filled by a request
  DataPtr aData = data();
  aData->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::type());
}

void Model_ResultConstruction::colorConfigInfo(std::string& theSection, std::string& theName,
                                       std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_construction_color";
  theDefault = DEFAULT_COLOR();
}

void Model_ResultConstruction::setShape(std::shared_ptr<GeomAPI_Shape> theShape)
{
  myShape = theShape;
}

std::shared_ptr<GeomAPI_Shape> Model_ResultConstruction::shape()
{
  return myShape;
}

Model_ResultConstruction::Model_ResultConstruction()
{
  myIsInHistory = true;
  setIsConcealed(false);
}

void Model_ResultConstruction::setIsInHistory(const bool isInHistory)
{
  myIsInHistory = isInHistory;
}
