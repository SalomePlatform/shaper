// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_OperationPrs.cpp
// Created:     01 Jul 2015
// Author:      Natalia ERMOLAEVA

#include "PartSet_OperationPrs.h"

//#include <ModelAPI_Tools.h>
//#include <ModelAPI_ResultConstruction.h>
//#include <GeomAPI_PlanarEdges.h>

//#include <BRep_Builder.hxx>
//#include <Prs3d_IsoAspect.hxx>
//#include <TopoDS_Builder.hxx>

IMPLEMENT_STANDARD_HANDLE(PartSet_OperationPrs, ViewerData_AISShape);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_OperationPrs, ViewerData_AISShape);

PartSet_OperationPrs::PartSet_OperationPrs()
  : ViewerData_AISShape(TopoDS_Shape()), myFeature(FeaturePtr())
{
}

void PartSet_OperationPrs::setFeature(FeaturePtr theFeature)
{
/*  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(theResult);
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
*/
}


/*#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
*/
void PartSet_OperationPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
/*  std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(myResult);
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
    /*
    TopExp_Explorer anExp(myOriginalShape, TopAbs_VERTEX);
    Handle(Prs3d_Drawer) aDrawer = Attributes();
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Vertex& aVertex = (const TopoDS_Vertex&)anExp.Current();
      StdPrs_WFDeflectionShape::Add(thePresentation, aVertex, aDrawer);
    }*|/
  }*/
}


void PartSet_OperationPrs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                            const Standard_Integer aMode)
{
/*  if (aMode > TopAbs_SHAPE)
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
  }*/
  AIS_Shape::ComputeSelection(aSelection, aMode);
}