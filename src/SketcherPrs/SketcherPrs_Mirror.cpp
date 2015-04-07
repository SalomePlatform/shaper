// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Mirror.cpp
// Created:     6 April 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Mirror.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Constraint.h>

#include <ModelAPI_AttributeRefList.h>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>



IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Mirror, SketcherPrs_SymbolPrs);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Mirror, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Mirror::SketcherPrs_Mirror(ModelAPI_Feature* theConstraint, 
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane)
{
}  


bool SketcherPrs_Mirror::updatePoints(double theStep) const
{
  ObjectPtr aAxisObj = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  if (SketcherPrs_Tools::getShape(aAxisObj).get() == NULL)
    return false;

  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB = aData->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (anAttrB.get() == NULL)
    return false;
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrC = aData->reflist(SketchPlugin_Constraint::ENTITY_C());
  if (anAttrC.get() == NULL)
    return false;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  int aNb = anAttrB->size();
  if (aNb != anAttrC->size())
    return false;

  myPntArray = new Graphic3d_ArrayOfPoints(2 * aNb);
  int i;
  ObjectPtr aObj;
  gp_Pnt aP1;
  for (i = 0; i < aNb; i++) {
    aObj = anAttrB->object(i);
    aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->SetVertice(i + 1, aP1);
  }  
  for (i = 0; i < aNb; i++) {
    aObj = anAttrC->object(i);
    aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->SetVertice(aNb + i + 1, aP1);
  }  
  return true;
}


void SketcherPrs_Mirror::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB = aData->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (anAttrB.get() == NULL)
    return;
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrC = aData->reflist(SketchPlugin_Constraint::ENTITY_C());
  if (anAttrC.get() == NULL)
    return;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
  int aNb = anAttrB->size();
  if (aNb != anAttrC->size())
    return;

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  // Draw axis line
  addLine(aGroup, SketchPlugin_Constraint::ENTITY_A());

  // Draw source objects
  int i;
  ObjectPtr aObj;
  for (i = 0; i < aNb; i++) {
    aObj = anAttrB->object(i);
    std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getShape(aObj);
    if (aShape.get() != NULL)
      drawShape(aShape, thePrs);
  }
  // draw mirrored objects
  for (i = 0; i < aNb; i++) {
    aObj = anAttrC->object(i);
    std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getShape(aObj);
    if (aShape.get() != NULL)
      drawShape(aShape, thePrs);
  }
}

