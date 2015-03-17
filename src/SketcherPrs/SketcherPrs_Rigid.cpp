// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Rigid.cpp
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Rigid.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Curve.h>
#include <GeomAPI_Vertex.h>

#include <SketchPlugin_Constraint.h>

#include <AIS_Drawer.hxx>
#include <gp_Pnt2d.hxx>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_Point.hxx>
#include <Geom_CartesianPoint.hxx>



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

  ObjectPtr aObj1 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<GeomAPI_Shape> aLine1 = SketcherPrs_Tools::getShape(aObj1);
  if (aLine1.get() == NULL)
    return;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aObj1, this);

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);
  aGroup->SetPrimitivesAspect(myAspect);
  myPntArray->SetVertice(1, aP1);
  aGroup->AddPrimitiveArray(myPntArray);
}

void SketcherPrs_Rigid::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  ObjectPtr aObj = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getShape(aObj);
  if (aShape.get() == NULL)
    return;

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);
  if (aShape->isEdge()) {
    Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
    aGroup->SetPrimitivesAspect(aLineAspect);
    std::shared_ptr<GeomAPI_Curve> aCurve = std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(aShape));
    if (aCurve->isLine()) {
      addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());
    } else {
      GeomAdaptor_Curve aAdaptor(aCurve->impl<Handle(Geom_Curve)>(), aCurve->startParam(), aCurve->endParam());
      StdPrs_DeflectionCurve::Add(thePrs,aAdaptor,myDrawer);
    }
  } else {
    // This is a point
    Handle(Prs3d_PointAspect) aPntAspect = new Prs3d_PointAspect(Aspect_TOM_PLUS, theColor, 1);
    myDrawer->SetPointAspect(aPntAspect);

    std::shared_ptr<GeomAPI_Vertex> aVertex = std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(aShape));
    std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
    Handle(Geom_CartesianPoint) aPoint = new Geom_CartesianPoint(aPnt->impl<gp_Pnt>());
    StdPrs_Point::Add(thePrs, aPoint, myDrawer);
  }
}

