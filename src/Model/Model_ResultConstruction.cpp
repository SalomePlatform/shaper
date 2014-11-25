// File:        ModelAPI_ResultConstruction.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultConstruction.h>

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
