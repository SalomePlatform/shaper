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

#include "PartSet_ResultSketchPrs.h"
#include "PartSet_Tools.h"
#include "ModuleBase_Tools.h"

#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <GeomAPI_PlanarEdges.h>

#include <Events_InfoMessage.h>
#include <Events_Loop.h>

#include <SketchPlugin_SketchEntity.h>

#include <SketcherPrs_Tools.h>

#include <Config_PropManager.h>

#include <BRep_Builder.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d.hxx>
#include <Prs3d_PointAspect.hxx>
#include <TopoDS_Builder.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Graphic3d_AspectMarker3d.hxx>

#define DEBUG_WIRE

#ifdef DEBUG_WIRE
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#endif

//*******************************************************************************************

IMPLEMENT_STANDARD_RTTIEXT(PartSet_ResultSketchPrs, ViewerData_AISShape);

PartSet_ResultSketchPrs::PartSet_ResultSketchPrs(ResultPtr theResult)
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
  //aDrawer->SetTypeOfDeflection(Aspect_TOD_ABSOLUTE);

  // Activate individual repaintng if this is a part of compsolid
  ResultBodyPtr anOwner = ModelAPI_Tools::bodyOwner(myResult);
  SetAutoHilight(anOwner.get() == NULL);

  ModuleBase_Tools::setPointBallHighlighting(this);
}

void PartSet_ResultSketchPrs::Compute(
                      const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                      const Handle(Prs3d_Presentation)& thePresentation,
                      const Standard_Integer theMode)
{
  thePresentation->Clear();

  TopoDS_Shape aResultShape;
  TopoDS_Compound anAuxiliaryCompound;
  NCollection_List<TopoDS_Shape> aFaceList;
  fillShapes(aResultShape, anAuxiliaryCompound, mySketchFaceList);

  bool aReadyToDisplay = !aResultShape.IsNull() || !anAuxiliaryCompound.IsNull();

  if (aReadyToDisplay) {
    if (!aResultShape.IsNull()) {
      myOriginalShape = aResultShape;
      if (!myOriginalShape.IsNull())
        Set(myOriginalShape);
    }
    myAuxiliaryCompound = anAuxiliaryCompound;
  }

  setAuxiliaryPresentationStyle(false);

  // change deviation coefficient to provide more precise circle
  ModuleBase_Tools::setDefaultDeviationCoefficient(Shape(), DynamicHilightAttributes());
  ModuleBase_Tools::setDefaultDeviationCoefficient(Shape(), Attributes());
  try {
    AIS_Shape::Compute(thePresentationManager, thePresentation, theMode);
  }
  catch (...) {
    return;
  }
  if (!myAuxiliaryCompound.IsNull()) {
    setAuxiliaryPresentationStyle(true);

    Handle(Prs3d_Drawer) aDrawer = Attributes();
    StdPrs_WFShape::Add(thePresentation, myAuxiliaryCompound, aDrawer);
  }

  if (!aReadyToDisplay) {
    Events_InfoMessage("PartSet_ResultSketchPrs",
                       "An empty AIS presentation: PartSet_ResultSketchPrs").send();
    static const Events_ID anEvent = Events_Loop::eventByName(EVENT_EMPTY_AIS_PRESENTATION);
    ModelAPI_EventCreator::get()->sendUpdated(myResult, anEvent);
  }
}

#ifdef DEBUG_WIRE
void debugInfo(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  TopTools_IndexedMapOfShape aSubShapes;
  TopExp::MapShapes (theShape, theType, aSubShapes);

  Standard_Boolean
    isComesFromDecomposition = !((aSubShapes.Extent() == 1) && (theShape == aSubShapes (1)));
  int anExtent = aSubShapes.Extent();
  for (Standard_Integer aShIndex = 1; aShIndex <= aSubShapes.Extent(); ++aShIndex)
  {
    const TopoDS_Shape& aSubShape = aSubShapes (aShIndex);
    int aValue = 0;
  }
}
#endif

void PartSet_ResultSketchPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer theMode)
{
  int aMode = theMode;

  if (aMode > 8 &&
      aMode != SketcherPrs_Tools::Sel_Sketch_Face &&
      aMode != SketcherPrs_Tools::Sel_Sketch_Wire)
    // In order to avoid using custom selection modes
    return;

  bool aShapeIsChanged = false;
  double aPrevDeviation = Attributes()->DeviationCoefficient();
  if (aMode == SketcherPrs_Tools::Sel_Sketch_Face ||
      aMode == SketcherPrs_Tools::Sel_Sketch_Wire) {
    aMode = (aMode == SketcherPrs_Tools::Sel_Sketch_Face) ? AIS_Shape::SelectionMode(TopAbs_FACE)
                                                          : AIS_Shape::SelectionMode(TopAbs_WIRE);
#ifdef DEBUG_WIRE
    const TopoDS_Shape& aShape = Shape();
    debugInfo(aShape, TopAbs_VERTEX); // 24
    debugInfo(aShape, TopAbs_EDGE); // 12
    debugInfo(aShape, TopAbs_WIRE); // 0
    debugInfo(aShape, TopAbs_FACE); // 0
#endif
    BRep_Builder aBuilder;
    TopoDS_Compound aComp;
    aBuilder.MakeCompound(aComp);
    aBuilder.Add(aComp, myOriginalShape);

    for(NCollection_List<TopoDS_Shape>::Iterator anIt(mySketchFaceList); anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aFace = anIt.Value();
      aBuilder.Add(aComp, aFace);
    }
#ifdef DEBUG_WIRE
    debugInfo(aComp, TopAbs_VERTEX); // 24
    debugInfo(aComp, TopAbs_EDGE); // 12
    debugInfo(aComp, TopAbs_WIRE); // 4
    debugInfo(aComp, TopAbs_FACE); // 2
#endif
    Set(aComp);
    double aBodyDefDeflection = Config_PropManager::real("Visualization", "body_deflection");
    Attributes()->SetDeviationCoefficient(aBodyDefDeflection);
    aShapeIsChanged = true;
  }
  else
    Set(myOriginalShape);

  // append auxiliary compound to selection of edges/vertices
  if (aMode == AIS_Shape::SelectionMode(TopAbs_EDGE) ||
      aMode == AIS_Shape::SelectionMode(TopAbs_VERTEX)) {

    bool isVertex = aMode == AIS_Shape::SelectionMode(TopAbs_VERTEX);
    appendShapeSelection(aSelection, myAuxiliaryCompound, isVertex ? TopAbs_VERTEX : TopAbs_EDGE);
  }

  AIS_Shape::ComputeSelection(aSelection, aMode);

  if (aShapeIsChanged) {
    Attributes()->SetDeviationCoefficient(aPrevDeviation);
    Set(myOriginalShape);
  }
}

void PartSet_ResultSketchPrs::appendShapeSelection(const Handle(SelectMgr_Selection)& theSelection,
                                                   const TopoDS_Shape& theShape,
                                                   const TopAbs_ShapeEnum& theTypeOfSelection)
{
  // POP protection against crash in low layers
  Standard_Real aDeflection = Prs3d::GetDeflection(theShape, myDrawer);
  try {
    StdSelect_BRepSelectionTool::Load(theSelection,
                                      this,
                                      theShape,
                                      theTypeOfSelection,
                                      aDeflection,
                                      myDrawer->HLRAngle(),
                                      myDrawer->IsAutoTriangulation());
  } catch ( Standard_Failure ) {
  }
}

void PartSet_ResultSketchPrs::setAuxiliaryPresentationStyle(const bool isAuxiliary)
{
  Standard_Integer aLineStyle = isAuxiliary?
    SketchPlugin_SketchEntity::SKETCH_LINE_STYLE_AUXILIARY() :
    SketchPlugin_SketchEntity::SKETCH_LINE_STYLE();

  Handle(Prs3d_Drawer) aDrawer = Attributes();

  // set line style
  Handle(Prs3d_LineAspect) aLineAspect;

  Aspect_TypeOfLine aType = (Aspect_TypeOfLine)aLineStyle;
  if (aDrawer->HasOwnLineAspect()) {
    aLineAspect = aDrawer->LineAspect();
  }
  if (aDrawer->HasOwnWireAspect()) {
    aLineAspect = aDrawer->WireAspect();
  }
  Quantity_Color aCurrentColor = aLineAspect->Aspect()->Color();
  Aspect_TypeOfLine aPrevLineType = aLineAspect->Aspect()->Type();
  Standard_Real aCurrentWidth = aLineAspect->Aspect()->Width();
  bool isChangedLineType = aType != aPrevLineType;
  if (isChangedLineType) {
    Handle(Prs3d_LineAspect) aAspect = new Prs3d_LineAspect(aCurrentColor, aType, aCurrentWidth);
    aDrawer->SetLineAspect(aAspect);
    aDrawer->SetWireAspect(aAspect);
    SetAttributes(aDrawer);
  }
}

void PartSet_ResultSketchPrs::fillShapes(TopoDS_Shape& theResultShape,
                                         TopoDS_Compound& theAuxiliaryCompound,
                                         NCollection_List<TopoDS_Shape>& theFaceList)
{
  //if (!aResultShape.IsNull() || !anAuxiliaryCompound.IsNull())
  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(myResult);
  if (!aShapePtr)
    return;

  theFaceList.Clear();
  ResultConstructionPtr aConstruction =
    std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(myResult);
  if (aConstruction.get()) {
    int aFacesNum = aConstruction->facesNum();
    for(int aFaceIndex = 0; aFaceIndex < aFacesNum; aFaceIndex++) {
      std::shared_ptr<GeomAPI_Face> aFaceShape = aConstruction->face(aFaceIndex);
      if (aFaceShape.get()) {
        TopoDS_Shape aFace = (aFaceShape)->impl<TopoDS_Shape>();
        theFaceList.Append(aFace);
      }
    }
  }
  theResultShape = aShapePtr->impl<TopoDS_Shape>();

  /// find auxiliary shapes
  FeaturePtr aResultFeature = ModelAPI_Feature::feature(myResult);
  CompositeFeaturePtr aSketchFeature = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>
                                                                          (aResultFeature);
  std::list<ResultPtr> anAuxiliaryResults;
  int aNumberOfSubs = aSketchFeature->numberOfSubs();
  for (int i = 0; i < aNumberOfSubs; i++) {
    FeaturePtr aFeature = aSketchFeature->subFeature(i);
    if (PartSet_Tools::isAuxiliarySketchEntity(aFeature)) {
      std::list<ResultPtr> aResults = aFeature->results();
      std::list<ResultPtr>::const_iterator aIt;
      for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
        ResultPtr aResult = *aIt;
        if (aResult.get() && aResult->shape().get())
          anAuxiliaryResults.push_back(aResult);
      }
    }
    else if (PartSet_Tools::isIncludeIntoSketchResult(aFeature)) {
      // Append not-edges shapes, e.g. center of a circle, an arc, a point feature.
      // Issue #2535: do not show elements produced by Projection or Intersection features,
      // which should not be included into the sketch result.
      const std::list<std::shared_ptr<ModelAPI_Result> >& aRes = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter = aRes.cbegin();
      for (; aResIter != aRes.cend(); aResIter++) {
        std::shared_ptr<ModelAPI_ResultConstruction> aConstr = std::dynamic_pointer_cast<
            ModelAPI_ResultConstruction>(*aResIter);
        if (aConstr) {
          std::shared_ptr<GeomAPI_Shape> aGeomShape = aConstr->shape();
          if (aGeomShape.get()) {
            const TopoDS_Shape& aShape = aGeomShape->impl<TopoDS_Shape>();
            if (aShape.ShapeType() != TopAbs_EDGE)
              anAuxiliaryResults.push_back(aConstr);
          }
        }
      }
    }
  }

  if (anAuxiliaryResults.size() > 0) {
    BRep_Builder aBuilder;
    //TopoDS_Compound aComp;
    aBuilder.MakeCompound(theAuxiliaryCompound);
    std::list<ResultPtr>::const_iterator anIt = anAuxiliaryResults.begin(),
                                         aLast = anAuxiliaryResults.end();
    for (; anIt != aLast; anIt++) {
      ResultPtr aResult = *anIt;
      if (aResult.get()) {
        GeomShapePtr aGeomShape = aResult->shape();
        if (aGeomShape.get()) {
          const TopoDS_Shape& aShape = aGeomShape->impl<TopoDS_Shape>();
          if (!aShape.IsNull())
            aBuilder.Add(theAuxiliaryCompound, aShape);
        }
      }
    }
  }
}
