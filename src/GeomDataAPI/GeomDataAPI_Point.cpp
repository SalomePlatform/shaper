// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomDataAPI_Point.cpp
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#include <GeomDataAPI_Point.h>

std::string GeomDataAPI_Point::attributeType()
{
  return typeId();
}

GeomDataAPI_Point::GeomDataAPI_Point()
{
}

GeomDataAPI_Point::~GeomDataAPI_Point()
{
}
