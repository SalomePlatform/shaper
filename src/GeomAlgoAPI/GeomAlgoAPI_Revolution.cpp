// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Revolution.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Revolution.h>

#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Rotation.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepTools.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=================================================================================================
GeomAlgoAPI_Revolution::GeomAlgoAPI_Revolution(std::shared_ptr<GeomAPI_Shape> theBasis,
                                               std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                               double                         theToAngle,
                                               double                         theFromAngle)
: myDone(false)
{
  build(theBasis, theAxis, std::shared_ptr<GeomAPI_Shape>(), theToAngle, std::shared_ptr<GeomAPI_Shape>(), theFromAngle);
}

//=================================================================================================
GeomAlgoAPI_Revolution::GeomAlgoAPI_Revolution(std::shared_ptr<GeomAPI_Shape> theBasis,
                                               std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                               std::shared_ptr<GeomAPI_Shape> theToShape,
                                               double                         theToAngle,
                                               std::shared_ptr<GeomAPI_Shape> theFromShape,
                                               double                         theFromAngle)
: myDone(false)
{
  build(theBasis, theAxis, theToShape, theToAngle, theFromShape, theFromAngle);
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
TopoDS_Solid GeomAlgoAPI_Revolution::makeSolidFromShape(const TopoDS_Shape& theShape)
{
  TopoDS_Shell aShell;
  TopoDS_Solid aSolid;

  BRep_Builder aBoundingBuilder;
  if(theShape.ShapeType() == TopAbs_SHELL) {
    aShell = TopoDS::Shell(theShape);
  } else {
    aBoundingBuilder.MakeShell(aShell);
    aBoundingBuilder.Add(aShell, theShape);
  }
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
                                   const std::shared_ptr<GeomAPI_Shape>& theToShape,
                                   double                                theToAngle,
                                   const std::shared_ptr<GeomAPI_Shape>& theFromShape,
                                   double                                theFromAngle)
{
  if(!theBasis || !theAxis ||
    (((!theFromShape && !theToShape) || (theFromShape && theToShape && theFromShape->isEqual(theToShape)))
    && (theFromAngle == -theToAngle))) {
    return;
  }

  // Checking that shell is planar.
  TopoDS_Shape aBasis = theBasis->impl<TopoDS_Shape>();
  // TODO: fix planar checking
  //TopExp_Explorer aBasisExp(aBasis, TopAbs_FACE);
  //for(; aBasisExp.More(); aBasisExp.Next()) {
  //  const TopoDS_Shape& aCurSh = aBasisExp.Current();
  //}

  // Geting base plane.
  std::shared_ptr<GeomAPI_Face> aBaseFace;
  if(theBasis->shapeType() == GeomAPI_Shape::FACE) {
    aBaseFace = std::shared_ptr<GeomAPI_Face>(new GeomAPI_Face(theBasis));
  } else if(theBasis->shapeType() == GeomAPI_Shape::SHELL) {
    GeomAPI_ShapeExplorer anExp(theBasis, GeomAPI_Shape::FACE);
    if(anExp.more()) {
      std::shared_ptr<GeomAPI_Shape> aFaceOnShell = anExp.current();
      aBaseFace = std::shared_ptr<GeomAPI_Face>(new GeomAPI_Face(aFaceOnShell));
    }
  }
  if(!aBaseFace.get()) {
    return;
  }
  TopoDS_Face aBasisFace = TopoDS::Face(aBaseFace->impl<TopoDS_Shape>());
  GeomLib_IsPlanarSurface isBasisPlanar(BRep_Tool::Surface(aBasisFace));
  gp_Pln aBasisPln = isBasisPlanar.Plan();
  Geom_Plane aBasisPlane(aBasisPln);
  gp_Ax1 anAxis = theAxis->impl<gp_Ax1>();
  if(aBasisPlane.Axis().Angle(anAxis) < Precision::Confusion()) {
    return;
  }
  gp_Lin anAxisLin(anAxis);

  // Creating circle for pipe.
  gp_Pnt aBasisCentre = GeomAlgoAPI_ShapeTools::centreOfMass(theBasis)->impl<gp_Pnt>();
  gp_Pnt aStartPnt = aBasisCentre;
  const TopoDS_Shape& aBasisShape = theBasis->impl<TopoDS_Shape>();
  Handle(Geom_Line) anAxisLine = new Geom_Line(anAxis);
  if(anAxisLin.Contains(aStartPnt, Precision::Confusion())) {
    aStartPnt.Translate(anAxis.Direction() ^ aBasisPln.Axis().Direction());
  }
  GeomAPI_ProjectPointOnCurve aProjection(aStartPnt, anAxisLine);
  if(aProjection.NbPoints() != 1) {
    return;
  }
  Standard_Real aRadius = aProjection.Distance(1);
  gp_Circ aCircle(gp_Ax2(aProjection.NearestPoint(), anAxis.Direction()), aRadius);

  TopoDS_Shape aResult;
  ListOfMakeShape aListOfMakeShape;
  if(!theFromShape && !theToShape) { // Case 1: When only angles was set.
    // Rotating base face with the negative value of "from angle".
    gp_Trsf aBaseTrsf;
    aBaseTrsf.SetRotation(anAxis, -theFromAngle / 180.0 * M_PI);
    BRepBuilderAPI_Transform* aBaseTransform = new BRepBuilderAPI_Transform(aBasisShape,
                                                                            aBaseTrsf,
                                                                            true);
    if(!aBaseTransform || !aBaseTransform->IsDone()) {
      return;
    }
    aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aBaseTransform)));
    TopoDS_Shape aRotatedBaseShape = aBaseTransform->Shape();

    if(theBasis->shapeType() == GeomAPI_Shape::FACE) {
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
      aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aRevolBuilder)));
      aResult = aRevolBuilder->Shape();

      // Setting naming.
      std::shared_ptr<GeomAPI_Shape> aFSHape(new GeomAPI_Shape);
      aFSHape->setImpl(new TopoDS_Shape(aRevolBuilder->FirstShape()));
      myFromFaces.push_back(aFSHape);
      std::shared_ptr<GeomAPI_Shape> aTSHape(new GeomAPI_Shape);
      aTSHape->setImpl(new TopoDS_Shape(aRevolBuilder->LastShape()));
      myToFaces.push_back(aTSHape);
    } else {
      gp_Pnt aFromPnt = aStartPnt.Transformed(aBaseTrsf);
      aCircle = gp_Circ(gp_Ax2(aProjection.NearestPoint(), anAxis.Direction(), gp_Vec(aProjection.NearestPoint(), aFromPnt)),
                        aRadius);

      // Making wire for pipe.
      TopoDS_Edge aPipeEdge = BRepBuilderAPI_MakeEdge(aCircle, 0, (theFromAngle + theToAngle) / 180.0 * M_PI);
      TopoDS_Wire aPipeWire = BRepBuilderAPI_MakeWire(aPipeEdge).Wire();

      // Making pipe.
      BRepOffsetAPI_MakePipe* aPipeBuilder = new BRepOffsetAPI_MakePipe(aPipeWire, aRotatedBaseShape);
      if(!aPipeBuilder || !aPipeBuilder->IsDone()) {
        return;
      }
      std::shared_ptr<GeomAPI_Shape> aWire(new GeomAPI_Shape);
      std::shared_ptr<GeomAPI_Shape> aBShape(new GeomAPI_Shape);
      aWire->setImpl(new TopoDS_Shape(aPipeWire));
      aBShape->setImpl(new TopoDS_Shape(aRotatedBaseShape));
      aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aPipeBuilder, aWire, aBShape)));
      aResult = aPipeBuilder->Shape();
      TopoDS_Shape aToShape = aPipeBuilder->LastShape();
      TopoDS_Shape aFromShape = aPipeBuilder->FirstShape();

      // Setting naming.
      TopExp_Explorer anExp(aToShape, TopAbs_FACE);
      for(; anExp.More(); anExp.Next()) {
        std::shared_ptr<GeomAPI_Shape> aTSHape(new GeomAPI_Shape);
        aTSHape->setImpl(new TopoDS_Shape(anExp.Current()));
        myToFaces.push_back(aTSHape);
      }
      anExp.Init(aFromShape, TopAbs_FACE);
      for(; anExp.More(); anExp.Next()) {
        std::shared_ptr<GeomAPI_Shape> aFSHape(new GeomAPI_Shape);
        aFSHape->setImpl(new TopoDS_Shape(anExp.Current()));
        myFromFaces.push_back(aFSHape);
      }
    }
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
    aFromFace = makeFaceFromPlane(aFromPln, aBasisCentre);
    aToFace   = makeFaceFromPlane(aToPln, aBasisCentre);

    // Making solids from bounding planes and putting them in compound.
    TopoDS_Shape aFromSolid = makeSolidFromShape(aFromFace);
    TopoDS_Shape aToSolid   = makeSolidFromShape(aToFace);

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
    if(theBasis->shapeType() == GeomAPI_Shape::FACE) {
      BRepPrimAPI_MakeRevol* aRevolBuilder = new BRepPrimAPI_MakeRevol(aBasisShape, anAxis, 2 * M_PI, Standard_True);
      aRevolBuilder->Build();
      aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aRevolBuilder)));
      aResult = aRevolBuilder->Shape();
    } else {
      // Making wire for pipe.
      TopoDS_Edge aPipeEdge = BRepBuilderAPI_MakeEdge(aCircle, 0, 2 * M_PI);
      TopoDS_Wire aPipeWire = BRepBuilderAPI_MakeWire(aPipeEdge).Wire();

      // Making pipe.
      BRepOffsetAPI_MakePipe* aPipeBuilder = new BRepOffsetAPI_MakePipe(aPipeWire, aBasisShape);
      if(!aPipeBuilder || !aPipeBuilder->IsDone()) {
        return;
      }
      std::shared_ptr<GeomAPI_Shape> aWire(new GeomAPI_Shape);
      std::shared_ptr<GeomAPI_Shape> aBShape(new GeomAPI_Shape);
      aWire->setImpl(new TopoDS_Shape(aPipeWire));
      aBShape->setImpl(new TopoDS_Shape(aBasisShape));
      aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aPipeBuilder, aWire, aBShape)));
      aResult = aPipeBuilder->Shape();
    }

    // Cutting revolution with from plane.
    BRepAlgoAPI_Cut* aFromCutBuilder = new BRepAlgoAPI_Cut(aResult, aFromSolid);
    aFromCutBuilder->Build();
    if(!aFromCutBuilder->IsDone()) {
      return;
    }
    aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aFromCutBuilder)));
    aResult = aFromCutBuilder->Shape();

    // Cutting revolution with to plane.
    BRepAlgoAPI_Cut* aToCutBuilder = new BRepAlgoAPI_Cut(aResult, aToSolid);
    aToCutBuilder->Build();
    if(!aToCutBuilder->IsDone()) {
      return;
    }
    aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aToCutBuilder)));
    aResult = aToCutBuilder->Shape();

    // If after cut we got more than one solids then take closest to the center of mass of the base face.
    //aResult = findClosest(aResult, aBasisCentre);

    // Setting naming.
    for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More (); anExp.Next ()) {
      const TopoDS_Shape& aFaceOnResult = anExp.Current();
      Handle(Geom_Surface) aFaceSurface = BRep_Tool::Surface(TopoDS::Face(aFaceOnResult));
      Handle(Geom_Surface) aFromSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedFromFace));
      Handle(Geom_Surface) aToSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedToFace));
      if(aFaceSurface == aFromSurface) {
        std::shared_ptr<GeomAPI_Shape> aFSHape(new GeomAPI_Shape);
        aFSHape->setImpl(new TopoDS_Shape(aFaceOnResult));
        myFromFaces.push_back(aFSHape);
      }
      if(aFaceSurface == aToSurface) {
        std::shared_ptr<GeomAPI_Shape> aTSHape(new GeomAPI_Shape);
        aTSHape->setImpl(new TopoDS_Shape(aFaceOnResult));
        myToFaces.push_back(aTSHape);
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
    gp_Pnt aBasisCentr = GeomAlgoAPI_ShapeTools::centreOfMass(theBasis)->impl<gp_Pnt>();
    aBoundingFace = makeFaceFromPlane(aBoundingPln, aBasisCentr);

    // Making solid from bounding plane.
    TopoDS_Shape aBoundingSolid = makeSolidFromShape(aBoundingFace);

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
    if(theBasis->shapeType() == GeomAPI_Shape::FACE) {
      BRepPrimAPI_MakeRevol* aRevolBuilder = new BRepPrimAPI_MakeRevol(aBasisShape, anAxis, 2 * M_PI, Standard_True);
      aRevolBuilder->Build();
      aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aRevolBuilder)));
      aResult = aRevolBuilder->Shape();
    } else {
      // Making wire for pipe.
      TopoDS_Edge aPipeEdge = BRepBuilderAPI_MakeEdge(aCircle, 0, 2 * M_PI);
      TopoDS_Wire aPipeWire = BRepBuilderAPI_MakeWire(aPipeEdge).Wire();

      // Making pipe.
      BRepOffsetAPI_MakePipe* aPipeBuilder = new BRepOffsetAPI_MakePipe(aPipeWire, aBasisShape);
      if(!aPipeBuilder || !aPipeBuilder->IsDone()) {
        return;
      }
      std::shared_ptr<GeomAPI_Shape> aWire(new GeomAPI_Shape);
      std::shared_ptr<GeomAPI_Shape> aBShape(new GeomAPI_Shape);
      aWire->setImpl(new TopoDS_Shape(aPipeWire));
      aBShape->setImpl(new TopoDS_Shape(aBasisShape));
      aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aPipeBuilder, aWire, aBShape)));
      aResult = aPipeBuilder->Shape();
    }

    // Cutting revolution with bounding plane.
    BRepAlgoAPI_Cut* aBoundingCutBuilder = new BRepAlgoAPI_Cut(aResult, aBoundingSolid);
    aBoundingCutBuilder->Build();
    if(!aBoundingCutBuilder->IsDone()) {
      return;
    }
    aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aBoundingCutBuilder)));
    aResult = aBoundingCutBuilder->Shape();

    // Setting naming.
    const TopTools_ListOfShape& aBndShapes = aBoundingCutBuilder->Modified(aBoundingFace);
    for(TopTools_ListIteratorOfListOfShape anIt(aBndShapes); anIt.More(); anIt.Next()) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      isFromFaceSet ? myFromFaces.push_back(aShape) : myToFaces.push_back(aShape);
    }

    // Try to cut with base face. If it can not be done then keep result of cut with bounding plane.
    if(isFromFaceSet) {
      gp_Trsf aMirrorTrsf;
      aMirrorTrsf.SetMirror(aBasisPlane.Position().Ax2());
      BRepBuilderAPI_Transform aMirrorTransform(aBasis, aMirrorTrsf, true);
      aBasis = aMirrorTransform.Shape();
    }

    // Making solid from basis face.
    TopoDS_Shape aBasisSolid = makeSolidFromShape(aBasis);

    // Rotating basis face to the specified angle.
    gp_Trsf aBasisTrsf;
    double aBasisRotAngle = isFromFaceSet ? theToAngle : -theFromAngle;
    aBasisTrsf.SetRotation(anAxis, aBasisRotAngle / 180.0 * M_PI);
    BRepBuilderAPI_Transform aBasisTransform(aBasisSolid, aBasisTrsf, true);
    TopoDS_Shape aRotatedBasis = aBasisTransform.Modified(aBasis).First();
    aBasisSolid = aBasisTransform.Shape();

    // Cutting revolution with basis.
    BRepAlgoAPI_Cut* aBasisCutBuilder = new BRepAlgoAPI_Cut(aResult, aBasisSolid);
    aBasisCutBuilder->Build();
    if(aBasisCutBuilder->IsDone()) {
      TopoDS_Shape aCutResult = aBasisCutBuilder->Shape();
      TopExp_Explorer anExp(aCutResult, TopAbs_SOLID);
      if(anExp.More()) {
        aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aBasisCutBuilder)));
        aResult = aCutResult;
      }
    }

    const TopTools_ListOfShape& aBsShapes = aBasisCutBuilder->Modified(aBoundingFace);
    for(TopTools_ListIteratorOfListOfShape anIt(aBsShapes); anIt.More(); anIt.Next()) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      isFromFaceSet ? myToFaces.push_back(aShape) : myFromFaces.push_back(aShape);
    }

    // If after cut we got more than one solids then take closest to the center of mass of the base face.
    //aResult = findClosest(aResult, aBasisCentr);

    // Setting naming.
    for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More (); anExp.Next ()) {
      const TopoDS_Shape& aFaceOnResult = anExp.Current();
      Handle(Geom_Surface) aFaceSurface = BRep_Tool::Surface(TopoDS::Face(aFaceOnResult));
      Handle(Geom_Surface) aBoundingSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedBoundingFace));
      if(aFaceSurface == aBoundingSurface) {
        std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
        aShape->setImpl(new TopoDS_Shape(aFaceOnResult));
        isFromFaceSet ? myFromFaces.push_back(aShape) : myToFaces.push_back(aShape);
      }
    }
  }

  TopExp_Explorer anExp(aResult, TopAbs_SOLID);
  if(!anExp.More()) {
    return;
  }
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    std::shared_ptr<GeomAPI_Shape> aCompound(new GeomAPI_Shape);
    aCompound->setImpl(new TopoDS_Shape(aResult));
    ListOfShape aCompSolids, aFreeSolids;
    GeomAlgoAPI_ShapeTools::combineShapes(aCompound, GeomAPI_Shape::COMPSOLID, aCompSolids, aFreeSolids);
    if(aCompSolids.size() == 1 && aFreeSolids.size() == 0) {
      aResult = aCompSolids.front()->impl<TopoDS_Shape>();
    } else if (aCompSolids.size() > 1 || (aCompSolids.size() >= 1 && aFreeSolids.size() >= 1)) {
      TopoDS_Compound aResultComp;
      TopoDS_Builder aBuilder;
      aBuilder.MakeCompound(aResultComp);
      for(ListOfShape::const_iterator anIter = aCompSolids.cbegin(); anIter != aCompSolids.cend(); anIter++) {
        aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
      }
      for(ListOfShape::const_iterator anIter = aFreeSolids.cbegin(); anIter != aFreeSolids.cend(); anIter++) {
        aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
      }
      aResult = aResultComp;
    }
  }

  // fill data map to keep correct orientation of sub-shapes
  myMap = std::shared_ptr<GeomAPI_DataMapOfShapeShape>(new GeomAPI_DataMapOfShapeShape());
  for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
    std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
    aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
    myMap->bind(aCurrentShape, aCurrentShape);
  }
  myShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
  myShape->setImpl(new TopoDS_Shape(aResult));
  myMkShape = std::shared_ptr<GeomAlgoAPI_MakeShapeList>(new GeomAlgoAPI_MakeShapeList(aListOfMakeShape));
  myDone = true;
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
const ListOfShape& GeomAlgoAPI_Revolution::fromFaces() const
{
  return myFromFaces;
}

//=================================================================================================
const ListOfShape& GeomAlgoAPI_Revolution::toFaces() const
{
  return myToFaces;
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
