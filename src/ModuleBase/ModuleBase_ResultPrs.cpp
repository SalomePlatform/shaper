// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModuleBase_ResultPrs.h"

#include <GeomAPI_PlanarEdges.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultCompSolid.h>

#include "ModuleBase_Tools.h"
#include "ModuleBase_BRepOwner.h"

#include <Events_InfoMessage.h>
#include <Events_Loop.h>

#include <AIS_ColoredDrawer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Selection.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>

//*******************************************************************************************

IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ResultPrs, ViewerData_AISShape);



//********************************************************************
ModuleBase_ResultPrs::ModuleBase_ResultPrs(ResultPtr theResult)
  : ViewerData_AISShape(TopoDS_Shape()), myResult(theResult), myAdditionalSelectionPriority(0),
  myTransparency(1)
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

  myHiddenSubShapesDrawer = new AIS_ColoredDrawer (myDrawer);
  Handle(Prs3d_ShadingAspect) aShadingAspect = new Prs3d_ShadingAspect();
  aShadingAspect->SetMaterial(Graphic3d_NOM_BRASS); //default value of context material
  myHiddenSubShapesDrawer->SetShadingAspect(aShadingAspect);

  ModuleBase_Tools::setPointBallHighlighting(this);
}

//********************************************************************
void ModuleBase_ResultPrs::setAdditionalSelectionPriority(const int thePriority)
{
  myAdditionalSelectionPriority = thePriority;
}

//********************************************************************
void ModuleBase_ResultPrs::SetColor (const Quantity_Color& theColor)
{
  ViewerData_AISShape::SetColor(theColor);
  myHiddenSubShapesDrawer->ShadingAspect()->SetColor (theColor, myCurrentFacingModel);
}

//********************************************************************
bool ModuleBase_ResultPrs::setSubShapeHidden(const NCollection_List<TopoDS_Shape>& theShapes)
{
  bool isModified = false;

  TopoDS_Compound aCompound;
  BRep_Builder aBBuilder;
  aBBuilder.MakeCompound (aCompound);
  // restore hidden shapes if there are not the shapes in parameter container
  NCollection_List<TopoDS_Shape> aVisibleSubShapes;
  for (NCollection_List<TopoDS_Shape>::Iterator aHiddenIt(myHiddenSubShapes); aHiddenIt.More();
       aHiddenIt.Next()) {
    if (!theShapes.Contains(aHiddenIt.Value()))
      aVisibleSubShapes.Append(aHiddenIt.Value());
    else
      aBBuilder.Add (aCompound, aHiddenIt.Value());
  }
  isModified = !aVisibleSubShapes.IsEmpty();
  for (NCollection_List<TopoDS_Shape>::Iterator aVisibleIt(aVisibleSubShapes); aVisibleIt.More();
       aVisibleIt.Next())
    myHiddenSubShapes.Remove(aVisibleIt.Value());

  // append hidden shapes into internal container if there are not these shapes
  for (NCollection_List<TopoDS_Shape>::Iterator aShapeIt(theShapes); aShapeIt.More();
    aShapeIt.Next())
  {
    if (aShapeIt.Value().ShapeType() != TopAbs_FACE) // only face shape can be hidden
      continue;

    if (!myHiddenSubShapes.Contains(aShapeIt.Value())) {
      myHiddenSubShapes.Append(aShapeIt.Value());
      aBBuilder.Add (aCompound, aShapeIt.Value());
      isModified = true;
    }
  }
  myHiddenCompound = aCompound;
  return isModified;
}

//********************************************************************
bool ModuleBase_ResultPrs::hasSubShapeVisible(
  const NCollection_List<TopoDS_Shape>& theShapesToSkip)
{
  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound (aCompound);
  NCollection_List<TopoDS_Shape> aShapesToSkip;
  aShapesToSkip.Append(myHiddenSubShapes);
  for (NCollection_List<TopoDS_Shape>::Iterator anIt(theShapesToSkip); anIt.More(); anIt.Next())
    aShapesToSkip.Append(anIt.Value());

  collectSubShapes(aBuilder, aCompound, myOriginalShape, aShapesToSkip);
  return !BOPTools_AlgoTools3D::IsEmptyShape(aCompound);
}

//********************************************************************
bool ModuleBase_ResultPrs::setHiddenSubShapeTransparency(double theTransparency)
{
  if (myTransparency == theTransparency || theTransparency > 1 || theTransparency < 0)
    return false;

  myTransparency = theTransparency;
  myHiddenSubShapesDrawer->ShadingAspect()->SetTransparency (theTransparency, myCurrentFacingModel);
  return true;
}

//********************************************************************
void ModuleBase_ResultPrs::Compute(
          const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
          const Handle(Prs3d_Presentation)& thePresentation,
          const Standard_Integer theMode)
{
  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(myResult);
  bool aReadyToDisplay = aShapePtr.get();
  if (aReadyToDisplay) {
    myOriginalShape = aShapePtr->impl<TopoDS_Shape>();
    if (myHiddenSubShapes.IsEmpty() || myOriginalShape.ShapeType() > TopAbs_FACE ) {
      if (!myOriginalShape.IsNull())
        Set(myOriginalShape);
    }
    else { // convert shape into SHELL
      TopoDS_Compound aCompound;
      BRep_Builder aBuilder;
      aBuilder.MakeCompound (aCompound);
      collectSubShapes(aBuilder, aCompound, myOriginalShape, myHiddenSubShapes);
      bool isEmptyShape = BOPTools_AlgoTools3D::IsEmptyShape(aCompound);
      Set(aCompound);
      if (isEmptyShape)
        aReadyToDisplay = false;
    }
  }
  // change deviation coefficient to provide more precise circle
  //ModuleBase_Tools::setDefaultDeviationCoefficient(myResult, Attributes());
  AIS_Shape::Compute(thePresentationManager, thePresentation, theMode);

  // visualize hidden sub-shapes transparent
  if (myTransparency < 1 && !myHiddenSubShapes.IsEmpty())
  {
    StdPrs_ShadedShape::Add (thePresentation, myHiddenCompound, myHiddenSubShapesDrawer);
    aReadyToDisplay = true;
  }

  if (!aReadyToDisplay) {
    Events_InfoMessage("ModuleBase_ResultPrs",
                       "An empty AIS presentation: ModuleBase_ResultPrs").send();
    static const Events_ID anEvent = Events_Loop::eventByName(EVENT_EMPTY_AIS_PRESENTATION);
    ModelAPI_EventCreator::get()->sendUpdated(myResult, anEvent);
  }
}

//********************************************************************
void ModuleBase_ResultPrs::collectSubShapes(BRep_Builder& theBuilder,
  TopoDS_Shape& theCompound, const TopoDS_Shape& theShape,
  const NCollection_List<TopoDS_Shape>& theHiddenSubShapes)
{
  switch (theShape.ShapeType()) {
    case TopAbs_COMPOUND: {
      for (TopoDS_Iterator aChildIter (theShape); aChildIter.More(); aChildIter.Next())
        collectSubShapes(theBuilder, theCompound, aChildIter.Value(), theHiddenSubShapes);
    }
    break;
    case TopAbs_SOLID:
    case TopAbs_SHELL: {
      for (TopExp_Explorer anExp (theShape, TopAbs_FACE); anExp.More(); anExp.Next()) {
        collectSubShapes(theBuilder, theCompound, anExp.Current(), theHiddenSubShapes);
      }
    }
    break;
    case TopAbs_WIRE: {
      for (TopExp_Explorer anExp (theShape, TopAbs_EDGE); anExp.More(); anExp.Next()) {
        collectSubShapes(theBuilder, theCompound, anExp.Current(), theHiddenSubShapes);
      }
    }
    break;
    case TopAbs_FACE: {
      if (theHiddenSubShapes.Contains(theShape))
        return; // remove hidden shape
      theBuilder.Add(theCompound, theShape);
    }
    break;
    case TopAbs_EDGE:
    case TopAbs_VERTEX: {
      theBuilder.Add(theCompound, theShape);
    }
    default:
      break;
  }
}

//********************************************************************
void ModuleBase_ResultPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer theMode)
{
  if (appendVertexSelection(aSelection, theMode))
    return;

  if (theMode > TopAbs_SHAPE) {
    // In order to avoid using custom selection modes
    if (theMode == ModuleBase_ResultPrs::Sel_Result) {
      AIS_Shape::ComputeSelection(aSelection, TopAbs_COMPOUND);
    }
    return;
  }

  // TODO: OCCT issue should be created for the COMPOUND processing
  // before it is fixed, the next workaround in necessary
  if (theMode == AIS_Shape::SelectionMode(TopAbs_COMPOUND)) {
    const TopoDS_Shape& aShape = Shape();
    TopExp_Explorer aCompExp(aShape, TopAbs_COMPOUND);
    // do not activate in compound mode shapes which do not contain compounds
    if (!aCompExp.More())
      return;
  }

  if (theMode == AIS_Shape::SelectionMode(TopAbs_COMPSOLID)) {
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
  AIS_Shape::ComputeSelection(aSelection, theMode);

  if (myAdditionalSelectionPriority > 0) {
    for (aSelection->Init(); aSelection->More(); aSelection->Next()) {
      Handle(SelectBasics_EntityOwner) aBasicsOwner =
        aSelection->Sensitive()->BaseSensitive()->OwnerId();
      if (!aBasicsOwner.IsNull())
        aBasicsOwner->Set(aBasicsOwner->Priority() + myAdditionalSelectionPriority);
    }
  }
}

//********************************************************************
bool ModuleBase_ResultPrs::appendVertexSelection(const Handle(SelectMgr_Selection)& aSelection,
                                                 const Standard_Integer theMode)
{
  if (Shape().ShapeType() == TopAbs_VERTEX) {
    const TopoDS_Shape& aShape = Shape();

    int aPriority = StdSelect_BRepSelectionTool::GetStandardPriority(aShape, TopAbs_VERTEX);
    double aDeflection = Prs3d::GetDeflection(aShape, myDrawer);

    /// The cause of this method is the last parameter of BRep owner setting into True.
    /// That means that owner should behave like it comes from decomposition. (In this case, OCCT
    /// visualizes it in Ring style) OCCT version is 7.0.0 with path for SHAPER module.
    Handle(StdSelect_BRepOwner) aOwner = new StdSelect_BRepOwner(aShape, aPriority, Standard_True);
    StdSelect_BRepSelectionTool::ComputeSensitive(aShape, aOwner, aSelection,
                                                  aDeflection, myDrawer->HLRAngle(), 9, 500);

    for (aSelection->Init(); aSelection->More(); aSelection->Next()) {
      Handle(SelectMgr_EntityOwner) anOwner =
        Handle(SelectMgr_EntityOwner)
        ::DownCast(aSelection->Sensitive()->BaseSensitive()->OwnerId());
      anOwner->Set(this);
    }
    return true;
  }
  return false;
}

//********************************************************************
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

//********************************************************************
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
