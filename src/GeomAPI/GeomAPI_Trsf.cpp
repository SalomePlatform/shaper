// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAPI_Trsf.cpp
// Created:     13 Jul 2015
// Author:      Mikhail PONIKAROV
//
// Modified by Clarisse Genrault (CEA) : 17 Nov 2016

#include<GeomAPI_Trsf.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Ax2.h>

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Trsf.hxx>

#define MY_TRSF implPtr<gp_Trsf>()

#include <iostream>

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
void GeomAPI_Trsf::setTranslation(const std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                                  const std::shared_ptr<GeomAPI_Pnt> theEndPoint)
{
  MY_TRSF->SetTranslation(theStartPoint->impl<gp_Pnt>(), theEndPoint->impl<gp_Pnt>());
}

//=================================================================================================
void GeomAPI_Trsf::setRotation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                               const double theAngle)
{
  MY_TRSF->SetRotation(theAxis->impl<gp_Ax1>(), theAngle / 180.0 * M_PI);
}

//=================================================================================================
void GeomAPI_Trsf::setRotation(const std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                               const std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                               const std::shared_ptr<GeomAPI_Pnt> theEndPoint)
{
  gp_Pnt aCenterPoint = theCenterPoint->impl<gp_Pnt>();
  gp_Pnt aStartPoint = theStartPoint->impl<gp_Pnt>();
  gp_Pnt aEndPoint = theEndPoint->impl<gp_Pnt>();

  gp_Vec aVec1(aCenterPoint, aStartPoint);
  gp_Vec aVec2(aCenterPoint, aEndPoint);
  gp_Dir aDir(aVec1 ^ aVec2);
  gp_Ax1 anAxis(aCenterPoint, aDir);
  double anAngle = aVec1.Angle(aVec2);
  if (fabs(anAngle) < Precision::Angular()) anAngle += 2.*M_PI; // Taken from old GEOM code

  MY_TRSF->SetRotation(anAxis, anAngle);
}

//=================================================================================================
void GeomAPI_Trsf::setSymmetry(const std::shared_ptr<GeomAPI_Pnt> thePoint)
{
  MY_TRSF->SetMirror(thePoint->impl<gp_Pnt>());
}

//=================================================================================================
void GeomAPI_Trsf::setSymmetry(const std::shared_ptr<GeomAPI_Ax1> theAxis)
{
  MY_TRSF->SetMirror(theAxis->impl<gp_Ax1>());
}

//=================================================================================================
void GeomAPI_Trsf::setSymmetry(const std::shared_ptr<GeomAPI_Ax2> thePlane)
{
  MY_TRSF->SetMirror(thePlane->impl<gp_Ax2>());
}
