// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomData_Point2D.cxx
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomData_Point2D.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Expression.h>
#include <ModelAPI_Feature.h>

#include <cassert>

GeomData_Point2D::GeomData_Point2D(TDF_Label& theLabel)
{
  myIsInitialized = true;
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

void GeomData_Point2D::setText(const std::string& theX,
                               const std::string& theY)
{
  if (!myIsInitialized || textX() != theX || textY() != theY) {
    myExpression[0]->setText(theX);
    myExpression[1]->setText(theY);
    // Send it to evaluator to convert into the double and store in the attribute
    ModelAPI_AttributeEvalMessage::send(owner()->data()->attribute(id()), this);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::string GeomData_Point2D::textX()
{
  return myExpression[0]->text();
}
std::string GeomData_Point2D::textY()
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

void GeomData_Point2D::setUsedParameters(int theComponent, const std::set<std::string>& theUsedParameters)
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  myExpression[theComponent]->setUsedParameters(theUsedParameters);
}

std::set<std::string> GeomData_Point2D::usedParameters(int theComponent) const
{
  assert(theComponent >= 0 && theComponent < NUM_COMPONENTS);
  return myExpression[theComponent]->usedParameters();
}
