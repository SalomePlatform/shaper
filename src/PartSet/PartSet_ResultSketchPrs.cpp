// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_ResultSketchPrs.h
// Created:     25 March 2015
// Author:      Natalia ERMOLAEVA

#include "PartSet_ResultSketchPrs.h"
#include "PartSet_Tools.h"
#include "ModuleBase_Tools.h"

#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <GeomAPI_PlanarEdges.h>

#include <Events_Error.h>

#include <SketchPlugin_SketchEntity.h>

#include <Config_PropManager.h>

#include <BRep_Builder.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d.hxx>
#include <Prs3d_PointAspect.hxx>
#include <TopoDS_Builder.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_LineAspect.hxx>

#define DEBUG_WIRE

#ifdef DEBUG_WIRE
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#endif

//*******************************************************************************************

IMPLEMENT_STANDARD_HANDLE(PartSet_ResultSketchPrs, ViewerData_AISShape);
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

  // Activate individual repaintng if this is a part of compsolid
  ResultCompSolidPtr aCompSolid = ModelAPI_Tools::compSolidOwner(myResult);
  SetAutoHilight(aCompSolid.get() == NULL);
}

bool PartSet_ResultSketchPrs::isValidShapeType(const TopAbs_ShapeEnum& theBaseType,
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


void PartSet_ResultSketchPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(myResult);
  if (!aShapePtr) {
    Events_Error::throwException("An empty AIS presentation: PartSet_ResultSketchPrs");
    return;
  }

  myFacesList.clear();
  ResultConstructionPtr aConstruction = 
    std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(myResult);
  if (aConstruction.get()) {
    int aFacesNum = aConstruction->facesNum();
    for(int aFaceIndex = 0; aFaceIndex < aFacesNum; aFaceIndex++) {
      myFacesList.push_back(aConstruction->face(aFaceIndex));
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
    Events_Error::throwException("An empty AIS presentation: PartSet_ResultSketchPrs");


  // create auxiliary shapes
  FeaturePtr aResultFeature = ModelAPI_Feature::feature(myResult);
  CompositeFeaturePtr aSketchFeature = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>
                                                                          (aResultFeature);
  std::list<ResultPtr> anAuxiliaryResults;
  for (int i = 0; i < aSketchFeature->numberOfSubs(); i++) {
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
  }
  if (anAuxiliaryResults.size() > 0) {
    std::vector<int> aColor;
    aColor = Config_PropManager::color("Visualization", "sketch_auxiliary_color",
                                         SKETCH_AUXILIARY_COLOR);
    Standard_Real anAuxiliaryWidth = 1.;
    Standard_Integer anAuxiliaryLineStyle = 3;

    SetColor(Quantity_Color(aColor[0] / 255., aColor[1] / 255., aColor[2] / 255., Quantity_TOC_RGB));

    //thePresentation->Clear();
    Handle(Prs3d_Drawer) aDrawer = Attributes();
    setWidth(aDrawer, anAuxiliaryWidth);
    // set line style
    Handle(Prs3d_LineAspect) aLineAspect;

    Aspect_TypeOfLine aType = (Aspect_TypeOfLine)anAuxiliaryLineStyle;
    if (aDrawer->HasOwnLineAspect()) {
      aLineAspect = aDrawer->LineAspect();
    }
    if (aDrawer->HasOwnWireAspect()) {
      aLineAspect = aDrawer->WireAspect();
    }
    Quantity_Color aCurrentColor;
    Aspect_TypeOfLine aCurrentType;
    Standard_Real aCurrentWidth;
    aLineAspect->Aspect()->Values(aCurrentColor, aCurrentType, aCurrentWidth);
    bool isChanged = aType != aCurrentType;
    if (isChanged) {
      aLineAspect->SetTypeOfLine(aType);
    }
    // end of set line style

    BRep_Builder aBuilder;
    TopoDS_Compound aComp;
    aBuilder.MakeCompound(aComp);

    std::list<ResultPtr>::const_iterator anIt = anAuxiliaryResults.begin(),
                                         aLast = anAuxiliaryResults.end();
    for (; anIt != aLast; anIt++) {
      ResultPtr aResult = *anIt;
      if (aResult.get()) {
        GeomShapePtr aGeomShape = aResult->shape();
        if (aGeomShape.get()) {
          const TopoDS_Shape& aShape = aGeomShape->impl<TopoDS_Shape>();
          if (!aShape.IsNull())
            aBuilder.Add(aComp, aShape);
        }
      }
    }
    myAuxiliaryCompound = aComp;
    StdPrs_WFDeflectionShape::Add(thePresentation, aComp, aDrawer);
  }
}

#ifdef DEBUG_WIRE
void debugInfo(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  TopTools_IndexedMapOfShape aSubShapes;
  TopExp::MapShapes (theShape, theType, aSubShapes);

  Standard_Boolean isComesFromDecomposition = !((aSubShapes.Extent() == 1) && (theShape == aSubShapes (1)));
  int anExtent = aSubShapes.Extent();
  for (Standard_Integer aShIndex = 1; aShIndex <= aSubShapes.Extent(); ++aShIndex)
  {
    const TopoDS_Shape& aSubShape = aSubShapes (aShIndex);
    int aValue = 0;
  }
}
#endif

void PartSet_ResultSketchPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  if (aMode > 8)
    // In order to avoid using custom selection modes
    return;

  if (aMode == AIS_Shape::SelectionMode(TopAbs_FACE)) {
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
    std::list<std::shared_ptr<GeomAPI_Shape>>::const_iterator aIt;
    for (aIt = myFacesList.cbegin(); aIt != myFacesList.cend(); ++aIt) {
      TopoDS_Shape aFace = (*aIt)->impl<TopoDS_Shape>();
      aBuilder.Add(aComp, aFace);
      // for sketch presentation in the face mode wires should be selectable also
      // accoring to #1343 Improvement of Extrusion and Revolution operations
      appendShapeSelection(aSelection, aFace, TopAbs_WIRE);
    }
#ifdef DEBUG_WIRE
    debugInfo(aComp, TopAbs_VERTEX); // 24
    debugInfo(aComp, TopAbs_EDGE); // 12
    debugInfo(aComp, TopAbs_WIRE); // 4
    debugInfo(aComp, TopAbs_FACE); // 2
#endif
    Set(aComp);
  } else
    Set(myOriginalShape);

  // append auxiliary compound to selection of edges/vertices
  if (aMode == AIS_Shape::SelectionMode(TopAbs_EDGE) ||
      aMode == AIS_Shape::SelectionMode(TopAbs_VERTEX)) {

        bool isVertex = aMode == AIS_Shape::SelectionMode(TopAbs_VERTEX);
    appendShapeSelection(aSelection, myAuxiliaryCompound, isVertex ? TopAbs_VERTEX : TopAbs_EDGE);
  }

  AIS_Shape::ComputeSelection(aSelection, aMode);
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
