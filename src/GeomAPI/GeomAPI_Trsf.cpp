// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAPI_Trsf.cpp
// Created:     13 Jul 2015
// Author:      Mikhail PONIKAROV
//
// Modified by Clarisse Genrault (CEA) : 17 Nov 2016

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
GeomAPI_Trsf::GeomAPI_Trsf(void* theTrsf)
: GeomAPI_Interface(theTrsf)
{
}

//=================================================================================================
void GeomAPI_Trsf::setTranslation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                  const double theDistance)
{
  MY_TRSF->SetTranslation(gp_Vec(theAxis->impl<gp_Ax1>().Direction()) * theDistance);
}

//=================================================================================================
void GeomAPI_Trsf::setTranslation(const double theDx, const double theDy, const double theDz)
{
  MY_TRSF->SetTranslation(gp_Vec(theDx, theDy, theDz));
}

//=================================================================================================
void GeomAPI_Trsf::setRotation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                               const double theAngle)
{
  MY_TRSF->SetRotation(theAxis->impl<gp_Ax1>(), theAngle / 180.0 * M_PI);
}
