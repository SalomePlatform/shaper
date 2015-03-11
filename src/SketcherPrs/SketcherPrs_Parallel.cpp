// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Parallel.cpp
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Parallel.h"
#include "SketcherPrs_Tools.h"

#include <GeomAPI_Pnt.h>

#include <SketchPlugin_Constraint.h>

#include <AIS_Drawer.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Root.hxx>
#include <gp_Pnt2d.hxx>
#include <Image_AlienPixMap.hxx>
#include <Graphic3d_MarkerImage.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <SelectMgr_Selection.hxx>
#include <Select3D_SensitiveSegment.hxx>

extern std::shared_ptr<GeomAPI_Pnt2d> getFeaturePoint(DataPtr theData,
                                                      const std::string& theAttribute);

#ifdef WIN32
# define FSEP "\\"
#else
# define FSEP "/"
#endif


IMPLEMENT_STANDARD_HANDLE(SketcherPrs_Parallel, AIS_InteractiveObject);
IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Parallel, AIS_InteractiveObject);

static Handle(Image_AlienPixMap) MyPixMap;

SketcherPrs_Parallel::SketcherPrs_Parallel(SketchPlugin_Constraint* theConstraint, 
                                           const std::shared_ptr<GeomAPI_Ax3>& thePlane) 
 : AIS_InteractiveObject(), myConstraint(theConstraint), myPlane(thePlane)
{
  if (MyPixMap.IsNull()) {
    TCollection_AsciiString aFile(getenv("NewGeomResources"));
    aFile += FSEP;
    aFile += "parallel.png";
    MyPixMap = new Image_AlienPixMap();
    if (!MyPixMap->Load(aFile))
      MyPixMap.Nullify();
  }
  if (!MyPixMap.IsNull()) {
    myAspect = new Graphic3d_AspectMarker3d(MyPixMap);
    myPntArray = new Graphic3d_ArrayOfPoints(2);
    myPntArray->AddVertex(0., 0., 0.);
    myPntArray->AddVertex(0. ,0., 0.);
  }
}  

void SketcherPrs_Parallel::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  if (myAspect.IsNull())
    return;

  std::shared_ptr<GeomAPI_Edge> aLine1 = SketcherPrs_Tools::getLine(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  if (aLine1.get() == NULL)
    return;

  std::shared_ptr<GeomAPI_Edge> aLine2 = SketcherPrs_Tools::getLine(myConstraint, SketchPlugin_Constraint::ENTITY_B());
  if (aLine2.get() == NULL)
    return;

  std::shared_ptr<GeomAPI_Pnt> aPnt1 = aLine1->firstPoint();
  std::shared_ptr<GeomAPI_Pnt> aPnt2 = aLine1->lastPoint();
  gp_Pnt aP1((aPnt1->x() + aPnt2->x())/2.,
             (aPnt1->y() + aPnt2->y())/2.,
             (aPnt1->z() + aPnt2->z())/2.);

  gp_Vec aVec1(aPnt1->impl<gp_Pnt>(), aPnt2->impl<gp_Pnt>());
  gp_Vec aShift = aVec1.Crossed(myPlane->norm()->impl<gp_Dir>());
  aShift.Normalize();
  aShift.Multiply(20);
  aP1.Translate(aShift);

  aPnt1 = aLine2->firstPoint();
  aPnt2 = aLine2->lastPoint();
  gp_Pnt aP2((aPnt1->x() + aPnt2->x())/2.,
             (aPnt1->y() + aPnt2->y())/2.,
             (aPnt1->z() + aPnt2->z())/2.);

  gp_Vec aVec2(aPnt1->impl<gp_Pnt>(), aPnt2->impl<gp_Pnt>());
  aShift = aVec1.Crossed(myPlane->norm()->impl<gp_Dir>());
  aShift.Normalize();
  aShift.Multiply(20);
  aP2.Translate(aShift);

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);
  aGroup->SetPrimitivesAspect(myAspect);
  myPntArray->SetVertice(1, aP1);
  myPntArray->SetVertice(2, aP2);
  aGroup->AddPrimitiveArray(myPntArray);
}

void SketcherPrs_Parallel::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  std::shared_ptr<GeomAPI_Edge> aLine1 = SketcherPrs_Tools::getLine(myConstraint, SketchPlugin_Constraint::ENTITY_A());
  if (aLine1.get() == NULL)
    return;

  std::shared_ptr<GeomAPI_Edge> aLine2 = SketcherPrs_Tools::getLine(myConstraint, SketchPlugin_Constraint::ENTITY_B());
  if (aLine2.get() == NULL)
    return;

  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitivePoint) aSP1 = new Select3D_SensitivePoint(eown, myPntArray->Vertice(1));
  Handle(Select3D_SensitivePoint) aSP2 = new Select3D_SensitivePoint(eown, myPntArray->Vertice(2));
  aSelection->Add(aSP1);
  aSelection->Add(aSP2);
}

