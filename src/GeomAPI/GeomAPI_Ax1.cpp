// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Ax1.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <GeomAPI_Ax1.h>

#include <gp_Ax1.hxx>

#define MY_AX1 implPtr<gp_Ax1>()

//=================================================================================================
GeomAPI_Ax1::GeomAPI_Ax1()
: GeomAPI_Interface(new gp_Ax1())
{
}

//=================================================================================================
GeomAPI_Ax1::GeomAPI_Ax1(std::shared_ptr<GeomAPI_Pnt> theOrigin,
                         std::shared_ptr<GeomAPI_Dir> theDir)
: GeomAPI_Interface(new gp_Ax1(theOrigin->impl<gp_Pnt>(),
                               theDir->impl<gp_Dir>()))
{
}

//=================================================================================================
void GeomAPI_Ax1::setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin)
{
  MY_AX1->SetLocation(theOrigin->impl<gp_Pnt>());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAPI_Ax1::origin() const
{
  gp_Pnt aPnt = MY_AX1->Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPnt.X(),aPnt.Y(),aPnt.Z()));
}

//=================================================================================================
void GeomAPI_Ax1::setDir(const std::shared_ptr<GeomAPI_Dir>& theDir)
{
  MY_AX1->SetDirection(theDir->impl<gp_Dir>());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Dir> GeomAPI_Ax1::dir() const
{
  gp_Dir aDir = MY_AX1->Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}

//=================================================================================================
void GeomAPI_Ax1::reverse()
{
  MY_AX1->Reverse();
}

//=================================================================================================
std::shared_ptr<GeomAPI_Ax1> GeomAPI_Ax1::reversed()
{
  gp_Ax1 anAxis = MY_AX1->Reversed();
  std::shared_ptr<GeomAPI_Pnt> aPnt(new GeomAPI_Pnt(anAxis.Location().X(), anAxis.Location().Y(), anAxis.Location().Z()));
  std::shared_ptr<GeomAPI_Dir> aDir(new GeomAPI_Dir(anAxis.Direction().X(), anAxis.Direction().Y(), anAxis.Direction().Z()));
  return std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(aPnt, aDir));
}
