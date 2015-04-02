// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_HVDirection.cpp
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_HVDirection.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Constraint.h>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>



IMPLEMENT_STANDARD_HANDLE(SketcherPrs_HVDirection, SketcherPrs_SymbolPrs);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_HVDirection, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_HVDirection::SketcherPrs_HVDirection(ModelAPI_Feature* theConstraint, 
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                           bool isHorisontal) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane), myIsHorisontal(isHorisontal)
{
  myPntArray = new Graphic3d_ArrayOfPoints(1);
  myPntArray->AddVertex(0., 0., 0.);
}  

bool SketcherPrs_HVDirection::updatePoints(double theStep) const 
{
  ObjectPtr aObj = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  if (SketcherPrs_Tools::getShape(aObj).get() == NULL)
    return false;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aObj, this, theStep);
  myPntArray->SetVertice(1, aP1);
  return true;
}

void SketcherPrs_HVDirection::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());
}

