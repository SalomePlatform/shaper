// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ResultPrs.cpp
// Created:     21 October 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_ResultPrs.h"

#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <GeomAPI_PlanarEdges.h>

#include <BRep_Builder.hxx>
#include <AIS_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <TopoDS_Builder.hxx>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_ResultPrs, AIS_Shape);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ResultPrs, AIS_Shape);

ModuleBase_ResultPrs::ModuleBase_ResultPrs(ResultPtr theResult)
  : AIS_Shape(TopoDS_Shape()), myResult(theResult), myIsSketchMode(false)
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
  Set(aShapePtr->impl<TopoDS_Shape>());
}


void ModuleBase_ResultPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(myResult);
  if (!aShapePtr)
    return;
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
    AIS_Shape::Compute(thePresentationManager, thePresentation, theMode);
  }
}


void ModuleBase_ResultPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
  if (aMode > TopAbs_SHAPE)
    // In order to avoid using custom selection modes
    return;

  if (myIsSketchMode) {
    if (aMode == TopAbs_FACE) {
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
    } else {
      Set(myOriginalShape);
    }
  }
  AIS_Shape::ComputeSelection(aSelection, aMode);
}