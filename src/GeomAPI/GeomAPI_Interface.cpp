// File:        GeomAPI_Interface.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include <GeomAPI_Interface.h>

GeomAPI_Interface::GeomAPI_Interface()
{
  myImpl = 0;
}

GeomAPI_Interface::GeomAPI_Interface(void* theImpl)
{
  myImpl = theImpl;
}

GeomAPI_Interface::~GeomAPI_Interface()
{
  if (myImpl)
    delete myImpl;
}

void GeomAPI_Interface::setImpl(void* theImpl)
{
  if (myImpl)
    delete myImpl;
  myImpl = theImpl;
}
