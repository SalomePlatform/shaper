// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Prism.cpp
// Created:     5 May 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_Prism.h"

#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_XYZ.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <BRepTools.hxx>

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(const GeomShapePtr theBaseShape,
                                     const double       theToSize,
                                     const double       theFromSize)
{
  build(theBaseShape, std::shared_ptr<GeomAPI_Dir>(), GeomShapePtr(), theToSize, GeomShapePtr(), theFromSize);
}

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(const GeomShapePtr                 theBaseShape,
                                     const std::shared_ptr<GeomAPI_Dir> theDirection,
                                     const double                       theToSize,
                                     const double                       theFromSize)
{
  build(theBaseShape, theDirection, GeomShapePtr(), theToSize, GeomShapePtr(), theFromSize);
}

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(const GeomShapePtr theBaseShape,
                                     const GeomShapePtr theToShape,
                                     const double       theToSize,
                                     const GeomShapePtr theFromShape,
                                     const double       theFromSize)
{
  build(theBaseShape, std::shared_ptr<GeomAPI_Dir>(), theToShape, theToSize, theFromShape, theFromSize);
}

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(const GeomShapePtr                 theBaseShape,
                                     const std::shared_ptr<GeomAPI_Dir> theDirection,
                                     const GeomShapePtr                 theToShape,
                                     const double                       theToSize,
                                     const GeomShapePtr                 theFromShape,
                                     const double                       theFromSize)
{
  build(theBaseShape, theDirection, theToShape, theToSize, theFromShape, theFromSize);
}

//=================================================================================================
void GeomAlgoAPI_Prism::build(const GeomShapePtr&                theBaseShape,
                              const std::shared_ptr<GeomAPI_Dir> theDirection,
                              const GeomShapePtr&                theToShape,
                              const double                       theToSize,
                              const GeomShapePtr&                theFromShape,
                              const double                       theFromSize)
{
  if(!theBaseShape.get() ||
    (((!theFromShape.get() && !theToShape.get()) || (theFromShape.get() && theToShape.get() && theFromShape->isEqual(theToShape)))
    && (theFromSize == -theToSize))) {
    return;
  }

  // Getting base shape.
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();
  TopAbs_ShapeEnum aShapeTypeToExp;
  switch(aBaseShape.ShapeType()) {
    case TopAbs_VERTEX:
      aShapeTypeToExp = TopAbs_VERTEX;
      break;
    case TopAbs_EDGE:
    case TopAbs_WIRE:
      aShapeTypeToExp = TopAbs_EDGE;
      break;
    case TopAbs_FACE:
    case TopAbs_SHELL:
      aShapeTypeToExp = TopAbs_FACE;
      break;
    default:
      return;
  }

  // Getting direction.
  gp_Vec aDirVec;
  std::shared_ptr<GeomAPI_Pnt> aBaseLoc;
  std::shared_ptr<GeomAPI_Dir> aBaseDir;
  GeomShapePtr aBasePlane;
  const bool isBoundingShapesSet = theFromShape.get() || theToShape.get();
  BRepBuilderAPI_FindPlane aFindPlane(aBaseShape);
  if(aBaseShape.ShapeType() == TopAbs_VERTEX || aBaseShape.ShapeType() == TopAbs_EDGE ||
     aFindPlane.Found() != Standard_True) {
    // Direction and both bounding planes should be set or empty.
    if(!theDirection.get() || (isBoundingShapesSet && (!theToShape.get() || !theFromShape.get()))) {
      return;
    }

    aBaseDir = theDirection;
    aDirVec = theDirection->impl<gp_Dir>();
  } else {
    Handle(Geom_Plane) aPlane = aFindPlane.Plane();
    gp_Pnt aLoc = aPlane->Axis().Location();
    aDirVec = aPlane->Axis().Direction();

    aBaseLoc.reset(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
    aBaseDir.reset(new GeomAPI_Dir(aDirVec.X(), aDirVec.Y(), aDirVec.Z()));
    aBasePlane = GeomAlgoAPI_FaceBuilder::planarFace(aBaseLoc, aBaseDir);
  }

  TopoDS_Shape aResult;
  if(!isBoundingShapesSet) {
    // Moving base shape.
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(aDirVec * -theFromSize);
    BRepBuilderAPI_Transform* aTransformBuilder = new BRepBuilderAPI_Transform(aBaseShape, aTrsf);
    if(!aTransformBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aTransformBuilder)));
    if(!aTransformBuilder->IsDone()) {
      return;
    }
    TopoDS_Shape aMovedBase = aTransformBuilder->Shape();

    // Making prism.
    BRepPrimAPI_MakePrism* aPrismBuilder = new BRepPrimAPI_MakePrism(aMovedBase, aDirVec * (theFromSize + theToSize));
    if(!aPrismBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aPrismBuilder)));
    if(!aPrismBuilder->IsDone()) {
      return;
    }
    aResult = aPrismBuilder->Shape();

    // Setting naming.
    for(TopExp_Explorer anExp(aMovedBase, aShapeTypeToExp); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aFace = anExp.Current();
      GeomShapePtr aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
      aFromShape->setImpl(new TopoDS_Shape(aPrismBuilder->FirstShape(aFace)));
      aToShape->setImpl(new TopoDS_Shape(aPrismBuilder->LastShape(aFace)));
      this->addFromShape(aFromShape);
      this->addToShape(aToShape);
    }
  } else {
    GeomShapePtr aBoundingFromShape = theFromShape ? theFromShape : aBasePlane;
    GeomShapePtr aBoundingToShape   = theToShape   ? theToShape   : aBasePlane;

    // Moving prism bounding faces according to "from" and "to" sizes.
    std::shared_ptr<GeomAPI_Face> aFromFace(new GeomAPI_Face(aBoundingFromShape));
    std::shared_ptr<GeomAPI_Pln>  aFromPln = aFromFace->getPlane();
    std::shared_ptr<GeomAPI_Pnt>  aFromLoc = aFromPln->location();
    std::shared_ptr<GeomAPI_Dir>  aFromDir = aFromPln->direction();

    std::shared_ptr<GeomAPI_Face> aToFace(new GeomAPI_Face(aBoundingToShape));
    std::shared_ptr<GeomAPI_Pln>  aToPln = aToFace->getPlane();
    std::shared_ptr<GeomAPI_Pnt>  aToLoc = aToPln->location();
    std::shared_ptr<GeomAPI_Dir>  aToDir = aToPln->direction();

    bool aSign = aFromLoc->xyz()->dot(aBaseDir->xyz()) > aToLoc->xyz()->dot(aBaseDir->xyz());

    std::shared_ptr<GeomAPI_Pnt> aFromPnt(new GeomAPI_Pnt(aFromLoc->xyz()->added(aBaseDir->xyz()->multiplied(
                                                          aSign ? theFromSize : -theFromSize))));
    aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);

    std::shared_ptr<GeomAPI_Pnt> aToPnt(new GeomAPI_Pnt(aToLoc->xyz()->added(aBaseDir->xyz()->multiplied(
                                                        aSign ? -theToSize : theToSize))));
    aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);

    // Getting bounding box for base shape.
    Bnd_Box aBndBox;
    BRepBndLib::Add(aBaseShape, aBndBox);
    Standard_Real aXArr[2] = {aBndBox.CornerMin().X(), aBndBox.CornerMax().X()};
    Standard_Real aYArr[2] = {aBndBox.CornerMin().Y(), aBndBox.CornerMax().Y()};
    Standard_Real aZArr[2] = {aBndBox.CornerMin().Z(), aBndBox.CornerMax().Z()};
    gp_Pnt aPoints[8];
    int aNum = 0;
    for(int i = 0; i < 2; i++) {
      for(int j = 0; j < 2; j++) {
        for(int k = 0; k < 2; k++) {
          aPoints[aNum] = gp_Pnt(aXArr[i], aYArr[j], aZArr[k]);
          aNum++;
        }
      }
    }

    // Project points to bounding planes. Search max distance to them.
    IntAna_Quadric aBndToQuadric(gp_Pln(aToPnt->impl<gp_Pnt>(), aToDir->impl<gp_Dir>()));
    IntAna_Quadric aBndFromQuadric(gp_Pln(aFromPnt->impl<gp_Pnt>(), aFromDir->impl<gp_Dir>()));
    Standard_Real aMaxToDist = 0, aMaxFromDist = 0;
    for(int i = 0; i < 8; i++) {
      gp_Lin aLine(aPoints[i], aDirVec);
      IntAna_IntConicQuad aToIntAna(aLine, aBndToQuadric);
      IntAna_IntConicQuad aFromIntAna(aLine, aBndFromQuadric);
      if(aToIntAna.NbPoints() == 0 || aFromIntAna.NbPoints() == 0) {
        return;
      }
      const gp_Pnt& aPntOnToFace = aToIntAna.Point(1);
      const gp_Pnt& aPntOnFromFace = aFromIntAna.Point(1);
      if(aPoints[i].Distance(aPntOnToFace) > aMaxToDist) {
        aMaxToDist = aPoints[i].Distance(aPntOnToFace);
      }
      if(aPoints[i].Distance(aPntOnFromFace) > aMaxFromDist) {
        aMaxFromDist = aPoints[i].Distance(aPntOnFromFace);
      }
    }

    // We added 1 just to be sure that prism is long enough for boolean operation.
    double aPrismLength = aMaxToDist + aMaxFromDist + 1;

    // Moving base shape.
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(aDirVec * -aPrismLength);
    BRepBuilderAPI_Transform* aTransformBuilder = new BRepBuilderAPI_Transform(aBaseShape, aTrsf);
    if(!aTransformBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aTransformBuilder)));
    if(!aTransformBuilder->IsDone()) {
      return;
    }
    TopoDS_Shape aMovedBase = aTransformBuilder->Shape();

    // Making prism.
    BRepPrimAPI_MakePrism* aPrismBuilder = new BRepPrimAPI_MakePrism(aMovedBase, aDirVec * 2 * aPrismLength);
    if(!aPrismBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aPrismBuilder)));
    if(!aPrismBuilder->IsDone()) {
      return;
    }
    aResult = aPrismBuilder->Shape();

    // Orienting bounding planes.
    std::shared_ptr<GeomAPI_Pnt> aCentreOfMass = GeomAlgoAPI_ShapeTools::centreOfMass(theBaseShape);
    const gp_Pnt& aCentrePnt = aCentreOfMass->impl<gp_Pnt>();
    gp_Lin aLine(aCentrePnt, aDirVec);
    IntAna_IntConicQuad aToIntAna(aLine, aBndToQuadric);
    IntAna_IntConicQuad aFromIntAna(aLine, aBndFromQuadric);
    Standard_Real aToParameter = aToIntAna.ParamOnConic(1);
    Standard_Real aFromParameter = aFromIntAna.ParamOnConic(1);
    if(aToParameter > aFromParameter) {
      gp_Vec aVec = aToDir->impl<gp_Dir>();
      if((aVec * aDirVec) > 0) {
        aToDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);
      }
      aVec = aFromDir->impl<gp_Dir>();
      if((aVec * aDirVec) < 0) {
        aFromDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);
      }
    } else {
      gp_Vec aVec = aToDir->impl<gp_Dir>();
      if((aVec * aDirVec) < 0) {
        aToDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);
      }
      aVec = aFromDir->impl<gp_Dir>();
      if((aVec * aDirVec) > 0) {
        aFromDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);
      }
    }

    // Making solids from bounding planes.
    TopoDS_Shell aToShell, aFromShell;
    TopoDS_Solid aToSolid, aFromSolid;
    const TopoDS_Shape& aToShape   = aBoundingToShape->impl<TopoDS_Shape>();
    const TopoDS_Shape& aFromShape = aBoundingFromShape->impl<TopoDS_Shape>();
    BRep_Builder aBoundingBuilder;
    aBoundingBuilder.MakeShell(aToShell);
    aBoundingBuilder.Add(aToShell, aToShape);
    aBoundingBuilder.MakeShell(aFromShell);
    aBoundingBuilder.Add(aFromShell, aFromShape);
    aBoundingBuilder.MakeSolid(aToSolid);
    aBoundingBuilder.Add(aToSolid, aToShell);
    aBoundingBuilder.MakeSolid(aFromSolid);
    aBoundingBuilder.Add(aFromSolid, aFromShell);

    // Cutting with to plane.
    BRepAlgoAPI_Cut* aToCutBuilder = new BRepAlgoAPI_Cut(aResult, aToSolid);
    aToCutBuilder->Build();
    if(!aToCutBuilder->IsDone()) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aToCutBuilder)));
    const TopTools_ListOfShape& aToShapes = aToCutBuilder->Modified(aToShape);
    for(TopTools_ListIteratorOfListOfShape anIt(aToShapes); anIt.More(); anIt.Next()) {
      GeomShapePtr aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      this->addToShape(aShape);
    }
    aResult = aToCutBuilder->Shape();
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    }

    // Cutting with from plane.
    BRepAlgoAPI_Cut* aFromCutBuilder = new BRepAlgoAPI_Cut(aResult, aFromSolid);
    aFromCutBuilder->Build();
    if(!aFromCutBuilder->IsDone()) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aFromCutBuilder)));
    const TopTools_ListOfShape& aFromShapes = aFromCutBuilder->Modified(aFromShape);
    for(TopTools_ListIteratorOfListOfShape anIt(aFromShapes); anIt.More(); anIt.Next()) {
      GeomShapePtr aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      this->addFromShape(aShape);
    }
    aResult = aFromCutBuilder->Shape();

    TopoDS_Iterator anIt(aResult);
    if(!anIt.More()) {
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
