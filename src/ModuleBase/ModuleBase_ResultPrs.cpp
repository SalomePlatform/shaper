// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ResultPrs.cpp
// Created:     21 October 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_ResultPrs.h"
#include "ModuleBase_Tools.h"

#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <GeomAPI_PlanarEdges.h>

#include <Events_Error.h>

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
  : ViewerData_AISShape(TopoDS_Shape()), myResult(theResult), myIsSketchMode(false)
{
  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(theResult);
  std::shared_ptr<GeomAPI_PlanarEdges> aWirePtr = 
    std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aShapePtr);
  if (aWirePtr) {
    if (aWirePtr->hasPlane() ) {
      // If this is a wire with plane defined thin it is a sketch-like object
      // It must have invisible faces
      myIsSketchMode = true;
    }
  }
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


void ModuleBase_ResultPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(myResult);
  if (!aShapePtr) {
    Events_Error::throwException("An empty AIS presentation: ModuleBase_ResultPrs");
    return;
  }

  if (myIsSketchMode) {
    myFacesList.clear();
    ResultConstructionPtr aConstruction = 
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(myResult);
    if (aConstruction.get()) {
      int aFacesNum = aConstruction->facesNum();
      for(int aFaceIndex = 0; aFaceIndex < aFacesNum; aFaceIndex++) {
        myFacesList.push_back(aConstruction->face(aFaceIndex));
      }
    }
  }
  myOriginalShape = aShapePtr->impl<TopoDS_Shape>();
  if (!myOriginalShape.IsNull()) {
    Set(myOriginalShape);

    // change deviation coefficient to provide more precise circle
    ModuleBase_Tools::setDefaultDeviationCoefficient(myOriginalShape, Attributes());
    AIS_Shape::Compute(thePresentationManager, thePresentation, theMode);
  }
  else
    Events_Error::throwException("An empty AIS presentation: ModuleBase_ResultPrs");
}


void ModuleBase_ResultPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  if (aMode > 8)
    // In order to avoid using custom selection modes
    return;

  
  if (myIsSketchMode) {
    if (aMode == AIS_Shape::SelectionMode(TopAbs_FACE)) {
      BRep_Builder aBuilder;
      TopoDS_Compound aComp;
      aBuilder.MakeCompound(aComp);
      aBuilder.Add(aComp, myOriginalShape);
      std::list<std::shared_ptr<GeomAPI_Shape>>::const_iterator aIt;
      for (aIt = myFacesList.cbegin(); aIt != myFacesList.cend(); ++aIt) {
        TopoDS_Shape aFace = (*aIt)->impl<TopoDS_Shape>();
        aBuilder.Add(aComp, aFace);
      }
      Set(aComp);
    } else
      Set(myOriginalShape);
  } 
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


bool ModuleBase_ResultPrs::hasCompSolidSelectionMode() const
{
  if (!HasInteractiveContext()) 
    return false;

  Handle(AIS_InteractiveContext) aContext = GetContext();
  TColStd_ListOfInteger aModes;
  aContext->ActivatedModes(this, aModes);

  TColStd_ListIteratorOfListOfInteger aIt(aModes);
  for (; aIt.More(); aIt.Next()) {
    if (aIt.Value() == AIS_Shape::SelectionMode(TopAbs_COMPSOLID)) 
      return true;
  }
  return false;
}


TopoDS_Shape ModuleBase_ResultPrs::getSelectionShape() const
{
  if (hasCompSolidSelectionMode()) {
    // In case of CompSolid mode use shape from Parent for highlighting
    ResultCompSolidPtr aCompSolid = ModelAPI_Tools::compSolidOwner(myResult);
    if (aCompSolid.get()) {
      std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aCompSolid);
      if (aShapePtr.get()) 
        return aShapePtr->impl<TopoDS_Shape>();
    }
  } 
  return myOriginalShape;
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
