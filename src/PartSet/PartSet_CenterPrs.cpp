// Copyright (C) 2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

// File:        PartSet_CenterPrs.cpp
// Created:     25 April 2017
// Author:      Vitaly SMETANNIKOV

#include "PartSet_CenterPrs.h"

#include <ModuleBase_Tools.h>
#include <Geom_CartesianPoint.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Prs3d_Root.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <AIS_InteractiveContext.hxx>


IMPLEMENT_STANDARD_RTTIEXT(PartSet_CenterPrs, AIS_Point)


PartSet_CenterPrs::PartSet_CenterPrs(const ObjectPtr& theObject,
                                     const GeomEdgePtr& theEdge,
                                     const gp_Pnt& theCenter,
                                     ModelAPI_AttributeSelection::CenterType theType)
  : AIS_Point(new Geom_CartesianPoint(theCenter)),
  myObject(theObject),
  myEdge(theEdge),
  myCenterType(theType)
{
  SetAutoHilight(Standard_False);
}

void PartSet_CenterPrs::drawPoint(const Handle(Prs3d_Presentation)& thePrs,
                                  Quantity_Color theColor)
{
  Handle(Prs3d_Drawer) aDrawer = HilightAttributes();

  Handle(Prs3d_PointAspect) aPntAspect = aDrawer->PointAspect();

  Handle(Graphic3d_AspectMarker3d) PtA = aPntAspect->Aspect();
  PtA->SetType(Aspect_TOM_RING1);
  PtA->SetColor(theColor);

  Handle(Geom_Point) aPnt = Component();
  Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(thePrs);
  TheGroup->SetPrimitivesAspect(PtA);

  Handle(Graphic3d_ArrayOfPoints) aPoint = new Graphic3d_ArrayOfPoints (1);
  aPoint->AddVertex(aPnt->X(),aPnt->Y(),aPnt->Z());
  TheGroup->AddPrimitiveArray(aPoint);

  PtA = new Graphic3d_AspectMarker3d();
  PtA->SetType(Aspect_TOM_POINT);
  PtA->SetScale(5.);
  PtA->SetColor(theColor);
  TheGroup->SetPrimitivesAspect(PtA);
  TheGroup->AddPrimitiveArray (aPoint);
}

void PartSet_CenterPrs::HilightSelected(const Handle(PrsMgr_PresentationManager3d)& PM,
                                        const SelectMgr_SequenceOfOwner& Seq)
{
  Handle( Prs3d_Presentation ) aSelectionPrs = GetSelectPresentation( PM );
  aSelectionPrs->Clear();

  drawPoint(aSelectionPrs, GetContext()->SelectionStyle()->Color());

  aSelectionPrs->SetDisplayPriority(9);
  aSelectionPrs->Display();
  PM->Color(this, GetContext()->SelectionStyle());
}

void PartSet_CenterPrs::HilightOwnerWithColor(const Handle(PrsMgr_PresentationManager3d)& thePM,
                                              const Handle(Prs3d_Drawer)& theStyle,
                                              const Handle(SelectMgr_EntityOwner)& theOwner)
{
  Handle( Prs3d_Presentation ) aHilightPrs = GetHilightPresentation( thePM );
  aHilightPrs->Clear();

  thePM->Color(this, theStyle);
  drawPoint(aHilightPrs, theStyle->Color());

  aHilightPrs->SetZLayer(Graphic3d_ZLayerId_Topmost);

  if (thePM->IsImmediateModeOn())
    thePM->AddToImmediateList(aHilightPrs);
}
