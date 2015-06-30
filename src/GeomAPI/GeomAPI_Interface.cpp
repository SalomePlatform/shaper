// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Interface.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include <GeomAPI_Interface.h>

GeomAPI_Interface::GeomAPI_Interface()
{

}

GeomAPI_Interface::~GeomAPI_Interface()
{

}

bool GeomAPI_Interface::empty() const
{
  return myImpl.get() == 0;
}
