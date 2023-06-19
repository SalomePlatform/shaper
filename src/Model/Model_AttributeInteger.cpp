// Copyright (C) 2014-2023  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Model_AttributeInteger.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <Model_Expression.h>
#include <ModelAPI_Object.h>

Model_AttributeInteger::Model_AttributeInteger(TDF_Label& theLabel)
{
  // to the same label to support the backward compatibility
  myExpression.reset(new Model_ExpressionInteger(theLabel));
  myIsInitialized = myExpression->isInitialized();
}

void Model_AttributeInteger::reinit()
{
  myExpression->reinit();
  myIsInitialized = myExpression->isInitialized();
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

void Model_AttributeInteger::setText(const std::wstring& theValue)
{
  if (text() != theValue) {
    myExpression->setText(theValue);
    // Send it to evaluator to convert text to integer and store in the attribute
    ModelAPI_AttributeEvalMessage::send(owner()->data()->attribute(id()), this);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::wstring Model_AttributeInteger::text()
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

void Model_AttributeInteger::setUsedParameters(const std::set<std::wstring>& theUsedParameters)
{
  myExpression->setUsedParameters(theUsedParameters);
}

std::set<std::wstring> Model_AttributeInteger::usedParameters() const
{
  return myExpression->usedParameters();
}
