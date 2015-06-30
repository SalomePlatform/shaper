// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Lin2d.cpp
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Dir2d.h>

#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>

#include <IntAna2d_AnaIntersection.hxx>

#define MY_LIN2D implPtr<gp_Lin2d>()

static gp_Lin2d* newLine2d(const double theStartX, const double theStartY, const double theEndX,
                           const double theEndY)
{
  gp_XY aDir(theEndX - theStartX, theEndY - theStartY);
  gp_Pnt2d aStart(theStartX, theStartY);
  return new gp_Lin2d(aStart, gp_Dir2d(aDir));
}

GeomAPI_Lin2d::GeomAPI_Lin2d(const double theStartX, const double theStartY, const double theEndX,
                             const double theEndY)
    : GeomAPI_Interface(newLine2d(theStartX, theStartY, theEndX, theEndY))
{
}

GeomAPI_Lin2d::GeomAPI_Lin2d(const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd)
    : GeomAPI_Interface(newLine2d(theStart->x(), theStart->y(), theEnd->x(), theEnd->y()))
{
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Lin2d::location()
{
  gp_Pnt2d aLoc = impl<gp_Lin2d>().Location();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aLoc.X(), aLoc.Y()));
}

std::shared_ptr<GeomAPI_Dir2d> GeomAPI_Lin2d::direction()
{
  const gp_Dir2d& aDir = impl<gp_Lin2d>().Direction();
  return std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aDir.X(), aDir.Y()));
}

double GeomAPI_Lin2d::distance(const std::shared_ptr<GeomAPI_Pnt2d>& theOther) const
{
  return MY_LIN2D->Distance(theOther->impl<gp_Pnt2d>());
}

const std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Lin2d::intersect(
    const std::shared_ptr<GeomAPI_Lin2d>& theLine) const
{
  IntAna2d_AnaIntersection anInter(*MY_LIN2D, theLine->impl<gp_Lin2d>());
  if (!anInter.IsDone() || anInter.IsEmpty())
  return std::shared_ptr<GeomAPI_Pnt2d>();
  const gp_Pnt2d& aResult = anInter.Point(1).Value();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aResult.X(), aResult.Y()));
}

const std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Lin2d::project(
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) const
{
  const gp_XY& aDir = MY_LIN2D->Direction().XY();
  const gp_XY& aLoc = MY_LIN2D->Location().XY();
  const gp_XY& aPnt = thePoint->impl<gp_Pnt2d>().XY();
  double aParam = aDir.Dot(aPnt - aLoc);

  gp_XY aResult = aLoc + aDir * aParam;
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aResult.X(), aResult.Y()));
}

bool GeomAPI_Lin2d::isRight(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) const
{
  const gp_XY& aDir = MY_LIN2D->Direction().XY();
  const gp_XY& aLoc = MY_LIN2D->Location().XY();
  const gp_XY& aPnt = thePoint->impl<gp_Pnt2d>().XY();

  return aDir.Crossed(aPnt - aLoc) > 0;
}


std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Lin2d::shiftedLocation(double theShift) const
{
  gp_Vec2d aVec = MY_LIN2D->Direction();
  aVec = aVec.GetNormal();
  aVec.Normalize();
  aVec.Reverse();
  aVec.Scale(theShift);
  gp_Lin2d aLin = MY_LIN2D->Translated(aVec);
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aLin.Location().X(), aLin.Location().Y()));
}
