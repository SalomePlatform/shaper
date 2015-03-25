// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Lin.cpp
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

#include <ElSLib.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <Precision.hxx>
#include <ProjLib.hxx>

#define MY_LIN static_cast<gp_Lin*>(myImpl)

static gp_Lin* newLine(const double theStartX, const double theStartY, const double theStartZ,
                       const double theEndX, const double theEndY, const double theEndZ)
{
  gp_XYZ aDir(theEndX - theStartX, theEndY - theStartY, theEndZ - theStartZ);
  gp_Pnt aStart(theStartX, theStartY, theStartZ);
  return new gp_Lin(aStart, gp_Dir(aDir));
}

GeomAPI_Lin::GeomAPI_Lin(const double theStartX, const double theStartY, const double theStartZ,
                         const double theEndX, const double theEndY, const double theEndZ)
    : GeomAPI_Interface(newLine(theStartX, theStartY, theStartZ, theEndX, theEndY, theEndZ))
{
}

GeomAPI_Lin::GeomAPI_Lin(const std::shared_ptr<GeomAPI_Pnt>& theStart,
                         const std::shared_ptr<GeomAPI_Pnt>& theEnd)
    : GeomAPI_Interface(
        newLine(theStart->x(), theStart->y(), theStart->z(), theEnd->x(), theEnd->y(), theEnd->z()))
{
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Lin::location()
{
  gp_Pnt aLoc = impl<gp_Lin>().Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_Lin::direction()
{
  const gp_Dir& aDir = impl<gp_Lin>().Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}

double GeomAPI_Lin::distance(const std::shared_ptr<GeomAPI_Pnt>& thePoint) const
{
  return MY_LIN->Distance(thePoint->impl<gp_Pnt>());
}

const std::shared_ptr<GeomAPI_Pnt> GeomAPI_Lin::intersect(
    const std::shared_ptr<GeomAPI_Lin>& theLine) const
{
  if (MY_LIN->SquareDistance(theLine->impl<gp_Lin>()) > Precision::Confusion())
  return std::shared_ptr<GeomAPI_Pnt>();

  const gp_Dir& aDir1 = MY_LIN->Direction();
  const gp_Dir& aDir2 = theLine->impl<gp_Lin>().Direction();
  gp_Dir aCross = aDir1.Crossed(aDir2);
  gp_Pln aPlane(MY_LIN->Location(), aCross);  // plane containing both lines

  gp_Lin2d aPrjLine1 = ProjLib::Project(aPlane, *MY_LIN);
  gp_Lin2d aPrjLine2 = ProjLib::Project(aPlane, theLine->impl<gp_Lin>());

  IntAna2d_AnaIntersection anInter(aPrjLine1, aPrjLine1);
  if (!anInter.IsDone() || anInter.IsEmpty())
  return std::shared_ptr<GeomAPI_Pnt>();
  const gp_Pnt2d& anIntPnt2d = anInter.Point(0).Value();
  gp_Pnt aResult = ElSLib::Value(anIntPnt2d.X(), anIntPnt2d.Y(), aPlane);

  return std::shared_ptr<GeomAPI_Pnt>(
  new GeomAPI_Pnt(aResult.X(), aResult.Y(), aResult.Z()));
}

const std::shared_ptr<GeomAPI_Pnt> GeomAPI_Lin::project(
    const std::shared_ptr<GeomAPI_Pnt>& thePoint) const
{
  const gp_XYZ& aDir = MY_LIN->Direction().XYZ();
  const gp_XYZ& aLoc = MY_LIN->Location().XYZ();
  const gp_XYZ& aPnt = thePoint->impl<gp_Pnt>().XYZ();
  double aParam = aDir.Dot(aPnt - aLoc);

  gp_XYZ aResult = aPnt + aDir * aParam;
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aResult.X(), aResult.Y(), aResult.Z()));
}

