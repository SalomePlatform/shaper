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
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <gp_Pln.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                     double                         theToSize,
                                     double                         theFromSize)
{
  build(theBaseShape, std::shared_ptr<GeomAPI_Shape>(), theToSize, std::shared_ptr<GeomAPI_Shape>(), theFromSize);
}

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                     std::shared_ptr<GeomAPI_Shape> theToShape,
                                     double                         theToSize,
                                     std::shared_ptr<GeomAPI_Shape> theFromShape,
                                     double                         theFromSize)
{
  build(theBaseShape, theToShape, theToSize, theFromShape, theFromSize);
}

//=================================================================================================
void GeomAlgoAPI_Prism::build(const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                              const std::shared_ptr<GeomAPI_Shape>& theToShape,
                              double                                theToSize,
                              const std::shared_ptr<GeomAPI_Shape>& theFromShape,
                              double                                theFromSize)
{
  if(!theBaseShape ||
    (((!theFromShape && !theToShape) || (theFromShape && theToShape && theFromShape->isEqual(theToShape)))
    && (theFromSize == -theToSize))) {
    return;
  }

  // Getting base plane.
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();
  std::shared_ptr<GeomAPI_Face> aBaseFace;
  if(theBaseShape->shapeType() == GeomAPI_Shape::FACE) {
    aBaseFace = std::shared_ptr<GeomAPI_Face>(new GeomAPI_Face(theBaseShape));
  } else if(theBaseShape->shapeType() == GeomAPI_Shape::SHELL){
    GeomAPI_ShapeExplorer anExp(theBaseShape, GeomAPI_Shape::FACE);
    if(anExp.more()) {
      std::shared_ptr<GeomAPI_Shape> aFaceOnShell = anExp.current();
      aBaseFace = std::shared_ptr<GeomAPI_Face>(new GeomAPI_Face(aFaceOnShell));
    }
  }
  if(!aBaseFace.get()) {
    return;
  }

  std::shared_ptr<GeomAPI_Pln>   aBasePln = aBaseFace->getPlane();
  std::shared_ptr<GeomAPI_Dir>   aBaseDir = aBasePln->direction();
  std::shared_ptr<GeomAPI_Pnt>   aBaseLoc = aBasePln->location();
  std::shared_ptr<GeomAPI_Shape> aBasePlane = GeomAlgoAPI_FaceBuilder::planarFace(aBaseLoc, aBaseDir);

  gp_Vec aBaseVec(aBaseDir->impl<gp_Dir>());
  const gp_Pnt& aBasePnt = aBaseLoc->impl<gp_Pnt>();

  TopoDS_Shape aResult;
  bool isBoundingShapesSet = theFromShape || theToShape;
  if(!isBoundingShapesSet) {
    // Moving base shape.
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(aBaseVec * -theFromSize);
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
    BRepPrimAPI_MakePrism* aPrismBuilder = new BRepPrimAPI_MakePrism(aMovedBase, aBaseVec * (theFromSize + theToSize));
    if(!aPrismBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aPrismBuilder)));
    if(!aPrismBuilder->IsDone()) {
      return;
    }
    aResult = aPrismBuilder->Shape();

    // Setting naming.
    for(TopExp_Explorer anExp(aMovedBase, TopAbs_FACE); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aFace = anExp.Current();
      std::shared_ptr<GeomAPI_Shape> aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
      aFromShape->setImpl(new TopoDS_Shape(aPrismBuilder->FirstShape(aFace)));
      aToShape->setImpl(new TopoDS_Shape(aPrismBuilder->LastShape(aFace)));
      this->addFromFace(aFromShape);
      this->addToFace(aToShape);
    }
  } else {
    std::shared_ptr<GeomAPI_Shape> aBoundingFromShape = theFromShape ? theFromShape : aBasePlane;
    std::shared_ptr<GeomAPI_Shape> aBoundingToShape   = theToShape   ? theToShape   : aBasePlane;

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
      gp_Lin aLine(aPoints[i], aBaseVec);
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
    aTrsf.SetTranslation(aBaseVec * -aPrismLength);
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
    BRepPrimAPI_MakePrism* aPrismBuilder = new BRepPrimAPI_MakePrism(aMovedBase, aBaseVec * 2 * aPrismLength);
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
    gp_Lin aLine(aCentrePnt, aBaseVec);
    IntAna_IntConicQuad aToIntAna(aLine, aBndToQuadric);
    IntAna_IntConicQuad aFromIntAna(aLine, aBndFromQuadric);
    Standard_Real aToParameter = aToIntAna.ParamOnConic(1);
    Standard_Real aFromParameter = aFromIntAna.ParamOnConic(1);
    if(aToParameter > aFromParameter) {
      gp_Vec aVec = aToDir->impl<gp_Dir>();
      if((aVec * aBaseVec) > 0) {
        aToDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);
      }
      aVec = aFromDir->impl<gp_Dir>();
      if((aVec * aBaseVec) < 0) {
        aFromDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);
      }
    } else {
      gp_Vec aVec = aToDir->impl<gp_Dir>();
      if((aVec * aBaseVec) < 0) {
        aToDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);
      }
      aVec = aFromDir->impl<gp_Dir>();
      if((aVec * aBaseVec) > 0) {
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
    aBoundingBuilder.MakeShell(aFromShell);
    aBoundingBuilder.Add(aToShell, aToShape);
    aBoundingBuilder.Add(aFromShell, aFromShape);
    aBoundingBuilder.MakeSolid(aToSolid);
    aBoundingBuilder.MakeSolid(aFromSolid);
    aBoundingBuilder.Add(aToSolid, aToShell);
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
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      this->addToFace(aShape);
    }
    aResult = aToCutBuilder->Shape();

    // Cutting with from plane.
    BRepAlgoAPI_Cut* aFromCutBuilder = new BRepAlgoAPI_Cut(aResult, aFromSolid);
    aFromCutBuilder->Build();
    if(!aFromCutBuilder->IsDone()) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aFromCutBuilder)));
    const TopTools_ListOfShape& aFromShapes = aFromCutBuilder->Modified(aFromShape);
    for(TopTools_ListIteratorOfListOfShape anIt(aFromShapes); anIt.More(); anIt.Next()) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      this->addFromFace(aShape);
    }
    aResult = aFromCutBuilder->Shape();

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
  }

  // Setting result.
  if(aResult.IsNull()) {
    return;
  }
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
