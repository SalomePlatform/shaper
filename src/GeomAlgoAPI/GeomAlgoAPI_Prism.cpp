// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "GeomAlgoAPI_Prism.h"

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_XYZ.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_Offset.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Translation.h>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <gp_Pln.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <IntTools_Context.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>


/// Expand planar face to cover the bounding box if theOriginalShape is planar.
/// Otherwise, return the same shape;
static GeomShapePtr buildPlanarFace(const GeomShapePtr& theOriginalShape,
                                    const Bnd_Box& theBaseShapeBB);

/// Build offset for the given shape.
/// If the offset algorithm failed, translate the shape along the direction.
static GeomShapePtr buildOffset(const GeomShapePtr& theShape,
                                const double theOffset,
                                const GeomDirPtr theDirection,
                                GeomAlgoAPI_MakeShapeList& theMakeShapeList);

/// Collect base faces of the prism.
static void collectPrismBases(const TopoDS_Shape& theBaseShape,
                              BRepPrimAPI_MakePrism& thePrismAlgo,
                              ListOfShape& theBoundaries,
                              const GeomAPI_Shape::ShapeType theTypeToExp);

/// Collect all solids which contain boundaries but do not contain bases of prism.
static GeomShapePtr collectResults(const GeomMakeShapePtr& theOperation,
                                   const ListOfShape& theBoundaries,
                                   const ListOfShape& theShapesToExclude,
                                   const GeomAPI_Shape::ShapeType theTypeToExp);

static void storeGenerationHistory(GeomAlgoAPI_Prism* thePrismAlgo,
                                   const TopoDS_Shape& theBase,
                                   const TopAbs_ShapeEnum theType,
                                   BRepPrimAPI_MakePrism* thePrismBuilder);

static void storeGenerationHistory(GeomAlgoAPI_Prism* thePrismAlgo,
                                   const TopoDS_Shape& theResult,
                                   const TopAbs_ShapeEnum theType,
                                   const TopoDS_Face& theToFace,
                                   const TopoDS_Face& theFromFace);

static GeomShapePtr toShape(const TopoDS_Shape& theShape)
{
  GeomShapePtr aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(theShape));
  return aShape;
}


//==================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(const GeomShapePtr theBaseShape,
                                     const GeomDirPtr   theDirection,
                                     const GeomShapePtr theToShape,
                                     const double       theToSize,
                                     const GeomShapePtr theFromShape,
                                     const double       theFromSize)
{
  if(!theBaseShape.get() ||
    (((!theFromShape.get() && !theToShape.get()) ||
    (theFromShape.get() && theToShape.get() && theFromShape->isEqual(theToShape)))
    && (theFromSize == -theToSize))) {
    return;
  }

  // Getting base shape.
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();
  GeomAPI_Shape::ShapeType aShapeTypeToExp;
  switch(aBaseShape.ShapeType()) {
    case TopAbs_VERTEX:
      aShapeTypeToExp = GeomAPI_Shape::VERTEX;
      break;
    case TopAbs_EDGE:
    case TopAbs_WIRE:
      aShapeTypeToExp = GeomAPI_Shape::EDGE;
      break;
    case TopAbs_FACE:
    case TopAbs_SHELL:
      aShapeTypeToExp = GeomAPI_Shape::FACE;
      break;
    case TopAbs_COMPOUND:
      aShapeTypeToExp = GeomAPI_Shape::COMPOUND;
      break;
    default:
      return;
  }

  // Getting direction.
  gp_Vec aBaseVec;
  std::shared_ptr<GeomAPI_Pnt> aBaseLoc;
  std::shared_ptr<GeomAPI_Dir> aBaseDir;
  BRepBuilderAPI_FindPlane aFindPlane(aBaseShape);
  if(aFindPlane.Found() == Standard_True)
  {
    Handle(Geom_Plane) aPlane;
    if(aBaseShape.ShapeType() == TopAbs_FACE || aBaseShape.ShapeType() == TopAbs_SHELL) {
      TopExp_Explorer anExp(aBaseShape, TopAbs_FACE);
      const TopoDS_Shape& aFace = anExp.Current();
      Handle(Geom_Surface) aSurface = BRep_Tool::Surface(TopoDS::Face(aFace));
      if(aSurface->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
        Handle(Geom_RectangularTrimmedSurface) aTrimSurface =
          Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
        aSurface = aTrimSurface->BasisSurface();
      }
      if(aSurface->DynamicType() != STANDARD_TYPE(Geom_Plane)) {
        return;
      }
      aPlane = Handle(Geom_Plane)::DownCast(aSurface);
    } else {
      aPlane = aFindPlane.Plane();
    }
    gp_Pnt aLoc = aPlane->Axis().Location();
    aBaseVec = aPlane->Axis().Direction();
    aBaseLoc.reset(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
    aBaseDir.reset(new GeomAPI_Dir(aBaseVec.X(), aBaseVec.Y(), aBaseVec.Z()));
  }
  else if (theDirection.get())
  {
    aBaseDir = theDirection;
    aBaseVec = theDirection->impl<gp_Dir>();
  }
  else
  {
    return;
  }

  if(!aBaseLoc.get()) {
    gp_Pnt aLoc;
    gp_XYZ aDirXYZ = aBaseVec.XYZ();
    Standard_Real aMinParam = Precision::Infinite();
    for(TopExp_Explorer anExp(aBaseShape, TopAbs_VERTEX); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aVertex = anExp.Current();
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aVertex));
      double aParam = aDirXYZ.Dot(aPnt.XYZ());
      if(aParam < aMinParam) {
        aMinParam = aParam;
        aLoc = aPnt;
      }
    }
    aBaseLoc.reset(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
  }

  gp_Vec anExtVec;
  std::shared_ptr<GeomAPI_Dir> anExtDir;
  if (theDirection.get())
  {
    anExtDir = theDirection;
    anExtVec = theDirection->impl<gp_Dir>();
  }
  else
  {
    anExtDir = aBaseDir;
    anExtVec = aBaseDir->impl<gp_Dir>();
  }


  TopoDS_Shape aResult;
  const bool isBoundingShapesSet = theFromShape.get() || theToShape.get();
  if(!isBoundingShapesSet) {
    buildBySizes(theBaseShape, anExtDir, theToSize, theFromSize, aShapeTypeToExp);
  } else {
    GeomShapePtr aBasePlane = GeomAlgoAPI_FaceBuilder::squareFace(aBaseLoc, aBaseDir, 100.0);

    GeomShapePtr aBoundingFromShape = theFromShape ? theFromShape : aBasePlane;
    GeomShapePtr aBoundingToShape   = theToShape   ? theToShape   : aBasePlane;

    bool isFromShapePlanar = aBoundingFromShape->isPlanar();
    bool isToShapePlanar   = aBoundingToShape->isPlanar();

    // Set signs of offsets if both bounding shapes are planar
    if (isFromShapePlanar && isToShapePlanar) {
      std::shared_ptr<GeomAPI_Pln> aFromPln = GeomAPI_Face(aBoundingFromShape).getPlane();
      std::shared_ptr<GeomAPI_Pln> aToPln = GeomAPI_Face(aBoundingToShape).getPlane();
      buildByPlanes(theBaseShape, anExtDir,
                    aToPln, theToSize,
                    aFromPln, theFromSize,
                    aShapeTypeToExp);
    }
    else {
      buildByFaces(theBaseShape, anExtDir,
                   aBoundingToShape, theToSize, isToShapePlanar,
                   aBoundingFromShape, theFromSize, isFromShapePlanar,
                   aShapeTypeToExp);
    }
  }
}

//==================================================================================================
void GeomAlgoAPI_Prism::buildBySizes(const GeomShapePtr             theBaseShape,
                                     const GeomDirPtr               theDirection,
                                     const double                   theToSize,
                                     const double                   theFromSize,
                                     const GeomAPI_Shape::ShapeType theTypeToExp)
{
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();
  gp_Vec anExtVec = theDirection->impl<gp_Dir>();

  // Moving base shape.
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(anExtVec * -theFromSize);
  BRepBuilderAPI_Transform* aTransformBuilder =
      new BRepBuilderAPI_Transform(aBaseShape, aTrsf);
  if (!aTransformBuilder || !aTransformBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aTransformBuilder)));
  TopoDS_Shape aMovedBase = aTransformBuilder->Shape();

  // Making prism.
  BRepPrimAPI_MakePrism* aPrismBuilder =
      new BRepPrimAPI_MakePrism(aMovedBase, anExtVec * (theFromSize + theToSize));
  if (!aPrismBuilder || !aPrismBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aPrismBuilder)));
  TopoDS_Shape aResult = aPrismBuilder->Shape();

  // Setting naming.
  if(theTypeToExp == GeomAPI_Shape::COMPOUND) {
    storeGenerationHistory(this, aMovedBase, TopAbs_EDGE, aPrismBuilder);
    storeGenerationHistory(this, aMovedBase, TopAbs_FACE, aPrismBuilder);
  } else {
    storeGenerationHistory(this, aMovedBase, (TopAbs_ShapeEnum)theTypeToExp, aPrismBuilder);
  }

  // Setting result.
  if (!aResult.IsNull()) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    this->setShape(toShape(aResult));
    this->setDone(true);
  }
}

//==================================================================================================
void GeomAlgoAPI_Prism::buildByPlanes(const GeomShapePtr             theBaseShape,
                                      const GeomDirPtr               theDirection,
                                      const GeomPlanePtr             theToPlane,
                                      const double                   theToSize,
                                      const GeomPlanePtr             theFromPlane,
                                      const double                   theFromSize,
                                      const GeomAPI_Shape::ShapeType theTypeToExp)
{
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();
  gp_Vec anExtVec = theDirection->impl<gp_Dir>();

  // Moving prism bounding faces according to "from" and "to" sizes.
  std::shared_ptr<GeomAPI_Pnt> aFromLoc = theFromPlane->location();
  std::shared_ptr<GeomAPI_Dir> aFromDir = theFromPlane->direction();

  std::shared_ptr<GeomAPI_Pnt> aToLoc = theToPlane->location();
  std::shared_ptr<GeomAPI_Dir> aToDir = theToPlane->direction();

  std::shared_ptr<GeomAPI_XYZ> anExtDir = theDirection->xyz();
  bool aSign = aFromLoc->xyz()->dot(anExtDir) > aToLoc->xyz()->dot(anExtDir);

  std::shared_ptr<GeomAPI_Pnt> aFromPnt(
    new GeomAPI_Pnt(aFromLoc->xyz()->added(anExtDir->multiplied(
                    aSign ? theFromSize : -theFromSize))));

  std::shared_ptr<GeomAPI_Pnt> aToPnt(
    new GeomAPI_Pnt(aToLoc->xyz()->added(anExtDir->multiplied(
                    aSign ? -theToSize : theToSize))));

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
    gp_Lin aLine(aPoints[i], anExtVec);
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
  aTrsf.SetTranslation(anExtVec * -aPrismLength);
  BRepBuilderAPI_Transform* aTransformBuilder = new BRepBuilderAPI_Transform(aBaseShape, aTrsf);
  if(!aTransformBuilder || !aTransformBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
    new GeomAlgoAPI_MakeShape(aTransformBuilder)));
  TopoDS_Shape aMovedBase = aTransformBuilder->Shape();

  // Making prism.
  BRepPrimAPI_MakePrism* aPrismBuilder =
    new BRepPrimAPI_MakePrism(aMovedBase, anExtVec * 2 * aPrismLength);
  if(!aPrismBuilder || !aPrismBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
    new GeomAlgoAPI_MakeShape(aPrismBuilder)));
  TopoDS_Shape aResult = aPrismBuilder->Shape();

  BRepBndLib::Add(aResult, aBndBox);
  Standard_Real aBndBoxSize = aBndBox.CornerMin().Distance(aBndBox.CornerMax());

  // Orienting bounding planes.
  std::shared_ptr<GeomAPI_Pnt> aCentreOfMass = GeomAlgoAPI_ShapeTools::centreOfMass(theBaseShape);
  const gp_Pnt& aCentrePnt = aCentreOfMass->impl<gp_Pnt>();
  gp_Lin aLine(aCentrePnt, anExtVec);
  IntAna_IntConicQuad aToIntAna(aLine, aBndToQuadric);
  IntAna_IntConicQuad aFromIntAna(aLine, aBndFromQuadric);
  Standard_Real aToParameter = aToIntAna.ParamOnConic(1);
  Standard_Real aFromParameter = aFromIntAna.ParamOnConic(1);
  if(aToParameter > aFromParameter) {
    gp_Vec aVec = aToDir->impl<gp_Dir>();
    if((aVec * anExtVec) > 0)
      aToDir->setImpl(new gp_Dir(aVec.Reversed()));
    aVec = aFromDir->impl<gp_Dir>();
    if((aVec * anExtVec) < 0)
      aFromDir->setImpl(new gp_Dir(aVec.Reversed()));
  } else {
    gp_Vec aVec = aToDir->impl<gp_Dir>();
    if((aVec * anExtVec) < 0)
      aToDir->setImpl(new gp_Dir(aVec.Reversed()));
    aVec = aFromDir->impl<gp_Dir>();
    if((aVec * anExtVec) > 0)
      aFromDir->setImpl(new gp_Dir(aVec.Reversed()));
  }

  static const double THE_FACE_SIZE_COEFF = 10.0;
  GeomShapePtr aBoundingFromShape =
      GeomAlgoAPI_FaceBuilder::squareFace(aFromPnt, aFromDir, THE_FACE_SIZE_COEFF * aBndBoxSize);
  GeomShapePtr aBoundingToShape =
      GeomAlgoAPI_FaceBuilder::squareFace(aToPnt, aToDir, THE_FACE_SIZE_COEFF * aBndBoxSize);

  // bounding planes
  const TopoDS_Shape& aToShape   = aBoundingToShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aFromShape = aBoundingFromShape->impl<TopoDS_Shape>();
  TopoDS_Face aToFace   = TopoDS::Face(aToShape);
  TopoDS_Face aFromFace = TopoDS::Face(aFromShape);

  // Solid based on "To" bounding plane
  gp_Vec aNormal = aToDir->impl<gp_Dir>();
  BRepPrimAPI_MakePrism* aToPrismBuilder =
      new BRepPrimAPI_MakePrism(aToShape, aNormal * (-2.0 * aBndBoxSize));
  if (!aToPrismBuilder || !aToPrismBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
    new GeomAlgoAPI_MakeShape(aToPrismBuilder)));
  TopoDS_Shape aToSolid = aToPrismBuilder->Shape();

  // Cutting with to plane.
  BRepAlgoAPI_Cut* aToCutBuilder = new BRepAlgoAPI_Cut(aResult, aToSolid);
  aToCutBuilder->Build();
  if(!aToCutBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
    new GeomAlgoAPI_MakeShape(aToCutBuilder)));
  aResult = aToCutBuilder->Shape();
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }
  if (theTypeToExp == GeomAPI_Shape::FACE || theTypeToExp == GeomAPI_Shape::COMPOUND) {
    TopTools_ListOfShape aPrismShapes = aToPrismBuilder->Modified(aToShape);
    if (aPrismShapes.IsEmpty())
      aPrismShapes.Append(aToShape);
    for (TopTools_ListIteratorOfListOfShape anIt1(aPrismShapes); anIt1.More(); anIt1.Next()) {
      const TopTools_ListOfShape& aToShapes = aToCutBuilder->Modified(anIt1.Value());
      for (TopTools_ListIteratorOfListOfShape anIt2(aToShapes); anIt2.More(); anIt2.Next()) {
        GeomShapePtr aGeomSh = toShape(anIt2.Value());
        fixOrientation(aGeomSh);
        this->addToShape(aGeomSh);
      }
    }
  }

  // Solid based on "From" bounding plane
  aNormal = aFromDir->impl<gp_Dir>();
  BRepPrimAPI_MakePrism* aFromPrismBuilder =
      new BRepPrimAPI_MakePrism(aFromShape, aNormal * (-2.0 * aBndBoxSize));
  if (!aFromPrismBuilder || !aFromPrismBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
    new GeomAlgoAPI_MakeShape(aFromPrismBuilder)));
  TopoDS_Shape aFromSolid = aFromPrismBuilder->Shape();

  // Cutting with from plane.
  BRepAlgoAPI_Cut* aFromCutBuilder = new BRepAlgoAPI_Cut(aResult, aFromSolid);
  aFromCutBuilder->Build();
  if(!aFromCutBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
    new GeomAlgoAPI_MakeShape(aFromCutBuilder)));
  aResult = aFromCutBuilder->Shape();
  TopoDS_Iterator aCheckIt(aResult);
  if(!aCheckIt.More()) {
    return;
  }
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }
  if (theTypeToExp == GeomAPI_Shape::FACE || theTypeToExp == GeomAPI_Shape::COMPOUND) {
    TopTools_ListOfShape aPrismShapes = aFromPrismBuilder->Modified(aFromShape);
    if (aPrismShapes.IsEmpty())
      aPrismShapes.Append(aFromShape);
    for (TopTools_ListIteratorOfListOfShape anIt1(aPrismShapes); anIt1.More(); anIt1.Next()) {
      const TopTools_ListOfShape& aFromShapes = aFromCutBuilder->Modified(anIt1.Value());
      for (TopTools_ListIteratorOfListOfShape anIt2(aFromShapes); anIt2.More(); anIt2.Next()) {
        GeomShapePtr aGeomSh = toShape(anIt2.Value());
        fixOrientation(aGeomSh);
        this->addFromShape(aGeomSh);
      }
    }
  }

  // Naming for extrusion from vertex, edge.
  if(theTypeToExp == GeomAPI_Shape::COMPOUND) {
    storeGenerationHistory(this, aResult, TopAbs_EDGE, aToFace, aFromFace);
    storeGenerationHistory(this, aResult, TopAbs_FACE, aToFace, aFromFace);
  } else {
    storeGenerationHistory(this, aResult, (TopAbs_ShapeEnum)theTypeToExp, aToFace, aFromFace);
  }

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    GeomShapePtr aGeomShape = toShape(aResult);
    ListOfShape aResults;
    aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                        GeomAPI_Shape::COMPSOLID,
                                                        aResults);
    aResult = aGeomShape->impl<TopoDS_Shape>();
  }

  // Setting result.
  if (!aResult.IsNull()) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    this->setShape(toShape(aResult));
    this->setDone(true);
  }
}

//==================================================================================================
void GeomAlgoAPI_Prism::buildByFaces(const GeomShapePtr             theBaseShape,
                                     const GeomDirPtr               theDirection,
                                     const GeomShapePtr             theToShape,
                                     const double                   theToSize,
                                     const bool                     theToIsPlanar,
                                     const GeomShapePtr             theFromShape,
                                     const double                   theFromSize,
                                     const bool                     theFromIsPlanar,
                                     const GeomAPI_Shape::ShapeType theTypeToExp)
{
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();
  gp_Vec anExtVec = theDirection->impl<gp_Dir>();

  // Moving prism bounding faces according to "from" and "to" sizes.
  GeomShapePtr aBoundingFromShape = buildOffset(theFromShape, -theFromSize, theDirection, *this);
  GeomShapePtr aBoundingToShape   = buildOffset(theToShape, theToSize, theDirection, *this);

  // Bounding box for shapes used in prism building.
  Bnd_Box aBndBox;
  BRepBndLib::Add(aBaseShape, aBndBox);
  BRepBndLib::Add(aBoundingFromShape->impl<TopoDS_Shape>(), aBndBox);
  BRepBndLib::Add(aBoundingToShape->impl<TopoDS_Shape>(), aBndBox);
  double aPrismLength = 2.0 * aBndBox.CornerMin().Distance(aBndBox.CornerMax());

  // Prism building.
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(anExtVec * -aPrismLength);
  BRepBuilderAPI_Transform* aTransformBuilder = new BRepBuilderAPI_Transform(aBaseShape, aTrsf);
  if (!aTransformBuilder || !aTransformBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aTransformBuilder)));
  TopoDS_Shape aMovedBase = aTransformBuilder->Shape();

  // Making prism.
  BRepPrimAPI_MakePrism* aPrismBuilder =
      new BRepPrimAPI_MakePrism(aMovedBase, anExtVec * 2 * aPrismLength);
  if (!aPrismBuilder || !aPrismBuilder->IsDone()) {
    return;
  }
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aPrismBuilder)));

  GeomShapePtr aResult = toShape(aPrismBuilder->Shape());

  // Prism generatrix
  ListOfShape aPrismBaseFaces;
  collectPrismBases(aMovedBase, *aPrismBuilder, aPrismBaseFaces, theTypeToExp);

  // Build planar faces intersecting the prism fully.
  BRepBndLib::Add(aResult->impl<TopoDS_Shape>(), aBndBox);
  aBoundingFromShape = buildPlanarFace(aBoundingFromShape, aBndBox);
  aBoundingToShape   = buildPlanarFace(aBoundingToShape, aBndBox);

  // Perform partition.
  ListOfShape anObjects, aTools;
  anObjects.push_back(aResult);
  aTools.push_back(aBoundingFromShape);
  aTools.push_back(aBoundingToShape);

  GeomMakeShapePtr aPartition(new GeomAlgoAPI_Partition(anObjects, aTools));
  if (!aPartition->isDone())
    return;

  this->appendAlgo(aPartition);

  // Collect pieces of boundary shapes, split by Partition.
  if (theFromIsPlanar) {
    ListOfShape anImagesFrom;
    aPartition->modified(aBoundingFromShape, anImagesFrom);
    for (ListOfShape::iterator anIt = anImagesFrom.begin(); anIt != anImagesFrom.end(); ++anIt)
      addFromShape(*anIt);
  }

  if (theToIsPlanar) {
    ListOfShape anImagesTo;
    aPartition->modified(aBoundingToShape, anImagesTo);
    for (ListOfShape::iterator anIt = anImagesTo.begin(); anIt != anImagesTo.end(); ++anIt)
      addToShape(*anIt);
  }

  // Collect results which have both boundaries, selected for extrusion,
  // but which do not contain top and bottom faces of the prism
  // (these faces are treated as infinitely distant).
  aResult = collectResults(aPartition, aTools, aPrismBaseFaces, theTypeToExp);
  if (aResult && aResult->shapeType() == GeomAPI_Shape::COMPOUND) {
    ListOfShape aResults;
    aResult = GeomAlgoAPI_ShapeTools::combineShapes(aResult,
        theTypeToExp == GeomAPI_Shape::EDGE ? GeomAPI_Shape::SHELL : GeomAPI_Shape::COMPSOLID,
        aResults);

    if (aResults.size() > 1 &&
       (GeomAlgoAPI_ShapeTools::hasSharedTopology(aResults, GeomAPI_Shape::EDGE) ||
        GeomAlgoAPI_ShapeTools::hasSharedTopology(aResults, GeomAPI_Shape::VERTEX))) {
      // results shuold not have shared topology
      aResult = GeomShapePtr();
    }
  }

  if (aResult) {
    this->setShape(aResult);
    this->setDone(true);
  }
}


// Auxilary functions:
//==================================================================================================
GeomShapePtr buildPlanarFace(const GeomShapePtr& theOriginalShape,
                             const Bnd_Box& theBaseShapeBB)
{
  GeomPlanePtr aPlane = GeomAPI_Face(theOriginalShape).getPlane();
  if (!aPlane)
    return theOriginalShape;

  gp_Pnt aCornerMin = theBaseShapeBB.CornerMin();
  gp_Pnt aCornerMax = theBaseShapeBB.CornerMax();
  double aSize = aCornerMin.SquareDistance(aCornerMax);

  gp_Pnt aLocation = aPlane->location()->impl<gp_Pnt>();

  gp_Pnt aCurPnt;
  for (int x = 0; x < 2; ++x) {
    aCurPnt.SetX(x == 0 ? aCornerMin.X() : aCornerMax.X());
    for (int y = 0; y < 2; ++y) {
      aCurPnt.SetY(y == 0 ? aCornerMin.Y() : aCornerMax.Y());
      for (int z = 0; z < 2; ++z) {
        aCurPnt.SetZ(z == 0 ? aCornerMin.Z() : aCornerMax.Z());
        double aDist = aCurPnt.SquareDistance(aLocation);
        if (aDist > aSize)
          aSize = aDist;
      }
    }
  }

  aSize = Sqrt(aSize);
  return GeomAlgoAPI_FaceBuilder::squareFace(aPlane, 2.0 * aSize);
}

//==================================================================================================
GeomShapePtr buildOffset(const GeomShapePtr& theShape,
                         const double theOffset,
                         const GeomDirPtr theDirection,
                         GeomAlgoAPI_MakeShapeList& theMakeShapeList)
{
  if (Abs(theOffset) < Precision::Confusion())
    return theShape; // no need zero offset

  GeomMakeShapePtr anAlgo(new GeomAlgoAPI_Offset(theShape, theOffset));
  if (!anAlgo->isDone()) {
    // offset not done, perform translation
    std::shared_ptr<GeomAPI_Ax1> anAxis(new GeomAPI_Ax1());
    anAxis->setDir(theDirection);
    anAlgo.reset(new GeomAlgoAPI_Translation(theShape, anAxis, theOffset));
  }

  GeomShapePtr aResult = theShape;
  if (anAlgo->isDone()) {
    theMakeShapeList.appendAlgo(anAlgo);
    aResult = anAlgo->shape();
  }
  return aResult;
}

//==================================================================================================
void collectPrismBases(const TopoDS_Shape& theBaseShape,
                       BRepPrimAPI_MakePrism& thePrismAlgo,
                       ListOfShape& theBoundaries,
                       const GeomAPI_Shape::ShapeType theTypeToExp)
{
  for (TopExp_Explorer anExp(theBaseShape, (TopAbs_ShapeEnum)theTypeToExp);
       anExp.More(); anExp.Next()) {
    theBoundaries.push_back(toShape(thePrismAlgo.FirstShape(anExp.Current())));
    theBoundaries.push_back(toShape(thePrismAlgo.LastShape(anExp.Current())));
  }
}

//==================================================================================================
typedef std::set<GeomShapePtr, GeomAPI_Shape::Comparator> SetOfShape;

bool isShapeApplicable(const GeomShapePtr& theSolid,
                       const std::list<ListOfShape>& theShapesToExist,
                       const SetOfShape& theShapesToExclude,
                       const GeomAPI_Shape::ShapeType theTypeToExp)
{
  SetOfShape aFaces;
  for (GeomAPI_ShapeExplorer aFExp(theSolid, theTypeToExp);
       aFExp.more(); aFExp.next()) {
    GeomShapePtr aCurrent = aFExp.current();
    if (theShapesToExclude.find(aCurrent) != theShapesToExclude.end())
      return false;
    aFaces.insert(aCurrent);
  }

  // check all faces are in solid
  bool isApplicable = true;
  for (std::list<ListOfShape>::const_iterator it1 = theShapesToExist.begin();
       it1 != theShapesToExist.end() && isApplicable; ++it1) {
    ListOfShape::const_iterator it2 = it1->begin();
    for (; it2 != it1->end(); ++it2)
      if (aFaces.find(*it2) != aFaces.end())
        break;
    isApplicable = it2 != it1->end();
  }
  return isApplicable;
}

void collectModified(const GeomMakeShapePtr& theOperation,
                     const ListOfShape& theShapes,
                     std::list<ListOfShape>& theModified)
{
  for (ListOfShape::const_iterator anIt = theShapes.begin();
       anIt != theShapes.end(); ++anIt) {
    theModified.push_back(ListOfShape());
    theOperation->modified(*anIt, theModified.back());
    theOperation->generated(*anIt, theModified.back());
    theModified.back().push_back(*anIt);
  }
}

GeomShapePtr collectResults(const GeomMakeShapePtr& theOperation,
                            const ListOfShape& theBoundaries,
                            const ListOfShape& theShapesToExclude,
                            const GeomAPI_Shape::ShapeType theTypeToExp)
{
  ListOfShape aResults;

  // collect modified shapes
  std::list<ListOfShape> aModifiedBoundaries;
  collectModified(theOperation, theBoundaries, aModifiedBoundaries);

  std::list<ListOfShape> aModifiedExclude;
  collectModified(theOperation, theShapesToExclude, aModifiedExclude);
  SetOfShape aTabooShapes;
  for (std::list<ListOfShape>::iterator anIt = aModifiedExclude.begin();
       anIt != aModifiedExclude.end(); ++anIt)
    aTabooShapes.insert(anIt->begin(), anIt->end());

  // type of sub-shapes to explode
  GeomAPI_Shape::ShapeType aSubshapeType;
  switch (theTypeToExp) {
  case GeomAPI_Shape::VERTEX:
    aSubshapeType = GeomAPI_Shape::EDGE;
    break;
  case GeomAPI_Shape::EDGE:
    aSubshapeType = GeomAPI_Shape::FACE;
    break;
  case GeomAPI_Shape::FACE:
    aSubshapeType = GeomAPI_Shape::SOLID;
    break;
  default:
    aSubshapeType = GeomAPI_Shape::COMPOUND;
  }

  // search applicable solids
  GeomShapePtr anOperationResult = theOperation->shape();
  for (GeomAPI_ShapeExplorer anExp(anOperationResult, aSubshapeType);
        anExp.more(); anExp.next()) {
    if (isShapeApplicable(anExp.current(), aModifiedBoundaries, aTabooShapes, theTypeToExp))
      aResults.push_back(anExp.current());
  }

  GeomShapePtr aResult;
  if (aResults.size() == 1)
    aResult = aResults.front();
  else if (!aResults.empty())
    aResult = GeomAlgoAPI_CompoundBuilder::compound(aResults);
  return aResult;
}

//==================================================================================================
void storeGenerationHistory(GeomAlgoAPI_Prism* thePrismAlgo,
                            const TopoDS_Shape& theBase,
                            const TopAbs_ShapeEnum theType,
                            BRepPrimAPI_MakePrism* thePrismBuilder)
{
  for(TopExp_Explorer anExp(theBase, theType); anExp.More(); anExp.Next()) {
    const TopoDS_Shape& aShape = anExp.Current();
    GeomShapePtr aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
    aFromShape->setImpl(new TopoDS_Shape(thePrismBuilder->FirstShape(aShape)));
    aToShape->setImpl(new TopoDS_Shape(thePrismBuilder->LastShape(aShape)));
    thePrismAlgo->fixOrientation(aFromShape);
    thePrismAlgo->fixOrientation(aToShape);
    thePrismAlgo->addFromShape(aFromShape);
    thePrismAlgo->addToShape(aToShape);
  }
}

//==================================================================================================
void storeGenerationHistory(GeomAlgoAPI_Prism* thePrismAlgo,
                            const TopoDS_Shape& theResult,
                            const TopAbs_ShapeEnum theType,
                            const TopoDS_Face& theToFace,
                            const TopoDS_Face& theFromFace)
{
  for(TopExp_Explorer anExp(theResult, theType); anExp.More(); anExp.Next()) {
    const TopoDS_Shape& aShape = anExp.Current();
    GeomShapePtr aGeomSh(new GeomAPI_Shape());
    if(theType == TopAbs_VERTEX) {
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
      IntTools_Context anIntTools;
      if(anIntTools.IsValidPointForFace(aPnt,
          theToFace, Precision::Confusion()) == Standard_True) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        thePrismAlgo->fixOrientation(aGeomSh);
        thePrismAlgo->addToShape(aGeomSh);
      }
      if(anIntTools.IsValidPointForFace(aPnt,
          theFromFace, Precision::Confusion()) == Standard_True) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        thePrismAlgo->fixOrientation(aGeomSh);
        thePrismAlgo->addFromShape(aGeomSh);
      }
    } else if(theType == TopAbs_EDGE) {
      TopoDS_Edge anEdge = TopoDS::Edge(aShape);
      BRepLib_CheckCurveOnSurface anEdgeCheck(anEdge, theToFace);
      anEdgeCheck.Perform();
      if(anEdgeCheck.MaxDistance() < Precision::Confusion()) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        thePrismAlgo->fixOrientation(aGeomSh);
        thePrismAlgo->addToShape(aGeomSh);
      }
      anEdgeCheck.Init(anEdge, theFromFace);
      anEdgeCheck.Perform();
      if(anEdgeCheck.MaxDistance() < Precision::Confusion()) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        thePrismAlgo->fixOrientation(aGeomSh);
        thePrismAlgo->addFromShape(aGeomSh);
      }
    } else {
      break;
    }
  }
}
