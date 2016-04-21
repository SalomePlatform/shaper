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

#include <ModelAPI_AttributeRefAttr.h>

#include <SketchPlugin_Constraint.h>

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



SketcherPrs_Rigid::SketcherPrs_Rigid(ModelAPI_Feature* theConstraint, 
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane) 
 : SketcherPrs_SymbolPrs(theConstraint, thePlane)
{
  // Set default points array
  myPntArray = new Graphic3d_ArrayOfPoints(1);
  myPntArray->AddVertex(0., 0., 0.);
}  

bool SketcherPrs_Rigid::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                         const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  bool aReadyToDisplay = false;

  std::shared_ptr<ModelAPI_Data> aData = theConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = aData->refattr(SketchPlugin_Constraint::ENTITY_A());
  AttributePtr aRefAttr = anAttr->attr();
  if (anAttr->isObject()) {
    // The constraint attached to an object
    ObjectPtr aObj = anAttr->object();
    aReadyToDisplay =  SketcherPrs_Tools::getShape(aObj).get() != NULL;

  } else {
    // The constraint attached to a point
    std::shared_ptr<GeomAPI_Pnt2d> aPnt = SketcherPrs_Tools::getPoint(theConstraint,
                                                                 SketchPlugin_Constraint::ENTITY_A());
    aReadyToDisplay = aPnt.get() != NULL;
  }
  return aReadyToDisplay;
}

bool SketcherPrs_Rigid::updateIfReadyToDisplay(double theStep) const 
{
  if (!IsReadyToDisplay(myConstraint, myPlane))
    return false;

  std::shared_ptr<ModelAPI_Data> aData = myConstraint->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = aData->refattr(SketchPlugin_Constraint::ENTITY_A());
  AttributePtr aRefAttr = anAttr->attr();
  if (anAttr->isObject()) {
    // The constraint attached to an object
    ObjectPtr aObj = anAttr->object();

    SketcherPrs_PositionMgr* aMgr = SketcherPrs_PositionMgr::get();
    gp_Pnt aP1 = aMgr->getPosition(aObj, this, theStep);
    myPntArray->SetVertice(1, aP1);
  } else {
    // The constraint attached to a point
    std::shared_ptr<GeomAPI_Pnt2d> aPnt = SketcherPrs_Tools::getPoint(myConstraint,
                                                  SketchPlugin_Constraint::ENTITY_A());
    std::shared_ptr<GeomAPI_Pnt> aPoint = myPlane->to3D(aPnt->x(), aPnt->y() + theStep);
    myPntArray->SetVertice(1, aPoint->impl<gp_Pnt>());
  }
  return true;
}


void SketcherPrs_Rigid::drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const
{
  ObjectPtr aObj = SketcherPrs_Tools::getResult(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
  std::shared_ptr<GeomAPI_Shape> aShape;
  if (aFeature.get()) {
    // If constraint attached to a feature
    const std::list<ResultPtr>& aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    // Find a shape
    for (aIt = aResults.cbegin(); aIt != aResults.cend(); aIt++) {
      aShape = (*aIt)->shape();
      if (aShape->isEdge())
        break;
    }
  } else {
    // Else it is a result
    aShape = SketcherPrs_Tools::getShape(aObj);
  }
  if (aShape.get() == NULL)
    return;

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(thePrs);
  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 2);
  aGroup->SetPrimitivesAspect(aLineAspect);

  Handle(Prs3d_PointAspect) aPntAspect = new Prs3d_PointAspect(Aspect_TOM_PLUS, theColor, 1);
  myDrawer->SetPointAspect(aPntAspect);
  drawShape(aShape, thePrs);
}

