// Copyright (C) 2016-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Angle.cpp
// Created:     19 April 2016
// Author:      Artem ZHIDKOV

#include <GeomAPI_Angle.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_XYZ.h>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

struct ThreePoints {
  gp_Pnt myCenter;
  gp_Pnt myFirst;
  gp_Pnt mySecond;
  bool myReversed[2];
};

#define MY_ANGLE implPtr<ThreePoints>()
#define PI 3.1415926535897932

static ThreePoints* newAngle(const std::shared_ptr<GeomAPI_Pnt>& theCenter,
                             const std::shared_ptr<GeomAPI_Pnt>& theFirst,
                             const std::shared_ptr<GeomAPI_Pnt>& theSecond)
{
  ThreePoints* aResult = new ThreePoints;
  aResult->myCenter = gp_Pnt(theCenter->x(), theCenter->y(), theCenter->z());
  aResult->myFirst  = gp_Pnt(theFirst->x(), theFirst->y(), theFirst->z());
  aResult->mySecond = gp_Pnt(theSecond->x(), theSecond->y(), theSecond->z());
  aResult->myReversed[0] = aResult->myReversed[1] = false;
  return aResult;
}

static ThreePoints* newAngle(const std::shared_ptr<GeomAPI_Pnt>& theStart1,
                             const std::shared_ptr<GeomAPI_Pnt>& theEnd1,
                             const std::shared_ptr<GeomAPI_Pnt>& theStart2,
                             const std::shared_ptr<GeomAPI_Pnt>& theEnd2)
{
  std::shared_ptr<GeomAPI_Lin> aLine1(new GeomAPI_Lin(theStart1, theEnd1));
  std::shared_ptr<GeomAPI_Lin> aLine2(new GeomAPI_Lin(theStart2, theEnd2));
  std::shared_ptr<GeomAPI_Pnt> aCenter = aLine1->intersect(aLine2);
  bool isParallel = !aCenter;
  if (isParallel)
    aCenter = theStart1;
  std::shared_ptr<GeomAPI_Pnt> aPoint1, aPoint2;
  if (isParallel)
    aPoint1 = aPoint2 = theEnd1;
  else {
    aPoint1 = theStart1->distance(aCenter) < theEnd1->distance(aCenter) ? theEnd1 : theStart1;
    aPoint2 = theStart2->distance(aCenter) < theEnd2->distance(aCenter) ? theEnd2 : theStart2;
  }
  ThreePoints* anAngle = newAngle(aCenter, aPoint1, aPoint2);
  anAngle->myReversed[0] = aPoint1 == theStart1;
  anAngle->myReversed[1] = !isParallel && aPoint2 == theStart2;
  return anAngle;
}

static ThreePoints* newAngle(const std::shared_ptr<GeomAPI_Lin>& theLine1, bool theReversed1,
                             const std::shared_ptr<GeomAPI_Lin>& theLine2, bool theReversed2)
{
  std::shared_ptr<GeomAPI_Pnt> aCenter = theLine1->intersect(theLine2);
  if (!aCenter)
    aCenter = theLine1->location();
  double aCoeff = theReversed1 ? -1.0 : 1.0;
  std::shared_ptr<GeomAPI_Pnt> aPoint1(new GeomAPI_Pnt(
      aCenter->xyz()->added(theLine1->direction()->xyz()->multiplied(aCoeff))));
  aCoeff = theReversed2 ? -1.0 : 1.0;
  std::shared_ptr<GeomAPI_Pnt> aPoint2(new GeomAPI_Pnt(
      aCenter->xyz()->added(theLine2->direction()->xyz()->multiplied(aCoeff))));
  ThreePoints* anAngle = newAngle(aCenter, aPoint1, aPoint2);
  anAngle->myReversed[0] = theReversed1;
  anAngle->myReversed[1] = theReversed2;
  return anAngle;
}



GeomAPI_Angle::GeomAPI_Angle(const std::shared_ptr<GeomAPI_Pnt>& theStartLine1,
                             const std::shared_ptr<GeomAPI_Pnt>& theEndLine1,
                             const std::shared_ptr<GeomAPI_Pnt>& theStartLine2,
                             const std::shared_ptr<GeomAPI_Pnt>& theEndLine2)
    : GeomAPI_Interface(newAngle(theStartLine1, theEndLine1, theStartLine2, theEndLine2))
{
}

GeomAPI_Angle::GeomAPI_Angle(const std::shared_ptr<GeomAPI_Lin>& theLine1, bool theReversed1,
                             const std::shared_ptr<GeomAPI_Lin>& theLine2, bool theReversed2)
    : GeomAPI_Interface(newAngle(theLine1, theReversed1, theLine2, theReversed2))
{
}

GeomAPI_Angle::GeomAPI_Angle(const std::shared_ptr<GeomAPI_Pnt>& theCenter,
                             const std::shared_ptr<GeomAPI_Pnt>& thePoint1,
                             const std::shared_ptr<GeomAPI_Pnt>& thePoint2)
    : GeomAPI_Interface(newAngle(theCenter, thePoint1, thePoint2))
{
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Angle::center()
{
  gp_Pnt aPnt = MY_ANGLE->myCenter;
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Angle::firstPoint()
{
  gp_Pnt aPnt = MY_ANGLE->myFirst;
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Angle::secondPoint()
{
  gp_Pnt aPnt = MY_ANGLE->mySecond;
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
}

double GeomAPI_Angle::angleDegree()
{
  return angleRadian() * 180.0 / PI;
}

double GeomAPI_Angle::angleRadian()
{
  ThreePoints* anAngle = MY_ANGLE;
  gp_Dir aDir1(anAngle->myFirst.XYZ() - anAngle->myCenter.XYZ());
  gp_Dir aDir2(anAngle->mySecond.XYZ() - anAngle->myCenter.XYZ());
  return aDir1.Angle(aDir2);
}

bool GeomAPI_Angle::isReversed(int theIndex)
{
  return MY_ANGLE->myReversed[theIndex & 0x1];
}
