// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_CenterPrs.cpp
// Created:     25 April 2017
// Author:      Vitaly SMETANNIKOV

#include "PartSet_CenterPrs.h"

#include <Geom_CartesianPoint.hxx>


IMPLEMENT_STANDARD_RTTIEXT(PartSet_CenterPrs, AIS_Point)

PartSet_CenterPrs::PartSet_CenterPrs(const ObjectPtr& theObject,
                                     const GeomShapePtr& theEdge,
                                     const gp_Pnt& theCenter)
  : AIS_Point(new Geom_CartesianPoint(theCenter)),
  myObject(theObject),
  myEdge(theEdge)
{
}