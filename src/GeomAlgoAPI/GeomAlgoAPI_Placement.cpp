// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Placement.cpp
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#include <GeomAlgoAPI_Placement.h>
#include <GeomAlgoAPI_DFLoader.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pln.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Trsf.hxx>
#include <gp_Quaternion.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Precision.hxx>

#define DEB_PLACEMENT 1
GeomAlgoAPI_Placement::GeomAlgoAPI_Placement(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Shape> theDestShape,
    std::shared_ptr<GeomAPI_Face> theSourcePlane,
    std::shared_ptr<GeomAPI_Face> theDestPlane,
    bool theIsReverse,
    bool theIsCentering)
  : myDone(false),
    myShape(new GeomAPI_Shape())
{
  build(theSourceShape, theDestShape, theSourcePlane, theDestPlane, theIsReverse, theIsCentering);
}

void GeomAlgoAPI_Placement::build(
    const std::shared_ptr<GeomAPI_Shape>& theSourceShape,
    const std::shared_ptr<GeomAPI_Shape>& theDestShape,
    const std::shared_ptr<GeomAPI_Face>& theSourcePlane,
    const std::shared_ptr<GeomAPI_Face>& theDestPlane,
    bool theIsReverse,
    bool theIsCentering)
{
  std::shared_ptr<GeomAPI_Pln> aSourcePlane = theSourcePlane->getPlane();
  std::shared_ptr<GeomAPI_Pln> aDestPlane = theDestPlane->getPlane();
  std::shared_ptr<GeomAPI_Dir> aSourceDir = aSourcePlane->direction();
  std::shared_ptr<GeomAPI_Pnt> aSourceLoc = aSourcePlane->location();
  std::shared_ptr<GeomAPI_Dir> aDestDir = aDestPlane->direction();
  std::shared_ptr<GeomAPI_Pnt> aDestLoc = aDestPlane->location();

  // Initial shapes
  const TopoDS_Shape& aSourceShape = theSourceShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aDestShape = theDestShape->impl<TopoDS_Shape>();

  // Calculate transformation
  gp_Trsf aTrsf;
  gp_Vec aSrcDir(aSourceDir->x(), aSourceDir->y(), aSourceDir->z());
  gp_Vec aDstDir(aDestDir->x(), aDestDir->y(), aDestDir->z());
  // Check the material of the solids to be on the correct side
  BRepClass3d_SolidClassifier aClassifier;
  aClassifier.Load(aSourceShape);
  static const double aTransStep = 10. * Precision::Confusion();
  gp_Pnt aPoint(aSourceLoc->x(), aSourceLoc->y(), aSourceLoc->z());
  aPoint.Translate(aSrcDir * aTransStep);
  aClassifier.Perform(aPoint, Precision::Confusion());
  if ((aClassifier.State() == TopAbs_OUT && !theIsReverse) ||
      (aClassifier.State() == TopAbs_IN && theIsReverse))
    aSrcDir.Reverse();
  aClassifier.Load(aDestShape);
  aPoint.SetCoord(aDestLoc->x(), aDestLoc->y(), aDestLoc->z());
  aPoint.Translate(aDstDir * aTransStep);
  aClassifier.Perform(aPoint, Precision::Confusion());
  if (aClassifier.State() == TopAbs_IN)
    aDstDir.Reverse();
  // Calculate rotation
  gp_Quaternion aRot(aSrcDir, aDstDir);
  aTrsf.SetRotation(aRot);
  // Calculate translation
  gp_Vec aSrcLoc(aSourceLoc->x(), aSourceLoc->y(), aSourceLoc->z());
  gp_Vec aDstLoc(aDestLoc->x(), aDestLoc->y(), aDestLoc->z());
  if (!theIsCentering)
    aDstLoc = aSrcLoc + gp_Vec(aDstDir) * (aDstLoc-aSrcLoc).Dot(aDstDir);
  aSrcLoc.Transform(aTrsf);
  gp_Vec aTrans = aDstLoc - aSrcLoc;
  aTrsf.SetTransformation(aRot, aTrans);

  // Transform the shape with copying it
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, aTrsf, true);
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