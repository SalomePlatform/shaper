// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Trsf.cpp
// Created:     13 Jul 2015
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Trsf.h>

#include <GeomAPI_Ax1.h>

#include <gp_Ax1.hxx>
#include<gp_Trsf.hxx>

#define MY_TRSF implPtr<gp_Trsf>()

//=================================================================================================
GeomAPI_Trsf::GeomAPI_Trsf()
: GeomAPI_Interface(new gp_Trsf())
{
}

//=================================================================================================
void GeomAPI_Trsf::setTranslation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                  const double theDistance)
{
  MY_TRSF->SetTranslation(gp_Vec(theAxis->impl<gp_Ax1>().Direction()) * theDistance);
}

//=================================================================================================
void GeomAPI_Trsf::setRotation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                               const double theAngle)
{
  MY_TRSF->SetRotation(theAxis->impl<gp_Ax1>(), theAngle / 180.0 * M_PI);
}
