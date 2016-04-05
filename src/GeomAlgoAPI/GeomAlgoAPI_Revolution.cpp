// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Revolution.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_Revolution.h"

#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepGProp.hxx>
#include <Geom_Plane.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

/// \brief Constructs infinite face from thePlane, and with axis located on the same side
/// of the plane as thePoint. Modifies thePlane axis direction.
/// \param[in,out] thePlane plane to construct face.
/// \param[in] thePoint point to locate plane axis.
/// \return constructed face.
static TopoDS_Face makeFaceFromPlane(gp_Pln& thePlane, const gp_Pnt& thePoint);

/// \return solid created from face or shell.
static TopoDS_Solid makeSolidFromShape(const TopoDS_Shape& theShape);

/// \brief Selects solid from theShape with closest center of mass to thePoint
/// \param[in] theShape compound with solids.
/// \param[in] thePoint point.
/// \return solid.
static TopoDS_Shape findClosest(const TopoDS_Shape& theShape, const gp_Pnt& thePoint);

//=================================================================================================
GeomAlgoAPI_Revolution::GeomAlgoAPI_Revolution(const GeomShapePtr                 theBaseShape,
                                               const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                               const double                       theToAngle,
                                               const double                       theFromAngle)
{
  build(theBaseShape, theAxis, GeomShapePtr(), theToAngle, GeomShapePtr(), theFromAngle);
}

//=================================================================================================
GeomAlgoAPI_Revolution::GeomAlgoAPI_Revolution(const GeomShapePtr                 theBaseShape,
                                               const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                               const GeomShapePtr                 theToShape,
                                               const double                       theToAngle,
                                               const GeomShapePtr                 theFromShape,
                                               const double                       theFromAngle)
{
  build(theBaseShape, theAxis, theToShape, theToAngle, theFromShape, theFromAngle);
}

//=================================================================================================
void GeomAlgoAPI_Revolution::build(const GeomShapePtr&                 theBaseShape,
                                   const std::shared_ptr<GeomAPI_Ax1>& theAxis,
                                   const GeomShapePtr&                 theToShape,
                                   const double                        theToAngle,
                                   const GeomShapePtr&                 theFromShape,
                                   const double                        theFromAngle)
{
  if(!theBaseShape || !theAxis ||
    (((!theFromShape && !theToShape) || (theFromShape && theToShape && theFromShape->isEqual(theToShape)))
    && (theFromAngle == -theToAngle))) {
    return;
  }

  // Geting base plane.
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();
  TopoDS_Face aBaseFace;
  if(theBaseShape->shapeType() == GeomAPI_Shape::FACE) {
    aBaseFace = TopoDS::Face(theBaseShape->impl<TopoDS_Shape>());
  } else if(theBaseShape->shapeType() == GeomAPI_Shape::SHELL) {
    GeomAPI_ShapeExplorer anExp(theBaseShape, GeomAPI_Shape::FACE);
    if(anExp.more()) {
      GeomShapePtr aFaceOnShell = anExp.current();
      aBaseFace = TopoDS::Face(aFaceOnShell->impl<TopoDS_Shape>());
    }
  }
  if(aBaseFace.IsNull()) {
    return;
  }
  GeomLib_IsPlanarSurface isBasePlanar(BRep_Tool::Surface(aBaseFace));
  gp_Pln aBasePln = isBasePlanar.Plan();
  Geom_Plane aBasePlane(aBasePln);
  gp_Ax1 anAxis = theAxis->impl<gp_Ax1>();
  if(aBasePlane.Axis().Angle(anAxis) < Precision::Confusion()) {
    return;
  }

  gp_Pnt aBaseCentre = GeomAlgoAPI_ShapeTools::centreOfMass(theBaseShape)->impl<gp_Pnt>();

  TopoDS_Shape aResult;
  if(!theFromShape && !theToShape) { // Case 1: When only angles was set.
    // Rotating base face with the negative value of "from angle".
    gp_Trsf aBaseTrsf;
    aBaseTrsf.SetRotation(anAxis, -theFromAngle / 180.0 * M_PI);
    BRepBuilderAPI_Transform* aBaseTransform = new BRepBuilderAPI_Transform(aBaseShape,
                                                                            aBaseTrsf,
                                                                            true);
    if(!aBaseTransform) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aBaseTransform)));
    if(!aBaseTransform->IsDone()) {
      return;
    }
    TopoDS_Shape aRotatedBase = aBaseTransform->Shape();

    // Making revolution to the angle equal to the sum of "from angle" and "to angle".
    BRepPrimAPI_MakeRevol* aRevolBuilder = new BRepPrimAPI_MakeRevol(aRotatedBase,
                                                                      anAxis,
                                                                      (theFromAngle + theToAngle) / 180 * M_PI,
                                                                      Standard_True);
    if(!aRevolBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aRevolBuilder)));
    if(!aRevolBuilder->IsDone()) {
      return;
    }
    aResult = aRevolBuilder->Shape();

    // Setting naming.
    for(TopExp_Explorer anExp(aRotatedBase, TopAbs_FACE); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aFace = anExp.Current();
      GeomShapePtr aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
      aFromShape->setImpl(new TopoDS_Shape(aRevolBuilder->FirstShape(aFace)));
      aToShape->setImpl(new TopoDS_Shape(aRevolBuilder->LastShape(aFace)));
      this->addFromShape(aFromShape);
      this->addToShape(aToShape);
    }
  } else if(theFromShape && theToShape) { // Case 2: When both bounding planes were set.
    // Making revolution to the 360 angle.
    BRepPrimAPI_MakeRevol* aRevolBuilder = new BRepPrimAPI_MakeRevol(aBaseShape, anAxis, 2 * M_PI, Standard_True);
    if(!aRevolBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aRevolBuilder)));
    if(!aRevolBuilder->IsDone()) {
      return;
    }
    aResult = aRevolBuilder->Shape();

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
    aFromFace = makeFaceFromPlane(aFromPln, aBaseCentre);
    aToFace   = makeFaceFromPlane(aToPln, aBaseCentre);

    // Making solids from bounding planes and putting them in compound.
    TopoDS_Shape aFromSolid = makeSolidFromShape(aFromFace);
    TopoDS_Shape aToSolid   = makeSolidFromShape(aToFace);

    // Rotating bounding planes to the specified angle.
    gp_Trsf aFromTrsf;
    gp_Trsf aToTrsf;
    double aFromRotAngle = ((aFromPln.Axis().Direction() * aBasePln.Axis().Direction()) > 0) ? -theFromAngle : theFromAngle;
    double aToRotAngle = ((aToPln.Axis().Direction() * aBasePln.Axis().Direction()) > 0) ? -theToAngle : theToAngle;
    aFromTrsf.SetRotation(anAxis,aFromRotAngle / 180.0 * M_PI);
    aToTrsf.SetRotation(anAxis, aToRotAngle / 180.0 * M_PI);
    BRepBuilderAPI_Transform aFromTransform(aFromSolid, aFromTrsf, true);
    BRepBuilderAPI_Transform aToTransform(aToSolid, aToTrsf, true);
    TopoDS_Shape aRotatedFromFace = aFromTransform.Modified(aFromFace).First();
    TopoDS_Shape aRotatedToFace = aToTransform.Modified(aToFace).First();
    aFromSolid = aFromTransform.Shape();
    aToSolid = aToTransform.Shape();

    // Cutting revolution with from plane.
    BRepAlgoAPI_Cut* aFromCutBuilder = new BRepAlgoAPI_Cut(aResult, aFromSolid);
    aFromCutBuilder->Build();
    if(!aFromCutBuilder->IsDone()) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aFromCutBuilder)));
    aResult = aFromCutBuilder->Shape();

    // Cutting revolution with to plane.
    BRepAlgoAPI_Cut* aToCutBuilder = new BRepAlgoAPI_Cut(aResult, aToSolid);
    aToCutBuilder->Build();
    if(!aToCutBuilder->IsDone()) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aToCutBuilder)));
    aResult = aToCutBuilder->Shape();

    TopExp_Explorer anExp(aResult, TopAbs_SOLID);
    if(!anExp.More()) {
      return;
    }
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    }
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      GeomShapePtr aCompound(new GeomAPI_Shape);
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

    // If after cut we got more than one solids then take closest to the center of mass of the base face.
    aResult = findClosest(aResult, aBaseCentre);

    // Setting naming.
    for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More (); anExp.Next ()) {
      const TopoDS_Shape& aFaceOnResult = anExp.Current();
      Handle(Geom_Surface) aFaceSurface = BRep_Tool::Surface(TopoDS::Face(aFaceOnResult));
      Handle(Geom_Surface) aFromSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedFromFace));
      Handle(Geom_Surface) aToSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedToFace));
      if(aFaceSurface == aFromSurface) {
        GeomShapePtr aFSHape(new GeomAPI_Shape);
        aFSHape->setImpl(new TopoDS_Shape(aFaceOnResult));
        this->addFromShape(aFSHape);
      }
      if(aFaceSurface == aToSurface) {
        GeomShapePtr aTSHape(new GeomAPI_Shape);
        aTSHape->setImpl(new TopoDS_Shape(aFaceOnResult));
        this->addToShape(aTSHape);
      }
    }
  } else { //Case 3: When only one bounding plane was set.
    // Making revolution to the 360 angle.
    BRepPrimAPI_MakeRevol* aRevolBuilder = new BRepPrimAPI_MakeRevol(aBaseShape, anAxis, 2 * M_PI, Standard_True);
    if(!aRevolBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aRevolBuilder)));
    if(!aRevolBuilder->IsDone()) {
      return;
    }
    aResult = aRevolBuilder->Shape();

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
    aBoundingFace = makeFaceFromPlane(aBoundingPln, aBaseCentre);

    // Making solid from bounding plane.
    TopoDS_Shape aBoundingSolid = makeSolidFromShape(aBoundingFace);

    // Rotating bounding plane to the specified angle.
    double aBoundingRotAngle = isFromFaceSet ? theFromAngle : theToAngle;
    if(aBoundingPln.Axis().IsParallel(aBasePln.Axis(), Precision::Confusion())) {
      if(isFromFaceSet) aBoundingRotAngle = -aBoundingRotAngle;
    } else {
      double aSign = (aBoundingPln.Axis().Direction() ^ aBasePln.Axis().Direction()) *
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

    // Cutting revolution with bounding plane.
    BRepAlgoAPI_Cut* aBoundingCutBuilder = new BRepAlgoAPI_Cut(aResult, aBoundingSolid);
    aBoundingCutBuilder->Build();
    if(!aBoundingCutBuilder->IsDone()) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aBoundingCutBuilder)));
    aResult = aBoundingCutBuilder->Shape();

    // Setting naming.
    const TopTools_ListOfShape& aBndShapes = aBoundingCutBuilder->Modified(aBoundingFace);
    for(TopTools_ListIteratorOfListOfShape anIt(aBndShapes); anIt.More(); anIt.Next()) {
      GeomShapePtr aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      isFromFaceSet ? this->addFromShape(aShape) : this->addToShape(aShape);
    }

    // Try to cut with base face. If it can not be done then keep result of cut with bounding plane.
    TopoDS_Shape aModifiedBaseShape = aBaseShape;
    if(isFromFaceSet) {
      if(aModifiedBaseShape.ShapeType() == TopAbs_FACE) {
        aModifiedBaseShape.Orientation(TopAbs_REVERSED);
      } else {
        gp_Trsf aMirrorTrsf;
        aMirrorTrsf.SetMirror(aBasePlane.Position().Ax2());
        BRepBuilderAPI_Transform aMirrorTransform(aModifiedBaseShape, aMirrorTrsf, true);
        aModifiedBaseShape = aMirrorTransform.Shape();
      }
    }

    // Making solid from base face.
    TopoDS_Shape aBaseSolid = makeSolidFromShape(aModifiedBaseShape);

    // Rotating base face to the specified angle.
    gp_Trsf aBaseTrsf;
    double aBaseRotAngle = isFromFaceSet ? theToAngle : -theFromAngle;
    aBaseTrsf.SetRotation(anAxis, aBaseRotAngle / 180.0 * M_PI);
    BRepBuilderAPI_Transform aBaseTransform(aBaseSolid, aBaseTrsf, true);
    aBaseSolid = aBaseTransform.Shape();

    // Cutting revolution with base.
    BRepAlgoAPI_Cut* aBaseCutBuilder = new BRepAlgoAPI_Cut(aResult, aBaseSolid);
    aBaseCutBuilder->Build();
    if(aBaseCutBuilder->IsDone()) {
      TopoDS_Shape aCutResult = aBaseCutBuilder->Shape();
      TopExp_Explorer anExp(aCutResult, TopAbs_SOLID);
      if(anExp.More()) {
        this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aBaseCutBuilder)));
        aResult = aCutResult;
      }
    }

    const TopTools_ListOfShape& aBsShapes = aBaseCutBuilder->Modified(aBoundingFace);
    for(TopTools_ListIteratorOfListOfShape anIt(aBsShapes); anIt.More(); anIt.Next()) {
      GeomShapePtr aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      isFromFaceSet ? this->addToShape(aShape) : this->addFromShape(aShape);
    }

    TopExp_Explorer anExp(aResult, TopAbs_SOLID);
    if(!anExp.More()) {
      return;
    }
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    }
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      GeomShapePtr aCompound(new GeomAPI_Shape);
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

    // If after cut we got more than one solids then take closest to the center of mass of the base face.
    aResult = findClosest(aResult, aBaseCentre);

    // Setting naming.
    for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More (); anExp.Next ()) {
      const TopoDS_Shape& aFaceOnResult = anExp.Current();
      Handle(Geom_Surface) aFaceSurface = BRep_Tool::Surface(TopoDS::Face(aFaceOnResult));
      Handle(Geom_Surface) aBoundingSurface = BRep_Tool::Surface(TopoDS::Face(aRotatedBoundingFace));
      if(aFaceSurface == aBoundingSurface) {
        GeomShapePtr aShape(new GeomAPI_Shape());
        aShape->setImpl(new TopoDS_Shape(aFaceOnResult));
        isFromFaceSet ? this->addFromShape(aShape) : this->addToShape(aShape);
      }
    }
  }

  // Setting result.
  if(aResult.IsNull()) {
    return;
  }
  GeomShapePtr aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}

//=================================================================================================
TopoDS_Face makeFaceFromPlane(gp_Pln& thePlane, const gp_Pnt& thePoint)
{
  if(!thePlane.Contains(thePoint, Precision::Confusion())) {
    gp_XYZ aVec = thePoint.XYZ() - thePlane.Location().XYZ();
    double aSign = aVec * thePlane.Axis().Direction().XYZ();
    if(aSign < 0) thePlane.SetAxis(thePlane.Axis().Reversed());
  }

  BRepBuilderAPI_MakeFace aMakeFace(thePlane);
  TopoDS_Face aResultFace = TopoDS::Face(aMakeFace.Shape());

  return aResultFace;
}

//=================================================================================================
TopoDS_Solid makeSolidFromShape(const TopoDS_Shape& theShape)
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
TopoDS_Shape findClosest(const TopoDS_Shape& theShape, const gp_Pnt& thePoint)
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
