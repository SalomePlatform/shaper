// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Ax2.cpp
// Created:     08 September 2015
// Author:      Dmitry Bobylev

#include <GeomAPI_Ax2.h>

#include <gp_Ax2.hxx>

#define MY_AX1 implPtr<gp_Ax2>()

//=================================================================================================
GeomAPI_Ax2::GeomAPI_Ax2()
: GeomAPI_Interface(new gp_Ax2())
{
}

//=================================================================================================
GeomAPI_Ax2::GeomAPI_Ax2(std::shared_ptr<GeomAPI_Pnt> theOrigin,
                         std::shared_ptr<GeomAPI_Dir> theN,
                         std::shared_ptr<GeomAPI_Dir> theVX)
: GeomAPI_Interface(new gp_Ax2(theOrigin->impl<gp_Pnt>(),
                               theN->impl<gp_Dir>(),
                               theVX->impl<gp_Dir>()))
{
}

//=================================================================================================
GeomAPI_Ax2::GeomAPI_Ax2(std::shared_ptr<GeomAPI_Pnt> theOrigin,
                         std::shared_ptr<GeomAPI_Dir> theDir)
: GeomAPI_Interface(new gp_Ax2(theOrigin->impl<gp_Pnt>(),
                               theDir->impl<gp_Dir>()))
{
}

//=================================================================================================
void GeomAPI_Ax2::setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin)
{
  MY_AX1->SetLocation(theOrigin->impl<gp_Pnt>());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAPI_Ax2::origin() const
{
  gp_Pnt aPnt = MY_AX1->Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPnt.X(),aPnt.Y(),aPnt.Z()));
}

//=================================================================================================
void GeomAPI_Ax2::setDir(const std::shared_ptr<GeomAPI_Dir>& theDir)
{
  MY_AX1->SetDirection(theDir->impl<gp_Dir>());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Dir> GeomAPI_Ax2::dir() const
{
  gp_Dir aDir = MY_AX1->Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}
