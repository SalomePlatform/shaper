// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeInteger.cpp
// Created:     03 sep 2014
// Author:      sbh

#include <Model_AttributeInteger.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Expression.h>
#include <ModelAPI_Object.h>

Model_AttributeInteger::Model_AttributeInteger(TDF_Label& theLabel)
{
  myIsInitialized = true;
}

void Model_AttributeInteger::setCalculatedValue(const int theValue)
{
  if (!myIsInitialized || value() != theValue) {
    myExpression->setValue(theValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void Model_AttributeInteger::setValue(const int theValue)
{
  setCalculatedValue(text().empty() ? theValue : value());
}

int Model_AttributeInteger::value()
{
  return myExpression->value();
}

void Model_AttributeInteger::setText(const std::string& theValue)
{
  if (text() != theValue) {
    myExpression->setText(theValue);
    // Send it to evaluator to convert text to integer and store in the attribute
    ModelAPI_AttributeEvalMessage::send(owner()->data()->attribute(id()), this);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::string Model_AttributeInteger::text()
{
  return myExpression->text();
}

void Model_AttributeInteger::setExpressionInvalid(const bool theFlag)
{
  myExpression->setInvalid(theFlag);
}

bool Model_AttributeInteger::expressionInvalid()
{
  return myExpression->isInvalid();
}

void Model_AttributeInteger::setExpressionError(const std::string& theError)
{
  if (expressionError() != theError)
    myExpression->setError(theError);
}

std::string Model_AttributeInteger::expressionError()
{
  return myExpression->error();
}

void Model_AttributeInteger::setUsedParameters(const std::set<std::string>& theUsedParameters)
{
  myExpression->setUsedParameters(theUsedParameters);
}

std::set<std::string> Model_AttributeInteger::usedParameters() const
{
  return myExpression->usedParameters();
}
