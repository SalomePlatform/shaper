// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Coincident.cpp
// Created:     12 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Coincident.h"
#include "SketcherPrs_Tools.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>

#include <GeomAPI_XYZ.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Pnt2d.h>

#include <Events_Error.h>

#include <SketchPlugin_Constraint.h>

#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Root.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Select3D_SensitivePoint.hxx>


IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Coincident, AIS_InteractiveObject);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Coincident, AIS_InteractiveObject);

SketcherPrs_Coincident::SketcherPrs_Coincident(ModelAPI_Feature* theConstraint, 
                                               const std::shared_ptr<GeomAPI_Ax3>& thePlane) 
 : AIS_InteractiveObject(), myConstraint(theConstraint), myPlane(thePlane)
{
  
}  



void SketcherPrs_Coincident::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  // Get point of the presentation
  std::shared_ptr<GeomAPI_Pnt2d> aPnt = SketcherPrs_Tools::getPoint(myConstraint, 
                                                                    SketchPlugin_Constraint::ENTITY_A());
  if (aPnt.get() == NULL)
    aPnt = SketcherPrs_Tools::getPoint(myConstraint, SketchPlugin_Constraint::ENTITY_B());
  if (aPnt.get() == NULL) {
    Events_Error::throwException("An empty AIS presentation: SketcherPrs_Coincident");
    return;
  }
  std::shared_ptr<GeomAPI_Pnt> aPoint = myPlane->to3D(aPnt->x(), aPnt->y());
  myPoint = aPoint->impl<gp_Pnt>();

  static Handle(Graphic3d_AspectMarker3d) aPtA;
  if (aPtA.IsNull()) {
    aPtA = new Graphic3d_AspectMarker3d ();
  }
  // Create the presentation as a combination of standard point markers

  // The external yellow contour
  aPtA->SetType(Aspect_TOM_RING3);
  aPtA->SetScale(2.);
  aPtA->SetColor(Quantity_NOC_YELLOW);

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);
  aGroup->SetPrimitivesAspect(aPtA);
  Handle(Graphic3d_ArrayOfPoints) aPntArray = new Graphic3d_ArrayOfPoints(1);
  aPntArray->AddVertex (aPoint->x(), aPoint->y(), aPoint->z());
  aGroup->AddPrimitiveArray (aPntArray);

  // Make a black mid ring
  aPtA->SetType(Aspect_TOM_RING1);
  aPtA->SetScale(1.);
  aPtA->SetColor(Quantity_NOC_BLACK);
  aGroup->SetPrimitivesAspect(aPtA);
  aGroup->AddPrimitiveArray (aPntArray);

  // Make an internal ring
  aPtA->SetType(Aspect_TOM_POINT);
  aPtA->SetScale(5.);
  aGroup->SetPrimitivesAspect(aPtA);
  aGroup->AddPrimitiveArray (aPntArray);
}


void SketcherPrs_Coincident::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  // There is no selection of coincident - a point is selected instead of coincidence
}

void SketcherPrs_Coincident::SetColor(const Quantity_NameOfColor aCol)
{
  SetColor(Quantity_Color(aCol));
}

void SketcherPrs_Coincident::SetColor(const Quantity_Color &aCol)
{
  hasOwnColor=Standard_True;
  myOwnColor=aCol;
}
