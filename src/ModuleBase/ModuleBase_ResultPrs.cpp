// File:        ModuleBase_ResultPrs.cpp
// Created:     21 October 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_ResultPrs.h"

#include <ModelAPI_Tools.h>
#include <GeomAPI_Wire.h>
#include <GeomAlgoAPI_SketchBuilder.h>

#include <BRep_Builder.hxx>
#include <AIS_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_ResultPrs, AIS_Shape);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ResultPrs, AIS_Shape);

ModuleBase_ResultPrs::ModuleBase_ResultPrs(ResultPtr theResult)
  : AIS_Shape(TopoDS_Shape()), myResult(theResult), myIsSketchMode(false)
{
  boost::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(theResult);
  boost::shared_ptr<GeomAPI_Wire> aWirePtr = 
    boost::dynamic_pointer_cast<GeomAPI_Wire>(aShapePtr);
  if (aWirePtr) {
    if (aWirePtr->hasPlane() ) {
      // If this is a wire with plane defined thin it is a sketch-like object
      // It must have invisible faces
      std::list<boost::shared_ptr<GeomAPI_Shape> > aFaces;
      GeomAlgoAPI_SketchBuilder::createFaces(aWirePtr->origin(), aWirePtr->dirX(),
        aWirePtr->dirY(), aWirePtr->norm(), aWirePtr, aFaces);

      BRep_Builder aBuilder;
      TopoDS_Shape aShape = aWirePtr->impl<TopoDS_Shape>();
      std::list<boost::shared_ptr<GeomAPI_Shape>>::const_iterator aIt;
      for (aIt = aFaces.cbegin(); aIt != aFaces.cend(); ++aIt) {
        TopoDS_Shape aFace = (*aIt)->impl<TopoDS_Shape>();
        aBuilder.Add(aShape, aFace);
      }
      Set(aShape);
      myIsSketchMode = true;
      // Define number of ISO lines
      //Handle(AIS_Drawer) aDrawer = Attributes();
      //Attributes()->SetIsoOnPlane(Standard_False);
      //SetAttributes(aDrawer);
    } else {
      Set(aWirePtr->impl<TopoDS_Shape>());
    }
  } else {
    Set(aShapePtr->impl<TopoDS_Shape>());
  }
}


Standard_Boolean ModuleBase_ResultPrs::AcceptDisplayMode(const Standard_Integer theMode) const
{
  if (myIsSketchMode) {
    return theMode == 0;
  }
  return AIS_Shape::AcceptDisplayMode(theMode);
}

void ModuleBase_ResultPrs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                   const Handle(Prs3d_Presentation)& thePresentation, 
                                   const Standard_Integer theMode)
{
  if (myIsSketchMode) {
    Handle(AIS_Drawer) aDrawer = Attributes();
    aDrawer->SetIsoOnPlane(Standard_False);
    aDrawer->UIsoAspect()->SetNumber(0);
    aDrawer->VIsoAspect()->SetNumber(0);
    SetAttributes(aDrawer);
  }
  AIS_Shape::Compute(thePresentationManager, thePresentation, theMode);
}
