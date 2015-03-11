// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultConstruction.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultConstruction.h>

#include <ModelAPI_AttributeIntArray.h>
#include <Config_PropManager.h>

void Model_ResultConstruction::initAttributes()
{
  // append the color attribute
  DataPtr aData = data();
  aData->addAttribute(COLOR_ID(), ModelAPI_AttributeIntArray::type());
  AttributeIntArrayPtr aColorAttr = aData->intArray(COLOR_ID());
  std::vector<int> aRGB;
  aRGB = Config_PropManager::color("Visualization", "result_construction_color", DEFAULT_COLOR());
  aColorAttr->setSize(3);
  aColorAttr->setValue(0, aRGB[0]);
  aColorAttr->setValue(1, aRGB[1]);
  aColorAttr->setValue(2, aRGB[2]);
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
