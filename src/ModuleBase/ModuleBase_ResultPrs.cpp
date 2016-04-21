// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ResultPrs.cpp
// Created:     21 October 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_ResultPrs.h"
#include "ModuleBase_Tools.h"

#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <GeomAPI_PlanarEdges.h>

#include <Events_Error.h>
#include <Events_Loop.h>

#include <BRep_Builder.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <TopoDS_Builder.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Selection.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_BRepOwner, StdSelect_BRepOwner);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_BRepOwner, StdSelect_BRepOwner);

//*******************************************************************************************

IMPLEMENT_STANDARD_HANDLE(ModuleBase_ResultPrs, ViewerData_AISShape);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ResultPrs, ViewerData_AISShape);





ModuleBase_ResultPrs::ModuleBase_ResultPrs(ResultPtr theResult)
  : ViewerData_AISShape(TopoDS_Shape()), myResult(theResult)
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
}

bool ModuleBase_ResultPrs::isValidShapeType(const TopAbs_ShapeEnum& theBaseType,
                                            const TopAbs_ShapeEnum& theCheckedType)
{
  bool aValid = theBaseType == theCheckedType;
  if (!aValid) {
    // currently this functionality is for all, as we have no separate wire selection mode
    // lately it should be corrected to have the following check only for sketch presentations
    aValid = theBaseType == TopAbs_FACE && theCheckedType == TopAbs_WIRE;
  }
  return aValid;
}

void ModuleBase_ResultPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
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
  ModuleBase_Tools::setDefaultDeviationCoefficient(Shape(), Attributes());
  AIS_Shape::Compute(thePresentationManager, thePresentation, theMode);

  if (!aReadyToDisplay) {
    Events_Error::throwException("An empty AIS presentation: ModuleBase_ResultPrs");
    static const Events_ID anEvent = Events_Loop::eventByName(EVENT_EMPTY_AIS_PRESENTATION);
    ModelAPI_EventCreator::get()->sendUpdated(myResult, anEvent);
  }
}

void ModuleBase_ResultPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  if (aMode > 8)
    // In order to avoid using custom selection modes
    return;

  if (aMode == AIS_Shape::SelectionMode(TopAbs_COMPSOLID)) {
    // Limit selection area only by actual object (Shape)
    ResultCompSolidPtr aCompSolid = ModelAPI_Tools::compSolidOwner(myResult);
    if (aCompSolid.get()) {
      std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aCompSolid);
      if (aShapePtr.get()) {
        TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
        int aPriority = StdSelect_BRepSelectionTool::GetStandardPriority(aShape, TopAbs_SHAPE);
        double aDeflection = Prs3d::GetDeflection(aShape, myDrawer);

        Handle(ModuleBase_BRepOwner) aOwner = new ModuleBase_BRepOwner(aShape, aPriority);
        StdSelect_BRepSelectionTool::ComputeSensitive(aShape, aOwner, aSelection, 
          aDeflection, myDrawer->HLRAngle(), 9, 500);

        for (aSelection->Init(); aSelection->More(); aSelection->Next()) {
          Handle(SelectMgr_EntityOwner) anOwner =
            Handle(SelectMgr_EntityOwner)::DownCast(aSelection->Sensitive()->BaseSensitive()->OwnerId());
          anOwner->Set(this);
        }
        return;
      }
    }
    //AIS_Shape::ComputeSelection(aSelection, 0);
  } 
  AIS_Shape::ComputeSelection(aSelection, aMode);
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
    if (aCompOwner.IsNull())
      anOwner->Hilight(thePM);
    else {
      TopoDS_Shape aShape = aCompOwner->Shape();
      Handle( Prs3d_Presentation ) aSelectionPrs = GetSelectPresentation( thePM );
      aSelectionPrs->Clear();

      StdPrs_WFDeflectionShape::Add(aSelectionPrs, aShape, myDrawer);

      aSelectionPrs->SetDisplayPriority(9);
      aSelectionPrs->Highlight(Aspect_TOHM_COLOR, aSelectionPrs->HighlightColor());
      aSelectionPrs->Display();
      thePM->Highlight(this);
    }
  }
}
  
void ModuleBase_ResultPrs::HilightOwnerWithColor(const Handle(PrsMgr_PresentationManager3d)& thePM, 
                                                 const Quantity_NameOfColor theColor, 
                                                 const Handle(SelectMgr_EntityOwner)& theOwner)
{
  Handle(StdSelect_BRepOwner) aOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
  if (aOwner.IsNull())
    return;

  TopoDS_Shape aShape = aOwner->Shape();
  if (!aShape.IsNull()) {
    thePM->Color(this, theColor);

    Handle( Prs3d_Presentation ) aHilightPrs = GetHilightPresentation( thePM );
    aHilightPrs->Clear();

    StdPrs_WFDeflectionShape::Add(aHilightPrs, aShape, myDrawer);
    aHilightPrs->Highlight(Aspect_TOHM_COLOR, theColor);

    if (thePM->IsImmediateModeOn())
      thePM->AddToImmediateList(aHilightPrs);
  }
}
