// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Prism.cpp
// Created:     5 May 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Prism.h>

#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_XYZ.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepFeat_MakePrism.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepTools.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(std::shared_ptr<GeomAPI_Shape> theBasis,
                                     double                         theToSize,
                                     double                         theFromSize)
: myDone(false)
{
  build(theBasis, std::shared_ptr<GeomAPI_Shape>(), theToSize, std::shared_ptr<GeomAPI_Shape>(), theFromSize);
}

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(std::shared_ptr<GeomAPI_Shape> theBasis,
                                     std::shared_ptr<GeomAPI_Shape> theToShape,
                                     double                         theToSize,
                                     std::shared_ptr<GeomAPI_Shape> theFromShape,
                                     double                         theFromSize)
: myDone(false)
{
  build(theBasis, theToShape, theToSize, theFromShape, theFromSize);
}

//=================================================================================================
void GeomAlgoAPI_Prism::build(const std::shared_ptr<GeomAPI_Shape>& theBasis,
                              const std::shared_ptr<GeomAPI_Shape>& theToShape,
                              double                                theToSize,
                              const std::shared_ptr<GeomAPI_Shape>& theFromShape,
                              double                                theFromSize)
{
  if(!theBasis ||
    (((!theFromShape && !theToShape) || (theFromShape && theToShape && theFromShape->isEqual(theToShape)))
    && (theFromSize == -theToSize))) {
    return;
  }

  // If bounding faces was not set creating them.
  std::shared_ptr<GeomAPI_Face> aBaseFace;
  if(theBasis->shapeType() == GeomAPI_Shape::FACE) {
    aBaseFace = std::shared_ptr<GeomAPI_Face>(new GeomAPI_Face(theBasis));
  } else if(theBasis->shapeType() == GeomAPI_Shape::SHELL){
    GeomAPI_ShapeExplorer anExp(theBasis, GeomAPI_Shape::FACE);
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

  std::shared_ptr<GeomAPI_Shape> aBoundingFromShape = theFromShape ? theFromShape : aBasePlane;
  std::shared_ptr<GeomAPI_Shape> aBoundingToShape   = theToShape   ? theToShape   : aBasePlane;

  // Moving bounding faces according to "from" and "to" sizes.
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

  if(theBasis->shapeType() == GeomAPI_Shape::FACE) {
    TopoDS_Face aBasis = TopoDS::Face(aBaseFace->impl<TopoDS_Shape>());
    const gp_Dir& aNormal = aBaseDir->impl<gp_Dir>();
    BRepFeat_MakePrism* aBuilder = new BRepFeat_MakePrism(aBasis, aBasis, aBasis, aNormal, 2, Standard_True);

    if(aBuilder) {
      const TopoDS_Shape& aFromShape = aBoundingFromShape->impl<TopoDS_Shape>();
      const TopoDS_Shape& aToShape   = aBoundingToShape->impl<TopoDS_Shape>();
      aBuilder->Perform(aFromShape, aToShape);
      myDone = aBuilder->IsDone() == Standard_True;
      if(myDone){
        TopoDS_Shape aResult = aBuilder->Shape();
        TopExp_Explorer anExp(aResult, TopAbs_SOLID);
        if(!anExp.More()) {
          return;
        }
        if(aResult.ShapeType() == TopAbs_COMPOUND) {
          aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
        }
        // fill data map to keep correct orientation of sub-shapes
        myMap.reset(new GeomAPI_DataMapOfShapeShape);
        for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
          std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
          aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
          myMap->bind(aCurrentShape, aCurrentShape);
        }
        myShape.reset(new GeomAPI_Shape);
        myShape->setImpl(new TopoDS_Shape(aResult));
        std::shared_ptr<GeomAPI_Shape> aFrom(new GeomAPI_Shape());
        aFrom->setImpl(new TopoDS_Shape(aBuilder->Modified(aFromShape).First()));
        myFromFaces.push_back(aFrom);
        std::shared_ptr<GeomAPI_Shape> aTo(new GeomAPI_Shape());
        aTo->setImpl(new TopoDS_Shape(aBuilder->Modified(aToShape).First()));
        myToFaces.push_back(aTo);
        myMkShape.reset(new GeomAlgoAPI_MakeShape(aBuilder));
      }
    }
  } else {
    // Getting bounding box for base shape.
    const TopoDS_Shape& aBasisShape = theBasis->impl<TopoDS_Shape>();
    Bnd_Box aBndBox;
    BRepBndLib::Add(aBasisShape, aBndBox);
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
    gp_Vec aNormal(aBaseDir->impl<gp_Dir>());
    for(int i = 0; i < 8; i++) {
      gp_Lin aLine(aPoints[i], aNormal);
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
    // We added 1 just to be sure that pipe is long enough for boolean operation.
    Standard_Real aPipeLength = aMaxToDist + aMaxFromDist + 1;

    // Making wire for pipe.
    std::shared_ptr<GeomAPI_Pnt> aCentreOfMass = GeomAlgoAPI_ShapeTools::centreOfMass(theBasis);
    const gp_Pnt aCentrePnt = aCentreOfMass->impl<gp_Pnt>();
    TopoDS_Face aFace = TopoDS::Face(aBaseFace->impl<TopoDS_Shape>());
    gp_Pnt aPipeStartPnt = aCentrePnt.Translated(aNormal.Scaled(aPipeLength));
    gp_Pnt aPipeEndPnt = aCentrePnt.Translated(aNormal.Scaled(-aPipeLength));
    TopoDS_Edge aPipeEdge = BRepBuilderAPI_MakeEdge(aPipeStartPnt, aPipeEndPnt);
    TopoDS_Wire aPipeWire = BRepBuilderAPI_MakeWire(aPipeEdge).Wire();

    // Making pipe.
    ListOfMakeShape aListOfMakeShape;
    BRepOffsetAPI_MakePipe* aPipeBuilder = new BRepOffsetAPI_MakePipe(aPipeWire, aBasisShape);
    if(!aPipeBuilder) {
      return;
    }
    std::shared_ptr<GeomAPI_Shape> aWire(new GeomAPI_Shape);
    std::shared_ptr<GeomAPI_Shape> aBShape(new GeomAPI_Shape);
    aWire->setImpl(new TopoDS_Shape(aPipeWire));
    aBShape->setImpl(new TopoDS_Shape(aBasisShape));
    aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aPipeBuilder, aWire, aBShape)));
    TopoDS_Shape aResult = aPipeBuilder->Shape();

    // Orienting bounding planes.
    gp_Lin aLine(aCentrePnt, aNormal);
    IntAna_IntConicQuad aToIntAna(aLine, aBndToQuadric);
    IntAna_IntConicQuad aFromIntAna(aLine, aBndFromQuadric);
    Standard_Real aToParameter = aToIntAna.ParamOnConic(1);
    Standard_Real aFromParameter = aFromIntAna.ParamOnConic(1);
    if(aToParameter > aFromParameter) {
      gp_Vec aVec = aToDir->impl<gp_Dir>();
      if((aVec * aNormal) > 0) {
        aToDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);
      }
      aVec = aFromDir->impl<gp_Dir>();
      if((aVec * aNormal) < 0) {
        aFromDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);
      }
    } else {
      gp_Vec aVec = aToDir->impl<gp_Dir>();
      if((aVec * aNormal) < 0) {
        aToDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);
      }
      aVec = aFromDir->impl<gp_Dir>();
      if((aVec * aNormal) > 0) {
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
    aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aToCutBuilder)));
    const TopTools_ListOfShape& aToShapes = aToCutBuilder->Modified(aToShape);
    for(TopTools_ListIteratorOfListOfShape anIt(aToShapes); anIt.More(); anIt.Next()) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      myToFaces.push_back(aShape);
    }
    aResult = aToCutBuilder->Shape();

    // Cutting with from plane.
    BRepAlgoAPI_Cut* aFromCutBuilder = new BRepAlgoAPI_Cut(aResult, aFromSolid);
    aFromCutBuilder->Build();
    if(!aFromCutBuilder->IsDone()) {
      return;
    }
    aListOfMakeShape.push_back(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aFromCutBuilder)));
    const TopTools_ListOfShape& aFromShapes = aFromCutBuilder->Modified(aFromShape);
    for(TopTools_ListIteratorOfListOfShape anIt(aFromShapes); anIt.More(); anIt.Next()) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      myFromFaces.push_back(aShape);
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

    // Fill data map to keep correct orientation of sub-shapes.
    myMap = std::shared_ptr<GeomAPI_DataMapOfShapeShape>(new GeomAPI_DataMapOfShapeShape);
    for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
      std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
      aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
      myMap->bind(aCurrentShape, aCurrentShape);
    }
    myShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape);
    myShape->setImpl(new TopoDS_Shape(aResult));
    myMkShape = std::shared_ptr<GeomAlgoAPI_MakeShapeList>(new GeomAlgoAPI_MakeShapeList(aListOfMakeShape));
    myDone = true;
  }
}

//=================================================================================================
bool GeomAlgoAPI_Prism::isDone() const
{
  return myDone;
}

//=================================================================================================
bool GeomAlgoAPI_Prism::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
bool GeomAlgoAPI_Prism::hasVolume() const
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
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Prism::shape() const
{
  return myShape;
}

//=================================================================================================
const ListOfShape& GeomAlgoAPI_Prism::fromFaces() const
{
  return myFromFaces;
}

//=================================================================================================
const ListOfShape& GeomAlgoAPI_Prism::toFaces() const
{
  return myToFaces;
}

//=================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_Prism::mapOfShapes() const
{
  return myMap;
}

//=================================================================================================
std::shared_ptr<GeomAlgoAPI_MakeShape> GeomAlgoAPI_Prism::makeShape() const
{
  return myMkShape;
}
