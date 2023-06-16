// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include "GeomData_Point2D.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Expression.h>
#include <ModelAPI_Feature.h>

#include <cassert>

GeomData_Point2D::GeomData_Point2D()
{
  myIsInitialized = false;
}

void GeomData_Point2D::reinit()
{
  myIsInitialized = true;
  for (int aComponent = 0; aComponent < NUM_COMPONENTS; ++aComponent) {
    myExpression[aComponent]->reinit();
    myIsInitialized = myIsInitialized && myExpression[aComponent]->isInitialized();
  }
}

void GeomData_Point2D::reset()
{
  myIsInitialized = false;
  for(int aComponent = 0; aComponent < NUM_COMPONENTS; ++aComponent) {
    myExpression[aComponent]->reset();
  }
}

void GeomData_Point2D::setCalculatedValue(const double theX, const double theY)
{
  if (!myIsInitialized || x() != theX || y() != theY) {
    myExpression[0]->setValue(theX);
    myExpression[1]->setValue(theY);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void GeomData_Point2D::setValue(const double theX, const double theY)
{
  setCalculatedValue(textX().empty() ? theX : x(),
                     textY().empty() ? theY : y());
}

void GeomData_Point2D::setValue(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  setValue(thePoint->x(), thePoint->y());
}

double GeomData_Point2D::x() const
{
  return myExpression[0]->value();
}

double GeomData_Point2D::y() const
{
  return myExpression[1]->value();
}

std::shared_ptr<GeomAPI_Pnt2d> GeomData_Point2D::pnt()
{
  std::shared_ptr<GeomAPI_Pnt2d> aResult(new GeomAPI_Pnt2d(x(), y()));
  return aResult;
}

void GeomData_Point2D::setText(const std::wstring& theX,
                               const std::wstring& theY)
{
  if (!myIsInitialized && theX.empty() && theY.empty())
    return; // empty strings are not good initializers
  if (!myIsInitialized || textX() != theX || textY() != theY) {
    myExpression[0]->setText(theX);
    myExpression[1]->setText(theY);
    // Send it to evaluator to convert into the double and store in the attribute
    ModelAPI_AttributeEvalMessage::send(owner()->data()->attribute(id()), this);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::wstring GeomData_Point2D::textX()
{
  return myExpression[0]->text();
}
std::wstring GeomData_Point2D::textY()
{
  return myExpression[1]->text();
}

void GeomData_Point2D::setExpressionInvalid(int theComponent, bool theFlag)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  if (!myIsInitialized || expressionInvalid(theComponent) != theFlag)
    myExpression[theComponent]->setInvalid(theFlag);
}

bool GeomData_Point2D::expressionInvalid(int theComponent)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  return myExpression[theComponent]->isInvalid();
}

void GeomData_Point2D::setExpressionError(int theComponent, const std::string& theError)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  if (expressionError(theComponent) != theError)
    myExpression[theComponent]->setError(theError);
}

std::string GeomData_Point2D::expressionError(int theComponent)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  return myExpression[theComponent]->error();
}

void GeomData_Point2D::setUsedParameters(int theComponent,
                                         const std::set<std::wstring>& theUsedParameters)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  myExpression[theComponent]->setUsedParameters(theUsedParameters);
}

std::set<std::wstring> GeomData_Point2D::usedParameters(int theComponent) const
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  return myExpression[theComponent]->usedParameters();
}
