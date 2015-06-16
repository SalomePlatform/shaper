// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Transformation.cpp
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Transformation.h"
#include "SketcherPrs_Tools.h"
#include "SketcherPrs_PositionMgr.h"

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefList.h>

#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Root.hxx>



IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Transformation, SketcherPrs_SymbolPrs);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Transformation, SketcherPrs_SymbolPrs);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Transformation::SketcherPrs_Transformation(ModelAPI_Feature* theConstraint, 
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                           bool isTranslation) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane), myIsTranslation(isTranslation)
{
  int aPointsNb = 0;
  if (theConstraint->getKind() == SketchPlugin_MultiTranslation::ID()) {
    aPointsNb = theConstraint->integer(SketchPlugin_MultiTranslation::NUMBER_OF_COPIES_ID())->value();
  } else if (theConstraint->getKind() == SketchPlugin_MultiRotation::ID()) {
    aPointsNb = theConstraint->integer(SketchPlugin_MultiRotation::NUMBER_OF_COPIES_ID())->value();
  }
  aPointsNb == 0 ? aPointsNb = 2 : aPointsNb++; // by default we have 2 points for symbols
  myPntArray = new Graphic3d_ArrayOfPoints(aPointsNb);
}  

bool SketcherPrs_Transformation::updatePoints(double theStep) const 
{
  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB = aData->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (anAttrB.get() == NULL)
    return false;

  int aNbB = anAttrB->size();
  if (aNbB == 0)
    return false;

  SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();

  int i;
  ObjectPtr aObj;
  gp_Pnt aP1;
  for (i = 0; i < aNbB; i++) {
    aObj = anAttrB->object(i);
    if (!aObj.get()) // TODO:empty_result
      // this check should be removed here after the result flush is corrected
      // the problem is, that feature::execute() flushes redisplay by each result creation
      // but it is possible(e.g. in the sketch circle, that there should be more than one result.
      // Here, crash happens, because the second result is not created yet
      continue;
    aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->SetVertice(i + 1, aP1);
  }  

  return true;
}

void SketcherPrs_Transformation::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefList> anAttrB = aData->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (anAttrB.get() == NULL)
    return;

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);

  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  drawListOfShapes(anAttrB, thePrs);
}

