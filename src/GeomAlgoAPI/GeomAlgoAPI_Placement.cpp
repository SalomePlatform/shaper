// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Placement.cpp
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#include <GeomAlgoAPI_Placement.h>
#include <GeomAlgoAPI_DFLoader.h>

#include <GeomAPI_Pnt.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Trsf.hxx>
#include <gp_Quaternion.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Precision.hxx>
#define DEB_PLACEMENT 1
GeomAlgoAPI_Placement::GeomAlgoAPI_Placement(
    std::shared_ptr<GeomAPI_Shape> theAttractiveFace,
    std::shared_ptr<GeomAPI_Pln> theSourcePlane,
    std::shared_ptr<GeomAPI_Pln> theDestPlane)
  : myDone(false),
    myShape(new GeomAPI_Shape())
{
  build(theAttractiveFace, theSourcePlane, theDestPlane);
}

void GeomAlgoAPI_Placement::build(
    const std::shared_ptr<GeomAPI_Shape>& theAttractiveShape,
    const std::shared_ptr<GeomAPI_Pln>& theSourcePlane,
    const std::shared_ptr<GeomAPI_Pln>& theDestPlane)
{
  std::shared_ptr<GeomAPI_Dir> aSourceDir = theSourcePlane->direction();
  std::shared_ptr<GeomAPI_Pnt> aSourceLoc = theSourcePlane->location();
  std::shared_ptr<GeomAPI_Dir> aDestDir = theDestPlane->direction();
  std::shared_ptr<GeomAPI_Pnt> aDestLoc = theDestPlane->location();

  // Calculate transformation
  gp_Trsf aTrsf;
  gp_Vec aSrcDir(aSourceDir->x(), aSourceDir->y(), aSourceDir->z());
  gp_Vec aDstDir(aDestDir->x(), aDestDir->y(), aDestDir->z());
  gp_Quaternion aRot(aSrcDir, aDstDir);
  aTrsf.SetRotation(aRot);
  gp_Vec aSrcCenter(aSourceLoc->x(), aSourceLoc->y(), aSourceLoc->z());
  aSrcCenter.Transform(aTrsf);
  gp_Vec aTrans(aDestLoc->x() - aSrcCenter.X(),
                aDestLoc->y() - aSrcCenter.Y(),
                aDestLoc->z() - aSrcCenter.Z());
  aTrsf.SetTransformation(aRot, aTrans);

  // Transform the shape with copying it
  const TopoDS_Shape& aShape = theAttractiveShape->impl<TopoDS_Shape>();
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aShape, aTrsf, true);
  if(aBuilder) {
    setImpl(aBuilder);
    myDone = aBuilder->IsDone() == Standard_True;
    if (myDone) {
      TopoDS_Shape aResult = aBuilder->Shape();
      // fill data map to keep correct orientation of sub-shapes 
      for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
        std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
        aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
        myMap.bind(aCurrentShape, aCurrentShape);
      }
#ifdef DEB_PLACEMENT
	  int aNum = myMap.size();
	  cout << "MAP of Oriented shapes =" << aNum <<endl;

#endif

      myShape->setImpl(new TopoDS_Shape(aResult));
      myMkShape = new GeomAlgoAPI_MakeShape (aBuilder);
    }
  }
}

//============================================================================
const bool GeomAlgoAPI_Placement::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//============================================================================
const bool GeomAlgoAPI_Placement::hasVolume() const
{
  bool hasVolume(false);
  if(isValid()) {
    const TopoDS_Shape& aRShape = myShape->impl<TopoDS_Shape>();
    GProp_GProps aGProp;
    BRepGProp::VolumeProperties(aRShape, aGProp);
    if(aGProp.Mass() > Precision::Confusion()) 
      hasVolume = true;	
  }
  return hasVolume;
}

//============================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Placement::shape () const 
{
  return myShape;
}

//============================================================================
void GeomAlgoAPI_Placement::mapOfShapes (GeomAPI_DataMapOfShapeShape& theMap) const
{
  theMap = myMap;
}

//============================================================================
GeomAlgoAPI_MakeShape * GeomAlgoAPI_Placement::makeShape() const
{
  return myMkShape;
}

//============================================================================
GeomAlgoAPI_Placement::~GeomAlgoAPI_Placement()
{
  if (myImpl)
    myMap.clear();
}