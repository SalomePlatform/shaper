// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Revolution.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Revolution.h>

#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Rotation.h>
#include <GeomAlgoAPI_ShapeProps.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepGProp.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//=================================================================================================
GeomAlgoAPI_Revolution::GeomAlgoAPI_Revolution(std::shared_ptr<GeomAPI_Shape> theBasis,
                                               std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                               double                         theFromAngle,
                                               double                         theToAngle)
: myDone(false)
{
  build(theBasis, theAxis, std::shared_ptr<GeomAPI_Shape>(), theFromAngle, std::shared_ptr<GeomAPI_Shape>(), theToAngle);
}

//=================================================================================================
GeomAlgoAPI_Revolution::GeomAlgoAPI_Revolution(std::shared_ptr<GeomAPI_Shape> theBasis,
                                               std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                               std::shared_ptr<GeomAPI_Shape> theFromShape,
                                               double                         theFromAngle,
                                               std::shared_ptr<GeomAPI_Shape> theToShape,
                                               double                         theToAngle)
: myDone(false)
{
  build(theBasis, theAxis, theFromShape, theFromAngle, theToShape, theToAngle);
}

//=================================================================================================
TopoDS_Face GeomAlgoAPI_Revolution::makeFaceFromPlane(gp_Pln& thePlane, const gp_Pnt& thePoint)
{
  gp_XYZ aVec = thePoint.XYZ() - thePlane.Location().XYZ();
  double aSign = aVec * thePlane.Axis().Direction().XYZ();
  if(aSign < 0) thePlane.SetAxis(thePlane.Axis().Reversed());

  BRepBuilderAPI_MakeFace aMakeFace(thePlane);
  TopoDS_Face aResultFace = TopoDS::Face(aMakeFace.Shape());

  return aResultFace;
}

//=================================================================================================
TopoDS_Solid GeomAlgoAPI_Revolution::makeSolidFromFace(const TopoDS_Face& theFace)
{
  TopoDS_Shell aShell;
  TopoDS_Solid aSolid;

  BRep_Builder aBoundingBuilder;
  aBoundingBuilder.MakeShell(aShell);
  aBoundingBuilder.Add(aShell, theFace);
  aBoundingBuilder.MakeSolid(aSolid);
  aBoundingBuilder.Add(aSolid, aShell);

  return aSolid;
}

//=================================================================================================
TopoDS_Shape GeomAlgoAPI_Revolution::findClosest(const TopoDS_Shape& theShape, const gp_Pnt& thePoint)
{
  TopoDS_Shape aResult = theShape;

  if(theShape.ShapeType() == TopAbs_COMPOUND) {
    double aMinDistance = Precision::Infinite();
    double aCurDistance;
    GProp_GProps aGProps;
    gp_Pnt aCentr;

    for (TopoDS_Iterator anItr(theShape); anItr.More(); anItr.Next()) {
      TopoDS_Shape aValue = anItr.Value();
      BRepGProp::VolumeProperties(aValue, aGProps);
      aCentr = aGProps.CentreOfMass();
      aCurDistance = aCentr.Distance(thePoint);

      if(aCurDistance < aMinDistance) {
        aMinDistance = aCurDistance;
        aResult = aValue;
      }
    }
  }

  return aResult;
}

//=================================================================================================
void GeomAlgoAPI_Revolution::build(const std::shared_ptr<GeomAPI_Shape>& theBasis,
                                   const std::shared_ptr<GeomAPI_Ax1>&   theAxis,
                                   const std::shared_ptr<GeomAPI_Shape>& theFromShape,
                                   double                                theFromAngle,
                                   const std::shared_ptr<GeomAPI_Shape>& theToShape,
                                   double                                theToAngle)
{
  if(!theBasis || !theAxis ||
    (((!theFromShape && !theToShape) || (theFromShape && theToShape && theFromShape->isEqual(theToShape)))
    && (theFromAngle == 0.0 && theToAngle == 0.0))) {
    return;
  }

  TopoDS_Face aBasisFace = TopoDS::Face(theBasis->impl<TopoDS_Shape>());
  GeomLib_IsPlanarSurface isBasisPlanar(BRep_Tool::Surface(aBasisFace));
  if(!isBasisPlanar.IsPlanar()) {// non-planar shapes is not supported for revolution
    return;
  }
  gp_Pln aBasisPln = isBasisPlanar.Plan();
  gp_Ax1 anAxis = theAxis->impl<gp_Ax1>();

  ListOfMakeShape aListOfMakeShape;
  myFirst = std::make_shared<GeomAPI_Shape>();
  myLast = std::make_shared<GeomAPI_Shape>();

  TopoDS_Shape aResult;
  if(!theFromShape && !theToShape) { // Case 1: When only angles was set.
    // Rotating base face with the negative value of "from angle".
    gp_Trsf aBaseTrsf;
    aBaseTrsf.SetRotation(anAxis, -theFromAngle / 180.0 * M_PI);
    BRepBuilderAPI_Transform* aBaseTransform = new BRepBuilderAPI_Transform(aBasisFace,
                                                                            aBaseTrsf,
                                                                            true);
    aListOfMakeShape.push_back(std::make_shared<GeomAlgoAPI_MakeShape>(aBaseTransform));
    TopoDS_Shape aRotatedBaseShape = aBaseTransform->Shape();

    // Making revolution to the angle equal to the sum of "from angle" and "to angle".
    double anAngle = theFromAngle + theToAngle;
    BRepPrimAPI_MakeRevol* aRevolBuilder = new BRepPrimAPI_MakeRevol(aRotatedBaseShape,
                                                                     anAxis,
                                                                     anAngle / 180 * M_PI,
                                                                     Standard_True);
    aRevolBuilder->Build();
    if(!aRevolBuilder->IsDone()) {
      return;
    }
    aListOfMakeShape.push_back(std::make_shared<GeomAlgoAPI_MakeShape>(aRevolBuilder));
    aResult = aRevolBuilder->Shape();

    // Setting naming.
    myFirst->setImpl(new TopoDS_Shape(aRevolBuilder->FirstShape()));
    myLast->setImpl(new TopoDS_Shape(aRevolBuilder->LastShape()));
  } else if(theFromShape && theToShape) { // Case 2: When both bounding planes were set.
    // Getting bounding faces.
    TopoDS_Face aFromFace = TopoDS::Face(theFromShape->impl<TopoDS_Shape>());
    TopoDS_Face aToFace   = TopoDS::Face(theToShape->impl<TopoDS_Shape>());

    // Getting planes from bounding face.
    GeomLib_IsPlanarSurface isFromPlanar(BRep_Tool::Surface(aFromFace));
    GeomLib_IsPlanarSurface isToPlanar(BRep_Tool::Surface(aToFace));
    if(!isFromPlanar.IsPlanar() || !isToPlanar.IsPlanar()) {// non-planar shapes is not supported for revolution bounding
      return;
    }
    gp_Pln aFromPln = isFromPlanar.Plan();
    gp_Pln aToPln   = isToPlanar.Plan();

    // Orienting bounding planes properly so that the center of mass of the base face stays
    // on the result shape after cut.
    gp_Pnt aBasisCentr = GeomAlgoAPI_ShapeProps::centreOfMass(theBasis)->impl<gp_Pnt>();
    aFromFace = makeFaceFromPlane(aFromPln, aBasisCentr);
    aToFace   = makeFaceFromPlane(aToPln, aBasisCentr);

    // Making solids from bounding planes and putting them in compound.
    TopoDS_Shape aFromSolid = makeSolidFromFace(aFromFace);
    TopoDS_Shape aToSolid   = makeSolidFromFace(aToFace);

    // Rotating bounding planes to the specified angle.
    gp_Trsf aFromTrsf;
    gp_Trsf aToTrsf;
    double aFromRotAngle = ((aFromPln.Axis().Direction() * aBasisPln.Axis().Direction()) > 0) ? -theFromAngle : theFromAngle;
    double aToRotAngle = ((aToPln.Axis().Direction() * aBasisPln.Axis().Direction()) > 0) ? -theToAngle : theToAngle;
    aFromTrsf.SetRotation(anAxis,aFromRotAngle / 180.0 * M_PI);
    aToTrsf.SetRotation(anAxis, aToRotAngle / 180.0 * M_PI);
    BRepBuilderAPI_Transform aFromTransform(aFromSolid, aFromTrsf, true);
    BRepBuilderAPI_Transform aToTransform(aToSolid, aToTrsf, true);
    TopoDS_Shape aRotatedFromFace = aFromTransform.Modified(aFromFace).First();
    TopoDS_Shape aRotatedToFace = aToTransform.Modified(aToFace).First();
    aFromSolid = aFromTransform.Shape();
    aToSolid = aToTransform.Shape();

    // Making revolution to the 360 angle.
    BRepPrimAPI_MakeRevol* aRevolBuilder = new BRepPrimAPI_MakeRevol(aBasisFace, anAxis, 2 * M_PI, Standard_True);
    aRevolBuilder->Build();
    aListOfMakeShape.push_back(std::make_shared<GeomAlgoAPI_MakeShape>(aRevolBuilder));
    TopoDS_Shape aRevolShape = aRevolBuilder->Shape();

    // Cutting revolution with from plane.
    BRepAlgoAPI_Cut* aFromCutBuilder = new BRepAlgoAPI_Cut(aRevolShape, aFromSolid);
    aFromCutBuilder->Build();
    if(!aFromCutBuilder->IsDone()) {
      return;
    }
    aListOfMakeShape.push_back(std::make_shared<GeomAlgoAPI_MakeShape>(aFromCutBuilder));
    aResult = aFromCutBuilder->Shape();

    // Cutting revolution with to plane.
    BRepAlgoAPI_Cut* aToCutBuilder = new BRepAlgoAPI_Cut(aResult, aToSolid);
    aToCutBuilder->Build();
    if(!aToCutBuilder->IsDone()) {
      return;
    }
    aListOfMakeShape.push_back(std::make_shared<GeomAlgoAPI_MakeShape>(aToCutBuilder));
    aResult = aToCutBuilder->Shape();

    // If after cut we got more than one solids then take closest to the center of mass of the base face.
    aResult = findClosest(aResult, aBasisCentr);

    // Setting naming.
    for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More (); anExp.Next ()) {
      const TopoDS_Shape& aFaceOnResult = anExp.Current();
      Handle(Geom_Surface) aFaceSurface = BRep_Tool::Surface(TopoDS::Face(aFaceOnResult));
      Handle(Geom_Surface) aFromSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedFromFace));
      Handle(Geom_Surface) aToSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedToFace));
      if(aFaceSurface == aFromSurface) {
        myFirst->setImpl(new TopoDS_Shape(aFaceOnResult));
      }
      if(aFaceSurface == aToSurface) {
        myLast->setImpl(new TopoDS_Shape(aFaceOnResult));
      }
    }
  } else { //Case 3: When only one bounding plane was set.
    // Getting bounding face.
    TopoDS_Face aBoundingFace;
    bool isFromFaceSet = false;
    if(theFromShape) {
      aBoundingFace = TopoDS::Face(theFromShape->impl<TopoDS_Shape>());
      isFromFaceSet = true;
    } else if(theToShape) {
      aBoundingFace = TopoDS::Face(theToShape->impl<TopoDS_Shape>());
    }

    // Getting plane from bounding face.
    GeomLib_IsPlanarSurface isBoundingPlanar(BRep_Tool::Surface(aBoundingFace));
    if(!isBoundingPlanar.IsPlanar()) { // non-planar shapes is not supported for revolution bounding
      return;
    }
    gp_Pln aBoundingPln = isBoundingPlanar.Plan();

    // Orienting bounding plane properly so that the center of mass of the base face stays
    // on the result shape after cut.
    gp_Pnt aBasisCentr = GeomAlgoAPI_ShapeProps::centreOfMass(theBasis)->impl<gp_Pnt>();
    aBoundingFace = makeFaceFromPlane(aBoundingPln, aBasisCentr);

    // Making solid from bounding plane.
    TopoDS_Shape aBoundingSolid = makeSolidFromFace(aBoundingFace);

    // Rotating bounding plane to the specified angle.
    double aBoundingRotAngle = isFromFaceSet ? theFromAngle : theToAngle;
    if(aBoundingPln.Axis().IsParallel(aBasisPln.Axis(), Precision::Confusion())) {
      if(isFromFaceSet) aBoundingRotAngle = -aBoundingRotAngle;
    } else {
      double aSign = (aBoundingPln.Axis().Direction() ^ aBasisPln.Axis().Direction()) *
                     anAxis.Direction();
      if((aSign <= 0 && !isFromFaceSet) || (aSign > 0 && isFromFaceSet)) {
        aBoundingRotAngle = -aBoundingRotAngle;
      }
    }
    gp_Trsf aBoundingTrsf;
    aBoundingTrsf.SetRotation(anAxis, aBoundingRotAngle / 180.0 * M_PI);
    BRepBuilderAPI_Transform aBoundingTransform(aBoundingSolid, aBoundingTrsf, true);
    TopoDS_Shape aRotatedBoundingFace = aBoundingTransform.Modified(aBoundingFace).First();
    aBoundingSolid = aBoundingTransform.Shape();

    // Making revolution to the 360 angle.
    BRepPrimAPI_MakeRevol* aRevolBuilder = new BRepPrimAPI_MakeRevol(aBasisFace, anAxis, 2 * M_PI, Standard_True);
    aRevolBuilder->Build();
    aListOfMakeShape.push_back(std::make_shared<GeomAlgoAPI_MakeShape>(aRevolBuilder));
    TopoDS_Shape aRevolShape = aRevolBuilder->Shape();

    // Cutting revolution with bounding plane.
    BRepAlgoAPI_Cut* aBoundingCutBuilder = new BRepAlgoAPI_Cut(aRevolShape, aBoundingSolid);
    aBoundingCutBuilder->Build();
    if(!aBoundingCutBuilder->IsDone()) {
      return;
    }
    aListOfMakeShape.push_back(std::make_shared<GeomAlgoAPI_MakeShape>(aBoundingCutBuilder));
    aResult = aBoundingCutBuilder->Shape();
    TopExp_Explorer anExp1(aResult, TopAbs_SOLID);

    // Setting naming.
    if(aBoundingCutBuilder->Modified(aBoundingFace).Extent() > 0) {
      std::shared_ptr<GeomAPI_Shape> aPtr = isFromFaceSet ? myFirst : myLast;
      aPtr->setImpl(new TopoDS_Shape(aBoundingCutBuilder->Modified(aBoundingFace).First()));
    }

    // Try to cut with base face. If it can not be done then keep result of cut with bounding plane.
    if(isFromFaceSet) {
      aBasisFace.Orientation(TopAbs_REVERSED);
    }

    // Making solid from basis face.
    TopoDS_Shape aBasisSolid = makeSolidFromFace(aBasisFace);

    // Rotating basis face to the specified angle.
    gp_Trsf aBasisTrsf;
    double aBasisRotAngle = isFromFaceSet ? theToAngle : -theFromAngle;
    aBasisTrsf.SetRotation(anAxis, aBasisRotAngle / 180.0 * M_PI);
    BRepBuilderAPI_Transform aBasisTransform(aBasisSolid, aBasisTrsf, true);
    TopoDS_Shape aRotatedBasisFace = aBasisTransform.Modified(aBasisFace).First();
    aBasisSolid = aBasisTransform.Shape();

    // Cutting revolution with basis face.
    BRepAlgoAPI_Cut* aBasisCutBuilder = new BRepAlgoAPI_Cut(aResult, aBasisSolid);
    aBasisCutBuilder->Build();
    if(aBasisCutBuilder->IsDone()) {
      TopoDS_Shape aCutResult = aBasisCutBuilder->Shape();
      TopExp_Explorer anExp(aCutResult, TopAbs_SOLID);
      if(anExp.More()) {
        aListOfMakeShape.push_back(std::make_shared<GeomAlgoAPI_MakeShape>(aBasisCutBuilder));
        aResult = aCutResult;
      }
    }

    // If after cut we got more than one solids then take closest to the center of mass of the base face.
    aResult = findClosest(aResult, aBasisCentr);

    // Setting naming.
    for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More (); anExp.Next ()) {
      const TopoDS_Shape& aFaceOnResult = anExp.Current();
      Handle(Geom_Surface) aFaceSurface = BRep_Tool::Surface(TopoDS::Face(aFaceOnResult));
      Handle(Geom_Surface) aBoundingSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedBoundingFace));
      Handle(Geom_Surface) aBasisSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedBasisFace));
      if(aFaceSurface == aBoundingSurface) {
        std::shared_ptr<GeomAPI_Shape> aPtr = isFromFaceSet ? myFirst : myLast;
        aPtr->setImpl(new TopoDS_Shape(aFaceOnResult));
      }
      if(aFaceSurface == aBasisSurface) {
        std::shared_ptr<GeomAPI_Shape> aPtr = isFromFaceSet ? myLast : myFirst;
        aPtr->setImpl(new TopoDS_Shape(aFaceOnResult));
      }
    }
  }

  TopExp_Explorer anExp(aResult, TopAbs_SOLID);
  if(!anExp.More()) {
    return;
  }

  // fill data map to keep correct orientation of sub-shapes
  myMap = std::make_shared<GeomAPI_DataMapOfShapeShape>();
  for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
    std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
    aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
    myMap->bind(aCurrentShape, aCurrentShape);
  }
  myShape = std::make_shared<GeomAPI_Shape>();
  myShape->setImpl(new TopoDS_Shape(aResult));
  myMkShape = std::make_shared<GeomAlgoAPI_MakeShapeList>();
  myDone = true;
  return;
}

//=================================================================================================
const bool GeomAlgoAPI_Revolution::isDone() const
{
  return myDone;
}

//=================================================================================================
const bool GeomAlgoAPI_Revolution::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
const bool GeomAlgoAPI_Revolution::hasVolume() const
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

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Revolution::shape () const
{
  return myShape;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Revolution::firstShape()
{
  return myFirst;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Revolution::lastShape()
{
  return myLast;
}

//=================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_Revolution::mapOfShapes() const
{
  return myMap;
}

//=================================================================================================
std::shared_ptr<GeomAlgoAPI_MakeShape> GeomAlgoAPI_Revolution::makeShape() const
{
  return myMkShape;
}
