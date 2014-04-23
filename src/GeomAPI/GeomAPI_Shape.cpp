// File:        GeomAPI_Shape.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Shape.h>

#include<TopoDS_Shape.hxx>

#define MY_PNT static_cast<gp_Pnt*>(myImpl)

GeomAPI_Shape::GeomAPI_Shape()
  : GeomAPI_Interface(new TopoDS_Shape())
{}
