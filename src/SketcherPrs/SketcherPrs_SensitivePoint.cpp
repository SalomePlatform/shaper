// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_SensitivePoint.cpp
// Created:     24 March 2015
// Author:      Vitaly SMETANNIKOV


#include "SketcherPrs_SensitivePoint.h"

#include <Graphic3d_ArrayOfPoints.hxx>
#include "SketcherPrs_SymbolPrs.h"

IMPLEMENT_STANDARD_HANDLE(SketcherPrs_SensitivePoint, Select3D_SensitiveEntity);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_SensitivePoint, Select3D_SensitiveEntity);

SketcherPrs_SensitivePoint::SketcherPrs_SensitivePoint(const Handle(SelectBasics_EntityOwner)& anOwner,
                                                       int theId)
:Select3D_SensitiveEntity(anOwner), myId(theId)
{
  SetSensitivityFactor(12.);
}

Standard_Boolean SketcherPrs_SensitivePoint::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                   SelectBasics_PickResult& thePickResult)
{
  Standard_Real aDepth      = RealLast();
  Standard_Real aDistToCOG  = RealLast();
  gp_Pnt aPnt = Point();
  if (!theMgr.Overlaps (aPnt, aDepth))
  {
    thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
    return Standard_False;
  }

  aDistToCOG = aDepth;
  thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
  return Standard_True;
}

gp_Pnt SketcherPrs_SensitivePoint::Point() const
{
  const Handle(SelectMgr_EntityOwner)& anOwner =
    Handle(SelectMgr_EntityOwner)::DownCast(OwnerId());
  const Handle(SketcherPrs_SymbolPrs)& aSymbPrs =
    Handle(SketcherPrs_SymbolPrs)::DownCast(anOwner->Selectable());
  return aSymbPrs->pointsArray()->Vertice(myId);
}

Handle(Select3D_SensitiveEntity) SketcherPrs_SensitivePoint::GetConnected()
{
  Handle(SketcherPrs_SensitivePoint) aNewEntity = new SketcherPrs_SensitivePoint (myOwnerId, myId);
  return aNewEntity;
}

gp_Pnt SketcherPrs_SensitivePoint::CenterOfGeometry() const
{
  return Point();
}

Select3D_BndBox3d SketcherPrs_SensitivePoint::BoundingBox()
{
  gp_Pnt aPnt = Point();
  return Select3D_BndBox3d (SelectMgr_Vec3 (aPnt.X(), aPnt.Y(), aPnt.Z()),
                            SelectMgr_Vec3 (aPnt.X(), aPnt.Y(), aPnt.Z()));
}

Standard_Integer SketcherPrs_SensitivePoint::NbSubElements()
{
  return 1;
}

void SketcherPrs_SensitivePoint::Clear()
{
  return;
}
