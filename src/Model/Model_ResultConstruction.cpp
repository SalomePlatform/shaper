// File:        ModelAPI_ResultConstruction.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultConstruction.h>

void Model_ResultConstruction::setShape(boost::shared_ptr<GeomAPI_Shape> theShape)
{
  myShape = theShape;
}

boost::shared_ptr<GeomAPI_Shape>& Model_ResultConstruction::shape()
{
  return myShape;
}

/*
boost::shared_ptr<ModelAPI_Feature> Model_ResultConstruction::owner()
{
  return myOwner;
}*/

Model_ResultConstruction::Model_ResultConstruction()
{
}
