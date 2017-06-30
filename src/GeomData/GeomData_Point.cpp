// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "GeomData_Point.h"

#include <GeomAPI_Pnt.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Expression.h>
#include <ModelAPI_Feature.h>

#include <cassert>

GeomData_Point::GeomData_Point()
{
  myIsInitialized = false;
}

void GeomData_Point::reinit()
{
  myIsInitialized = true;
  for (int aComponent = 0; aComponent < NUM_COMPONENTS; ++aComponent) {
    myExpression[aComponent]->reinit();
    myIsInitialized = myIsInitialized && myExpression[aComponent]->isInitialized();
  }
}

void GeomData_Point::setCalculatedValue(const double theX, const double theY, const double theZ)
{
  if (!myIsInitialized || x() != theX || y() != theY || z() != theZ) {
    myExpression[0]->setValue(theX);
    myExpression[1]->setValue(theY);
    myExpression[2]->setValue(theZ);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void GeomData_Point::setValue(const double theX, const double theY, const double theZ)
{
  setCalculatedValue(textX().empty() ? theX : x(),
                     textY().empty() ? theY : y(),
                     textZ().empty() ? theZ : z());
}

void GeomData_Point::setValue(const std::shared_ptr<GeomAPI_Pnt>& thePoint)
{
  setValue(thePoint->x(), thePoint->y(), thePoint->z());
}

double GeomData_Point::x() const
{
  return myExpression[0]->value();
}

double GeomData_Point::y() const
{
  return myExpression[1]->value();
}

double GeomData_Point::z() const
{
  return myExpression[2]->value();
}

std::shared_ptr<GeomAPI_Pnt> GeomData_Point::pnt()
{
  std::shared_ptr<GeomAPI_Pnt> aResult(new GeomAPI_Pnt(x(), y(), z()));
  return aResult;
}

void GeomData_Point::setText(const std::string& theX,
                             const std::string& theY,
                             const std::string& theZ)
{
  if (!myIsInitialized || textX() != theX || textY() != theY || textZ() != theZ) {
    myExpression[0]->setText(theX);
    myExpression[1]->setText(theY);
    myExpression[2]->setText(theZ);
    // Send it to evaluator to convert into the double and store in the attribute
    ModelAPI_AttributeEvalMessage::send(owner()->data()->attribute(id()), this);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::string GeomData_Point::textX()
{
  return myExpression[0]->text();
}
std::string GeomData_Point::textY()
{
  return myExpression[1]->text();
}
std::string GeomData_Point::textZ()
{
  return myExpression[2]->text();
}

void GeomData_Point::setExpressionInvalid(int theComponent, bool theFlag)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  if (!myIsInitialized || expressionInvalid(theComponent) != theFlag)
    myExpression[theComponent]->setInvalid(theFlag);
}

bool GeomData_Point::expressionInvalid(int theComponent)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  return myExpression[theComponent]->isInvalid();
}

void GeomData_Point::setExpressionError(int theComponent, const std::string& theError)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  if (expressionError(theComponent) != theError)
    myExpression[theComponent]->setError(theError);
}

std::string GeomData_Point::expressionError(int theComponent)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  return myExpression[theComponent]->error();
}

void GeomData_Point::setUsedParameters(int theComponent,
                                       const std::set<std::string>& theUsedParameters)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  myExpression[theComponent]->setUsedParameters(theUsedParameters);
}

std::set<std::string> GeomData_Point::usedParameters(int theComponent) const
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  return myExpression[theComponent]->usedParameters();
}
