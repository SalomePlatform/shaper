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

#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveSegment.hxx>

#include <SelectMgr_SequenceOfOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_EntityOwner.hxx>


// Function which is defined in SketchPlugin_ConstraintDistance.cpp
extern std::shared_ptr<GeomAPI_Pnt2d> getFeaturePoint(DataPtr theData,
                                                      const std::string& theAttribute);



IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Perpendicular, SketcherPrs_SymbolPrs);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Perpendicular, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Perpendicular::SketcherPrs_Perpendicular(SketchPlugin_Constraint* theConstraint, 
                                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane)
{
  myPntArray = new Graphic3d_ArrayOfPoints(2);
  myPntArray->AddVertex(0., 0., 0.);
  myPntArray->AddVertex(0. ,0., 0.);
}  

void SketcherPrs_Perpendicular::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  prepareAspect();

  std::shared_ptr<GeomAPI_Shape> aLine1 = SketcherPrs_Tools::getLine(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  if (aLine1.get() == NULL)
    return;

  std::shared_ptr<GeomAPI_Shape> aLine2 = SketcherPrs_Tools::getLine(myConstraint, SketchPlugin_Constraint::ENTITY_B());
  if (aLine2.get() == NULL)
    return;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aLine1, this);
  gp_Pnt aP2 = aMgr->getPosition(aLine2, this);
  //std::shared_ptr<GeomAPI_Pnt> aPnt1 = aLine1->firstPoint();
  //std::shared_ptr<GeomAPI_Pnt> aPnt2 = aLine1->lastPoint();
  //gp_Pnt aP1((aPnt1->x() + aPnt2->x())/2.,
  //           (aPnt1->y() + aPnt2->y())/2.,
  //           (aPnt1->z() + aPnt2->z())/2.);

  //gp_Vec aVec1(aPnt1->impl<gp_Pnt>(), aPnt2->impl<gp_Pnt>());
  //gp_Vec aShift = aVec1.Crossed(myPlane->norm()->impl<gp_Dir>());
  //aShift.Normalize();
  //aShift.Multiply(20);
  //aP1.Translate(aShift);

  //aPnt1 = aLine2->firstPoint();
  //aPnt2 = aLine2->lastPoint();
  //gp_Pnt aP2((aPnt1->x() + aPnt2->x())/2.,
  //           (aPnt1->y() + aPnt2->y())/2.,
  //           (aPnt1->z() + aPnt2->z())/2.);

  //gp_Vec aVec2(aPnt1->impl<gp_Pnt>(), aPnt2->impl<gp_Pnt>());
  //aShift = aVec2.Crossed(myPlane->norm()->impl<gp_Dir>());
  //aShift.Normalize();
  //aShift.Multiply(20);
  //aP2.Translate(aShift);

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);
  aGroup->SetPrimitivesAspect(myAspect);
  myPntArray->SetVertice(1, aP1);
  myPntArray->SetVertice(2, aP2);
  aGroup->AddPrimitiveArray(myPntArray);
}

void SketcherPrs_Perpendicular::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  ClearSelected();

  Handle(SelectMgr_EntityOwner) aOwn = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitivePoint) aSP1 = new Select3D_SensitivePoint(aOwn, myPntArray->Vertice(1));
  Handle(Select3D_SensitivePoint) aSP2 = new Select3D_SensitivePoint(aOwn, myPntArray->Vertice(2));
  aSelection->Add(aSP1);
  aSelection->Add(aSP2);
}

void SketcherPrs_Perpendicular::HilightSelected(const Handle(PrsMgr_PresentationManager3d)& thePM, 
                                           const SelectMgr_SequenceOfOwner& theOwners)
{

  Handle( Prs3d_Presentation ) aSelectionPrs = GetSelectPresentation( thePM );
  aSelectionPrs->Clear();
  drawLines(aSelectionPrs, Quantity_NOC_WHITE);

  aSelectionPrs->SetDisplayPriority(9);
  aSelectionPrs->Display();
  thePM->Highlight(this);
}

void SketcherPrs_Perpendicular::HilightOwnerWithColor(const Handle(PrsMgr_PresentationManager3d)& thePM, 
                                                 const Quantity_NameOfColor theColor, const Handle(SelectMgr_EntityOwner)& theOwner)
{
  thePM->Color(this, theColor);

  Handle( Prs3d_Presentation ) aHilightPrs = GetHilightPresentation( thePM );
  aHilightPrs->Clear();
  drawLines(aHilightPrs, theColor);

  if (thePM->IsImmediateModeOn())
    thePM->AddToImmediateList(aHilightPrs);
}


void SketcherPrs_Perpendicular::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());
  addLine(aGroup, SketchPlugin_Constraint::ENTITY_B());
}

