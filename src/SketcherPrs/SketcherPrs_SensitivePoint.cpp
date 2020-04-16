// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SketcherPrs_SensitivePoint.h"
#include "SketcherPrs_SymbolPrs.h"

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Standard_Version.hxx>

#define DEBUG_SENSITIVE_TO_BE_CORRECTED

IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_SensitivePoint, Select3D_SensitiveEntity);

SketcherPrs_SensitivePoint::SketcherPrs_SensitivePoint(
  const Handle(SelectBasics_EntityOwner)& anOwner, int theId)
:Select3D_SensitiveEntity(anOwner), myId(theId)
{
  SetSensitivityFactor(12);
}

Standard_Boolean SketcherPrs_SensitivePoint::Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                   SelectBasics_PickResult& thePickResult)
{
#if OCC_VERSION_HEX < 0x070400
	Standard_Real aDepth = RealLast();
	Standard_Real aDistToCOG = RealLast();
	gp_Pnt aPnt = Point();
	if (!theMgr.Overlaps(aPnt, aDepth))
	{
		thePickResult = SelectBasics_PickResult(aDepth, aDistToCOG);
		return Standard_False;
	}

	aDistToCOG = aDepth;
	thePickResult = SelectBasics_PickResult(aDepth, aDistToCOG);
	return Standard_True;
#else
  gp_Pnt aPnt = Point();
  if (theMgr.Overlaps (aPnt, thePickResult))
	  return Standard_True;
  return Standard_False;
#endif
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
#ifdef DEBUG_SENSITIVE_TO_BE_CORRECTED
  const Handle(SelectMgr_EntityOwner)& anOwner =
    Handle(SelectMgr_EntityOwner)::DownCast(OwnerId());
  const Handle(SketcherPrs_SymbolPrs)& aSymbPrs =
    Handle(SketcherPrs_SymbolPrs)::DownCast(anOwner->Selectable());
  if (aSymbPrs->pointsArray()->VertexNumber() < myId)
    return gp_Pnt();
#endif
  return Point();
}

Select3D_BndBox3d SketcherPrs_SensitivePoint::BoundingBox()
{
#ifdef DEBUG_SENSITIVE_TO_BE_CORRECTED
  const Handle(SelectMgr_EntityOwner)& anOwner =
    Handle(SelectMgr_EntityOwner)::DownCast(OwnerId());
  const Handle(SketcherPrs_SymbolPrs)& aSymbPrs =
    Handle(SketcherPrs_SymbolPrs)::DownCast(anOwner->Selectable());
  if (aSymbPrs->pointsArray()->VertexNumber() < myId)
    return Select3D_BndBox3d();
#endif
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
