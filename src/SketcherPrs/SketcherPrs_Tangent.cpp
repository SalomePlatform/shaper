// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Tangent.cpp
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Tangent.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <GeomAPI_Curve.h>

#include <SketchPlugin_Constraint.h>

#include <GeomAdaptor_Curve.hxx>
#include <Prs3d_Root.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <StdPrs_DeflectionCurve.hxx>


IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Tangent, SketcherPrs_SymbolPrs);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Tangent, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Tangent::SketcherPrs_Tangent(SketchPlugin_Constraint* theConstraint, 
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane)
{
  myPntArray = new Graphic3d_ArrayOfPoints(2);
  myPntArray->AddVertex(0., 0., 0.);
  myPntArray->AddVertex(0., 0., 0.);
}  

bool SketcherPrs_Tangent::updatePoints(double theStep) const
{
  ObjectPtr aObj1 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());
  if (SketcherPrs_Tools::getShape(aObj1).get() == NULL)
    return false;
  if (SketcherPrs_Tools::getShape(aObj2).get() == NULL)
    return false;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aObj1, this, theStep);
  gp_Pnt aP2 = aMgr->getPosition(aObj2, this, theStep);
  myPntArray->SetVertice(1, aP1);
  myPntArray->SetVertice(2, aP2);
  return true;
}

void SketcherPrs_Tangent::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  ObjectPtr aObj1 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());

  std::shared_ptr<GeomAPI_Shape> aShape1 = SketcherPrs_Tools::getShape(aObj1);
  std::shared_ptr<GeomAPI_Shape> aShape2 = SketcherPrs_Tools::getShape(aObj2);

  if ((aShape1.get() == NULL) || (aShape2.get() == NULL))
    return;

  std::shared_ptr<GeomAPI_Curve> aCurve1 = std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(aShape1));
  std::shared_ptr<GeomAPI_Curve> aCurve2 = std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(aShape2));
  if (aCurve1->isCircle() && aCurve2->isLine()) {
    addLine(aGroup, SketchPlugin_Constraint::ENTITY_B());
    GeomAdaptor_Curve aAdaptor(aCurve1->impl<Handle(Geom_Curve)>(), aCurve1->startParam(), aCurve1->endParam());
    StdPrs_DeflectionCurve::Add(thePrs,aAdaptor,myDrawer);
  } else if (aCurve1->isLine() && aCurve2->isCircle()) {
    addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());
    GeomAdaptor_Curve aAdaptor(aCurve2->impl<Handle(Geom_Curve)>(), aCurve2->startParam(), aCurve2->endParam());
    StdPrs_DeflectionCurve::Add(thePrs,aAdaptor,myDrawer);
  } else {
    // Both curves are arcs
    GeomAdaptor_Curve aAdaptor1(aCurve1->impl<Handle(Geom_Curve)>(), aCurve1->startParam(), aCurve1->endParam());
    StdPrs_DeflectionCurve::Add(thePrs, aAdaptor1, myDrawer);
    GeomAdaptor_Curve aAdaptor2(aCurve2->impl<Handle(Geom_Curve)>(), aCurve2->startParam(), aCurve2->endParam());
    StdPrs_DeflectionCurve::Add(thePrs, aAdaptor2, myDrawer);
  }
}

