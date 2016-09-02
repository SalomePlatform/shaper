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
bool ModelAPI_Expression::isVariable(const std::string& theString)
{
  if (theString.empty())
    return false;
  std::string::const_iterator it = theString.begin();
  if (!(isalpha(*it) || (*it) == '_') || it == theString.end())
    return false;
  it++;
  for ( ; it != theString.end(); ++it ) {
    if(!(isalnum(*it) || (*it) == '_')) {
      return false;
    }
  }
  return true;
}

