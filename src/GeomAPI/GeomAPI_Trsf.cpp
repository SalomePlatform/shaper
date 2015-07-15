// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Trsf.cpp
// Created:     13 Jul 2015
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Trsf.h>

#include<gp_Trsf.hxx>

GeomAPI_Trsf::GeomAPI_Trsf()
    : GeomAPI_Interface()
{
}

GeomAPI_Trsf::GeomAPI_Trsf(void* theTrsf)
    : GeomAPI_Interface(theTrsf)
{
}
