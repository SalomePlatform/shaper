// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Collinear.cpp
// Created:     29 February 2016
// Author:      Natalia ERMOLAEVA

#include "SketcherPrs_Collinear.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Constraint.h>

#include "Events_Error.h"

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>


IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Collinear, SketcherPrs_SymbolPrs);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Collinear, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Collinear::SketcherPrs_Collinear(ModelAPI_Feature* theConstraint, 
                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane)
{
  myPntArray = new Graphic3d_ArrayOfPoints(2);
  myPntArray->AddVertex(0., 0., 0.);
  myPntArray->AddVertex(0., 0., 0.);
}  

bool SketcherPrs_Collinear::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                         const std::shared_ptr<GeomAPI_Ax3>&/* thePlane*/)
{
  bool aReadyToDisplay = false;

  ObjectPtr aObj1 = SketcherPrs_Tools::getResult(theConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 = SketcherPrs_Tools::getResult(theConstraint, SketchPlugin_Constraint::ENTITY_B());

  aReadyToDisplay = SketcherPrs_Tools::getShape(aObj1).get() != NULL &&
                    SketcherPrs_Tools::getShape(aObj2).get() != NULL;

  return aReadyToDisplay;
}

bool SketcherPrs_Collinear::updatePoints(double theStep) const
{
  if (!IsReadyToDisplay(myConstraint, myPlane))
    return false;

  ObjectPtr aObj1 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj2 = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_B());

  // Set points of the presentation
  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  gp_Pnt aP1 = aMgr->getPosition(aObj1, this, theStep);
  gp_Pnt aP2 = aMgr->getPosition(aObj2, this, theStep);
  myPntArray->SetVertice(1, aP1);
  myPntArray->SetVertice(2, aP2);
  return true;
}

void SketcherPrs_Collinear::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  // Draw constrained lines
  addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());
  addLine(aGroup, SketchPlugin_Constraint::ENTITY_B());
}

