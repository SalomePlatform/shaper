// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ResultPrs.cpp
// Created:     21 October 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_ResultPrs.h"
#include "ModuleBase_Tools.h"

#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultCompSolid.h>
#include <GeomAPI_PlanarEdges.h>

#include <Events_InfoMessage.h>
#include <Events_Loop.h>

#include <BRep_Builder.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Selection.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <TopExp_Explorer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_BRepOwner, StdSelect_BRepOwner);

//*******************************************************************************************

IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ResultPrs, ViewerData_AISShape);



ModuleBase_ResultPrs::ModuleBase_ResultPrs(ResultPtr theResult)
  : ViewerData_AISShape(TopoDS_Shape()), myResult(theResult), myAdditionalSelectionPriority(0)
{
  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(theResult);
  TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
  Set(aShape);
  Handle(Prs3d_Drawer) aDrawer = Attributes();
  if (aDrawer->HasOwnPointAspect())
    aDrawer->PointAspect()->SetTypeOfMarker(Aspect_TOM_PLUS);
  else
    aDrawer->SetPointAspect(new Prs3d_PointAspect(Aspect_TOM_PLUS, Quantity_NOC_YELLOW, 1.));

  // Activate individual repaintng if this is a part of compsolid
  ResultCompSolidPtr aCompSolid = ModelAPI_Tools::compSolidOwner(myResult);
  SetAutoHilight(aCompSolid.get() == NULL);

  ModuleBase_Tools::setPointBallHighlighting(this);
}

void ModuleBase_ResultPrs::setAdditionalSelectionPriority(const int thePriority)
{
  myAdditionalSelectionPriority = thePriority;
}

void ModuleBase_ResultPrs::Compute(
          const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
          const Handle(Prs3d_Presentation)& thePresentation,
          const Standard_Integer theMode)
{
  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(myResult);
  bool aReadyToDisplay = aShapePtr.get();
  if (aReadyToDisplay) {
    myOriginalShape = aShapePtr->impl<TopoDS_Shape>();
    if (!myOriginalShape.IsNull())
      Set(myOriginalShape);
  }
  // change deviation coefficient to provide more precise circle
  //ModuleBase_Tools::setDefaultDeviationCoefficient(myResult, Attributes());
  AIS_Shape::Compute(thePresentationManager, thePresentation, theMode);

  if (!aReadyToDisplay) {
    Events_InfoMessage("ModuleBase_ResultPrs",
                       "An empty AIS presentation: ModuleBase_ResultPrs").send();
    static const Events_ID anEvent = Events_Loop::eventByName(EVENT_EMPTY_AIS_PRESENTATION);
    ModelAPI_EventCreator::get()->sendUpdated(myResult, anEvent);
  }
}

void ModuleBase_ResultPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  if (aMode > TopAbs_SHAPE) {
    // In order to avoid using custom selection modes
    if (aMode == ModuleBase_ResultPrs::Sel_Result) {
      AIS_Shape::ComputeSelection(aSelection, TopAbs_COMPOUND);
    }
    return;
  }

  // TODO: OCCT issue should be created for the COMPOUND processing
  // before it is fixed, the next workaround in necessary
  if (aMode == AIS_Shape::SelectionMode(TopAbs_COMPOUND)) {
    const TopoDS_Shape& aShape = Shape();
    TopExp_Explorer aCompExp(aShape, TopAbs_COMPOUND);
    // do not activate in compound mode shapes which do not contain compounds
    if (!aCompExp.More())
      return;
  }

  if (aMode == AIS_Shape::SelectionMode(TopAbs_COMPSOLID)) {
    // Limit selection area only by actual object (Shape)
    ResultCompSolidPtr aCompSolid = ModelAPI_Tools::compSolidOwner(myResult);
    if (aCompSolid.get()) {
      std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aCompSolid);
      if (aShapePtr.get()) {
        TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
        int aPriority = StdSelect_BRepSelectionTool::GetStandardPriority(aShape, TopAbs_COMPSOLID);
        /// It is important to have priority for the shape of comp solid result less than priority
        /// for the presentation shape which is a sub-result.
        /// Reason is to select the sub-objects before: #1592
        aPriority = aPriority - 1;
        double aDeflection = Prs3d::GetDeflection(aShape, myDrawer);

        Handle(ModuleBase_BRepOwner) aOwner = new ModuleBase_BRepOwner(aShape, aPriority);
        StdSelect_BRepSelectionTool::ComputeSensitive(aShape, aOwner, aSelection,
          aDeflection, myDrawer->HLRAngle(), 9, 500);

        for (aSelection->Init(); aSelection->More(); aSelection->Next()) {
          Handle(SelectMgr_EntityOwner) anOwner =
            Handle(SelectMgr_EntityOwner)
            ::DownCast(aSelection->Sensitive()->BaseSensitive()->OwnerId());
          anOwner->Set(this);
        }
        return;
      }
    }
    //AIS_Shape::ComputeSelection(aSelection, 0);
  }
  AIS_Shape::ComputeSelection(aSelection, aMode);

  if (myAdditionalSelectionPriority > 0) {
    for (aSelection->Init(); aSelection->More(); aSelection->Next()) {
      Handle(SelectBasics_EntityOwner) aBasicsOwner =
        aSelection->Sensitive()->BaseSensitive()->OwnerId();
      if (!aBasicsOwner.IsNull())
        aBasicsOwner->Set(aBasicsOwner->Priority() + myAdditionalSelectionPriority);
    }
  }
}

void ModuleBase_ResultPrs::appendWiresSelection(const Handle(SelectMgr_Selection)& theSelection,
                                                const TopoDS_Shape& theShape)
{
  static TopAbs_ShapeEnum TypOfSel
          = AIS_Shape::SelectionType(AIS_Shape::SelectionMode(TopAbs_WIRE));
  // POP protection against crash in low layers
  Standard_Real aDeflection = Prs3d::GetDeflection(theShape, myDrawer);
  try {
    StdSelect_BRepSelectionTool::Load(theSelection,
                                      this,
                                      theShape,
                                      TypOfSel,
                                      aDeflection,
                                      myDrawer->HLRAngle(),
                                      myDrawer->IsAutoTriangulation());
  } catch ( Standard_Failure ) {
  }
}

void ModuleBase_ResultPrs::HilightSelected(const Handle(PrsMgr_PresentationManager3d)& thePM,
                                           const SelectMgr_SequenceOfOwner& theOwners)
{
  Handle(SelectMgr_EntityOwner) anOwner;
  Handle(ModuleBase_BRepOwner) aCompOwner;
  for (int i = 1; i <= theOwners.Length(); i++) {
    anOwner = theOwners.Value(i);
    aCompOwner = Handle(ModuleBase_BRepOwner)::DownCast(anOwner);
    if (aCompOwner.IsNull()) {
      thePM->Color(anOwner->Selectable(), GetContext()->SelectionStyle());
    }
    else {
      TopoDS_Shape aShape = aCompOwner->Shape();
      Handle( Prs3d_Presentation ) aSelectionPrs = GetSelectPresentation( thePM );
      aSelectionPrs->Clear();

      StdPrs_WFShape::Add(aSelectionPrs, aShape, myDrawer);

      aSelectionPrs->SetDisplayPriority(9);
      aSelectionPrs->Highlight(GetContext()->SelectionStyle());
      aSelectionPrs->Display();
      thePM->Color(this, GetContext()->SelectionStyle());
    }
  }
}

void ModuleBase_ResultPrs::HilightOwnerWithColor(const Handle(PrsMgr_PresentationManager3d)& thePM,
                                                 const Handle(Graphic3d_HighlightStyle)& theStyle,
                                                 const Handle(SelectMgr_EntityOwner)& theOwner)
{
  Handle(StdSelect_BRepOwner) aOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
  if (aOwner.IsNull())
    return;

  TopoDS_Shape aShape = aOwner->Shape();
  if (!aShape.IsNull()) {
    thePM->Color(this, theStyle);

    Handle( Prs3d_Presentation ) aHilightPrs = GetHilightPresentation( thePM );
    aHilightPrs->Clear();

    StdPrs_WFShape::Add(aHilightPrs, aShape, myDrawer);
    aHilightPrs->Highlight(theStyle);

    if (thePM->IsImmediateModeOn())
      thePM->AddToImmediateList(aHilightPrs);
  }
}
