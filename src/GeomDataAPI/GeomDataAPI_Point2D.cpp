// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomDataAPI_Point2D.cpp
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV


#include <GeomDataAPI_Point2D.h>

void GeomDataAPI_Point2D::move(const double theDeltaX, const double theDeltaY)
{
  setValue(x() + theDeltaX, y() + theDeltaY);
}

std::string GeomDataAPI_Point2D::attributeType()
{
  return typeId();
}

GeomDataAPI_Point2D::GeomDataAPI_Point2D()
{
}

GeomDataAPI_Point2D::~GeomDataAPI_Point2D()
{
}
