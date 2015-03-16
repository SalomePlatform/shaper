// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Rigid.cpp
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Rigid.h"
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

#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveSegment.hxx>

#include <SelectMgr_SequenceOfOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_EntityOwner.hxx>


extern std::shared_ptr<GeomAPI_Pnt2d> getFeaturePoint(DataPtr theData,
                                                      const std::string& theAttribute);


IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Rigid, SketcherPrs_SymbolPrs);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Rigid, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Rigid::SketcherPrs_Rigid(SketchPlugin_Constraint* theConstraint, 
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane)
{
  myPntArray = new Graphic3d_ArrayOfPoints(1);
  myPntArray->AddVertex(0., 0., 0.);
}  

void SketcherPrs_Rigid::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  prepareAspect();

  std::shared_ptr<GeomAPI_Shape> aLine1 = SketcherPrs_Tools::getLine(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  if (aLine1.get() == NULL)
    return;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aLine1, this);

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);
  aGroup->SetPrimitivesAspect(myAspect);
  myPntArray->SetVertice(1, aP1);
  aGroup->AddPrimitiveArray(myPntArray);
}

void SketcherPrs_Rigid::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());
}

