// Copyright (C) 2016-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Angle2d.cpp
// Created:     19 April 2016
// Author:      Artem ZHIDKOV

#include <GeomAPI_Angle2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>

struct ThreePoints2d {
  gp_Pnt2d myCenter;
  gp_Pnt2d myFirst;
  gp_Pnt2d mySecond;
  bool myReversed[2];
};

#define MY_ANGLE implPtr<ThreePoints2d>()
#define PI 3.1415926535897932

static ThreePoints2d* newAngle(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theFirst,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theSecond)
{
  ThreePoints2d* aResult = new ThreePoints2d;
  aResult->myCenter = gp_Pnt2d(theCenter->x(), theCenter->y());
  aResult->myFirst  = gp_Pnt2d(theFirst->x(), theFirst->y());
  aResult->mySecond = gp_Pnt2d(theSecond->x(), theSecond->y());
  aResult->myReversed[0] = aResult->myReversed[1] = false;
  return aResult;
}

static ThreePoints2d* newAngle(const std::shared_ptr<GeomAPI_Pnt2d>& theStart1,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theEnd1,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theStart2,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theEnd2)
{
  std::shared_ptr<GeomAPI_Lin2d> aLine1(new GeomAPI_Lin2d(theStart1, theEnd1));
  std::shared_ptr<GeomAPI_Lin2d> aLine2(new GeomAPI_Lin2d(theStart2, theEnd2));
  std::shared_ptr<GeomAPI_Pnt2d> aCenter = aLine1->intersect(aLine2);
  bool isParallel = !aCenter;
  if (isParallel)
    aCenter = theStart1;
  std::shared_ptr<GeomAPI_Pnt2d> aPoint1, aPoint2;
  if (isParallel)
    aPoint1 = aPoint2 = theEnd1;
  else {
    aPoint1 = theStart1->distance(aCenter) < theEnd1->distance(aCenter) ? theEnd1 : theStart1;
    aPoint2 = theStart2->distance(aCenter) < theEnd2->distance(aCenter) ? theEnd2 : theStart2;
  }
  ThreePoints2d* anAngle = newAngle(aCenter, aPoint1, aPoint2);
  anAngle->myReversed[0] = aPoint1 == theStart1;
  anAngle->myReversed[1] = !isParallel && aPoint2 == theStart2;
  return anAngle;
}

static ThreePoints2d* newAngle(const std::shared_ptr<GeomAPI_Lin2d>& theLine1, bool theReversed1,
                               const std::shared_ptr<GeomAPI_Lin2d>& theLine2, bool theReversed2)
{
  std::shared_ptr<GeomAPI_Pnt2d> aCenter = theLine1->intersect(theLine2);
  if (!aCenter)
    aCenter = theLine1->location();
  double aCoeff = theReversed1 ? -1.0 : 1.0;
  std::shared_ptr<GeomAPI_Pnt2d> aPoint1(new GeomAPI_Pnt2d(
      aCenter->xy()->added(theLine1->direction()->xy()->multiplied(aCoeff))));
  aCoeff = theReversed2 ? -1.0 : 1.0;
  std::shared_ptr<GeomAPI_Pnt2d> aPoint2(new GeomAPI_Pnt2d(
      aCenter->xy()->added(theLine2->direction()->xy()->multiplied(aCoeff))));
  ThreePoints2d* anAngle = newAngle(aCenter, aPoint1, aPoint2);
  anAngle->myReversed[0] = theReversed1;
  anAngle->myReversed[1] = theReversed2;
  return anAngle;
}



GeomAPI_Angle2d::GeomAPI_Angle2d(const std::shared_ptr<GeomAPI_Pnt2d>& theStartLine1,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theEndLine1,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theStartLine2,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theEndLine2)
    : GeomAPI_Interface(newAngle(theStartLine1, theEndLine1, theStartLine2, theEndLine2))
{
}

GeomAPI_Angle2d::GeomAPI_Angle2d(const std::shared_ptr<GeomAPI_Lin2d>& theLine1, bool theReversed1,
                                 const std::shared_ptr<GeomAPI_Lin2d>& theLine2, bool theReversed2)
    : GeomAPI_Interface(newAngle(theLine1, theReversed1, theLine2, theReversed2))
{
}

GeomAPI_Angle2d::GeomAPI_Angle2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2)
    : GeomAPI_Interface(newAngle(theCenter, thePoint1, thePoint2))
{
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Angle2d::center()
{
  gp_Pnt2d aPnt = MY_ANGLE->myCenter;
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aPnt.X(), aPnt.Y()));
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Angle2d::firstPoint()
{
  gp_Pnt2d aPnt = MY_ANGLE->myFirst;
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aPnt.X(), aPnt.Y()));
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Angle2d::secondPoint()
{
  gp_Pnt2d aPnt = MY_ANGLE->mySecond;
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aPnt.X(), aPnt.Y()));
}

double GeomAPI_Angle2d::angleDegree()
{
  return angleRadian() * 180.0 / PI;
}

double GeomAPI_Angle2d::angleRadian()
{
  ThreePoints2d* anAngle = MY_ANGLE;
  gp_Dir2d aDir1(anAngle->myFirst.XY() - anAngle->myCenter.XY());
  gp_Dir2d aDir2(anAngle->mySecond.XY() - anAngle->myCenter.XY());
  double aRes = aDir1.Angle(aDir2);
  if (aRes < 0.0) aRes += 2 * PI;
  return aRes;
}

bool GeomAPI_Angle2d::isReversed(int theIndex)
{
  return MY_ANGLE->myReversed[theIndex & 0x1];
}
