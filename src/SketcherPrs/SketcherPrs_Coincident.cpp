// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SketcherPrs_Coincident.h"
#include "SketcherPrs_Tools.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Events.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>

#include <GeomAPI_XYZ.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Pnt2d.h>

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintCoincidence.h>

#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Root.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Select3D_SensitivePoint.hxx>


IMPLEMENT_STANDARD_RTTIEXT(SketcherPrs_Coincident, AIS_InteractiveObject);

SketcherPrs_Coincident::SketcherPrs_Coincident(ModelAPI_Feature* theConstraint,
  SketchPlugin_Sketch* theSketch)
: AIS_InteractiveObject(), myConstraint(theConstraint), mySketch(theSketch),
  myPoint(gp_Pnt(0.0, 0.0, 0.0)), myIsCustomColor(false)
{
}

bool SketcherPrs_Coincident::IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                                              const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  gp_Pnt aPoint;
  return readyToDisplay(theConstraint, thePlane, aPoint);
}

std::shared_ptr<GeomAPI_Pnt2d> getCoincidencePoint(ModelAPI_Feature* theConstraint)
{
  std::shared_ptr<GeomAPI_Pnt2d> aPnt = SketcherPrs_Tools::getPoint(theConstraint,
    SketchPlugin_Constraint::ENTITY_A());
  if (aPnt.get() == NULL)
    aPnt = SketcherPrs_Tools::getPoint(theConstraint, SketchPlugin_Constraint::ENTITY_B());

  return aPnt;
}


bool SketcherPrs_Coincident::readyToDisplay(ModelAPI_Feature* theConstraint,
                                            const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                            gp_Pnt& thePoint)
{
  bool aReadyToDisplay = false;
  // Get point of the presentation
  std::shared_ptr<GeomAPI_Pnt2d> aPnt = getCoincidencePoint(theConstraint);
  aReadyToDisplay = aPnt.get() != NULL;
  if (aReadyToDisplay) {
    std::shared_ptr<GeomAPI_Pnt> aPoint = thePlane->to3D(aPnt->x(), aPnt->y());
    thePoint = aPoint->impl<gp_Pnt>();
  }
  return aReadyToDisplay;
}


bool hasEdge(ModelAPI_Feature* theConstraint)
{
  ObjectPtr aObjA =
    SketcherPrs_Tools::getResult(theConstraint, SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObjB =
    SketcherPrs_Tools::getResult(theConstraint, SketchPlugin_Constraint::ENTITY_B());
  ResultPtr aResA = std::dynamic_pointer_cast<ModelAPI_Result>(aObjA);
  ResultPtr aResB = std::dynamic_pointer_cast<ModelAPI_Result>(aObjB);
  if (aResA.get()) {
    GeomAPI_Shape::ShapeType aTypeA = aResA->shape()->shapeType();
    if (aTypeA == GeomAPI_Shape::EDGE)
      return true;
  }
  if (aResB.get()) {
    GeomAPI_Shape::ShapeType aTypeB = aResB->shape()->shapeType();
    if (aTypeB == GeomAPI_Shape::EDGE)
      return true;
  }
  return false;
}


void SketcherPrs_Coincident::Compute(
  const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
  const Handle(Prs3d_Presentation)& thePresentation,
  const Standard_Integer theMode)
{
  gp_Pnt aPoint;
  bool aReadyToDisplay = readyToDisplay(myConstraint, mySketch->coordinatePlane(), aPoint);
  if (aReadyToDisplay)
    myPoint = aPoint;

  bool aIsEdge = hasEdge(myConstraint);
  if (!aIsEdge) {
    std::shared_ptr<GeomAPI_Pnt2d> aPnt = getCoincidencePoint(myConstraint);
    std::shared_ptr<GeomAPI_Pnt2d> aP;
    FeaturePtr aSub;
    int aNumberOfSubs = mySketch->numberOfSubs();
    for (int i = 0; i < aNumberOfSubs; i++) {
      aSub = mySketch->subFeature(i);
      if (aSub->getKind() == SketchPlugin_ConstraintCoincidence::ID() &&
        aSub.get() != myConstraint) {
        aP = getCoincidencePoint(aSub.get());
        if (aP && aP->isEqual(aPnt)) {
          aIsEdge = hasEdge(aSub.get());
          if (aIsEdge)
            break;
        }
      }
    }
  }
  Quantity_Color aExternalColor = aIsEdge ? Quantity_NOC_BLACK : Quantity_NOC_YELLOW;
  Quantity_Color aInternalColor = aIsEdge ? Quantity_NOC_YELLOW : Quantity_NOC_BLACK;

  int aRatio = SketcherPrs_Tools::pixelRatio();

  // Create the presentation as a combination of standard point markers
  bool aCustomColor = myIsCustomColor;
  // The external yellow contour
  Handle(Graphic3d_AspectMarker3d) aPtA = new Graphic3d_AspectMarker3d();
  aPtA->SetType(Aspect_TOM_RING3);
  aPtA->SetScale(2. * aRatio);
  aPtA->SetColor(!aCustomColor ? aExternalColor : myCustomColor);

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);
  aGroup->SetPrimitivesAspect(aPtA);
  Handle(Graphic3d_ArrayOfPoints) aPntArray = new Graphic3d_ArrayOfPoints(1);
  aPntArray->AddVertex (myPoint.X(), myPoint.Y(), myPoint.Z());
  aGroup->AddPrimitiveArray (aPntArray);

  // Make a black mid ring
  aPtA = new Graphic3d_AspectMarker3d();
  aPtA->SetType(aIsEdge ? Aspect_TOM_STAR : Aspect_TOM_RING1);
  aPtA->SetScale(1. * aRatio);
  aPtA->SetColor(!aCustomColor ? aInternalColor : myCustomColor);
  aGroup->SetPrimitivesAspect(aPtA);
  aGroup->AddPrimitiveArray (aPntArray);

  // Make an internal ring
  aPtA = new Graphic3d_AspectMarker3d();
  aPtA->SetType(Aspect_TOM_POINT);
  aPtA->SetScale(5. * aRatio);
  aPtA->SetColor(!aCustomColor ? aInternalColor : myCustomColor);
  aGroup->SetPrimitivesAspect(aPtA);
  aGroup->AddPrimitiveArray (aPntArray);

  if (!aReadyToDisplay)
    SketcherPrs_Tools::sendEmptyPresentationError(myConstraint,
                              "An empty AIS presentation: SketcherPrs_Coincident");
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
}

void SketcherPrs_Coincident::SetCustomColor(const std::vector<int>& theColor)
{
  myIsCustomColor = !theColor.empty();
  if (myIsCustomColor)
    myCustomColor = Quantity_Color(theColor[0] / 255., theColor[1] / 255.,
                                   theColor[2] / 255., Quantity_TOC_RGB);
  else
    myCustomColor = Quantity_Color();
}
