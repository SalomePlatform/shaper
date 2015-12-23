// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Expression.cpp
// Created:     5 Aug 2015
// Author:      Sergey POKHODENKO

#include "ModelAPI_Expression.h"

ModelAPI_Expression::ModelAPI_Expression()
{

}

ModelAPI_Expression::~ModelAPI_Expression()
{

}

bool ModelAPI_Expression::isInitialized()
{
  return myIsInitialized;
}

void ModelAPI_Expression::setInitialized()
{
  myIsInitialized = true;
}

ModelAPI_ExpressionDouble::ModelAPI_ExpressionDouble()
{

}

ModelAPI_ExpressionInteger::ModelAPI_ExpressionInteger()
{

}
