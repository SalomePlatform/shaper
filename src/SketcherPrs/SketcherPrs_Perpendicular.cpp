// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Perpendicular.cpp
// Created:     12 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Perpendicular.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <GeomAPI_Pnt.h>

#include <SketchPlugin_Constraint.h>

#include <AIS_Drawer.hxx>
#include <gp_Pnt2d.hxx>

#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>

#include <Graphic3d_MarkerImage.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>



IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Perpendicular, SketcherPrs_SymbolPrs);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Perpendicular, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Perpendicular::SketcherPrs_Perpendicular(SketchPlugin_Constraint* theConstraint, 
                                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane)
{
  myPntArray = new Graphic3d_ArrayOfPoints(2);
  myPntArray->AddVertex(0., 0., 0.);
  myPntArray->AddVertex(0., 0., 0.);
}  

void SketcherPrs_Perpendicular::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  prepareAspect();

  ObjectPtr aObj1 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());

  std::shared_ptr<GeomAPI_Shape> aLine1 = SketcherPrs_Tools::getShape(aObj1);
  if (aLine1.get() == NULL)
    return;

  std::shared_ptr<GeomAPI_Shape> aLine2 = SketcherPrs_Tools::getShape(aObj2);
  if (aLine2.get() == NULL)
    return;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aObj1, this);
  gp_Pnt aP2 = aMgr->getPosition(aObj2, this);

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);
  aGroup->SetPrimitivesAspect(myAspect);
  myPntArray->SetVertice(1, aP1);
  myPntArray->SetVertice(2, aP2);
  aGroup->AddPrimitiveArray(myPntArray);
}

void SketcherPrs_Perpendicular::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());
  addLine(aGroup, SketchPlugin_Constraint::ENTITY_B());
}

