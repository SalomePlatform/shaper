// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_Revolution.h"

#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepGProp.hxx>
#include <GC_MakePlane.hxx>
#include <Geom_Plane.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <IntTools_Context.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

/// \brief Constructs infinite face from thePlane, and with axis located on the same side
/// of the plane as thePoint. Modifies thePlane axis direction.
/// \param[in,out] thePlane plane to construct face.
/// \param[in] thePoint point to locate plane axis.
/// \return constructed face.
static TopoDS_Face makeFaceFromPlane(gp_Pln& thePlane, const gp_Pnt& thePoint);

/// \return solid created from face or shell.
static TopoDS_Solid makeSolidFromShape(const TopoDS_Shape& theShape);

/// \brief return centre of mass for theShape.
/// \param[in] theShape shape.
static gp_Pnt centreOfMass(const TopoDS_Shape& theShape);

/// \brief Selects solid from theShape with closest center of mass to thePoint
/// \param[in] theShape compound with solids.
/// \param[in] thePoint point.
/// \return solid.
static TopoDS_Shape findClosest(const TopoDS_Shape& theShape, const gp_Pnt& thePoint);

static void storeGenerationHistory(GeomAlgoAPI_Revolution* theRevolutionAlgo,
                                   const TopoDS_Shape& theBase,
                                   const TopAbs_ShapeEnum theType,
                                   BRepPrimAPI_MakeRevol* theRevolBuilder);

static void storeGenerationHistory(GeomAlgoAPI_Revolution* theRevolutionAlgo,
                                   const TopoDS_Shape& theResult,
                                   const TopAbs_ShapeEnum theType,
                                   const TopoDS_Shape& theToFace,
                                   const TopoDS_Shape& theFromFace);

static void storeGenerationHistory(GeomAlgoAPI_Revolution* theRevolutionAlgo,
                                   const TopoDS_Shape& theResult,
                                   const TopAbs_ShapeEnum theType,
                                   const TopoDS_Shape& theRotatedBoundingFace,
                                   const TopoDS_Shape& theModifiedBaseShape,
                                   const bool theIsFromFaceSet);

//==================================================================================================
GeomAlgoAPI_Revolution::GeomAlgoAPI_Revolution(const GeomShapePtr                 theBaseShape,
                                               const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                               const GeomShapePtr                 theToShape,
                                               const double                       theToAngle,
                                               const GeomShapePtr                 theFromShape,
                                               const double                       theFromAngle)
{
  build(theBaseShape, theAxis, theToShape, theToAngle, theFromShape, theFromAngle);
}

//==================================================================================================
void GeomAlgoAPI_Revolution::build(const GeomShapePtr&                 theBaseShape,
                                   const std::shared_ptr<GeomAPI_Ax1>& theAxis,
                                   const GeomShapePtr&                 theToShape,
                                   const double                        theToAngle,
                                   const GeomShapePtr&                 theFromShape,
                                   const double                        theFromAngle)
{
  if(!theBaseShape || !theAxis ||
    (((!theFromShape && !theToShape) ||
    (theFromShape && theToShape && theFromShape->isEqual(theToShape)))
    && (theFromAngle == -theToAngle))) {
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
    case TopAbs_COMPOUND:
      aShapeTypeToExp = TopAbs_COMPOUND;
      break;
    default:
      return;
  }

  // Getting axis.
  gp_Ax1 anAxis = theAxis->impl<gp_Ax1>();

  // Getting base plane.
  Handle(Geom_Plane) aBasePlane;
  BRepBuilderAPI_FindPlane aFindPlane(aBaseShape);
  if(aShapeTypeToExp == TopAbs_FACE && aFindPlane.Found() == Standard_True) {
    aBasePlane = aFindPlane.Plane();
  } else {
    gp_Pnt aPnt1 = anAxis.Location();
    gp_Pnt aPnt2 = aPnt1;
    aPnt2.Translate(anAxis.Direction());
    gp_Pnt aPnt3;

    for(TopExp_Explorer anExp(aBaseShape, TopAbs_VERTEX); anExp.More(); anExp.Next()) {
      aPnt3 = BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current()));

      GC_MakePlane aMkPlane(aPnt1, aPnt2, aPnt3);
      if(aMkPlane.IsDone() != Standard_True) {
        continue;
      }

      aBasePlane = aMkPlane.Value();
      break;
    }

    if(aBasePlane.IsNull()) {
      aPnt3 = centreOfMass(aBaseShape);

      GC_MakePlane aMkPlane(aPnt1, aPnt2, aPnt3);
      if(aMkPlane.IsDone() != Standard_True) {
        return;
      }

      aBasePlane = aMkPlane.Value();
    }
  }

  if(aShapeTypeToExp == TopAbs_FACE) {
    if(aBasePlane->Axis().Angle(anAxis) < Precision::Confusion()) {
      return;
    }
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
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aBaseTransform)));
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
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aRevolBuilder)));
    if(!aRevolBuilder->IsDone()) {
      return;
    }
    aResult = aRevolBuilder->Shape();

    // Setting naming.
    if(aShapeTypeToExp == TopAbs_COMPOUND) {
      storeGenerationHistory(this, aRotatedBase, TopAbs_EDGE, aRevolBuilder);
      storeGenerationHistory(this, aRotatedBase, TopAbs_FACE, aRevolBuilder);
    } else {
      storeGenerationHistory(this, aRotatedBase, aShapeTypeToExp, aRevolBuilder);
    }
  } else if(theFromShape && theToShape) { // Case 2: When both bounding planes were set.
    // Making revolution to the 360 angle.
    BRepPrimAPI_MakeRevol* aRevolBuilder =
      new BRepPrimAPI_MakeRevol(aBaseShape, anAxis, 2 * M_PI, Standard_True);
    if(!aRevolBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aRevolBuilder)));
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
    if(!isFromPlanar.IsPlanar() || !isToPlanar.IsPlanar()) {
      // non-planar shapes is not supported for revolution bounding
      return;
    }

    std::shared_ptr<GeomAPI_Face> aGeomFromFace(new GeomAPI_Face(theFromShape));
    std::shared_ptr<GeomAPI_Face> aGeomToFace(new GeomAPI_Face(theToShape));

    gp_Pln aFromPln = aGeomFromFace->getPlane()->impl<gp_Pln>();
    gp_Pln aToPln   = aGeomToFace->getPlane()->impl<gp_Pln>();

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
    double aFromRotAngle =
      ((aFromPln.Axis().Direction() * aBasePlane->Axis().Direction()) > 0) ? -theFromAngle :
                                                                              theFromAngle;
    double aToRotAngle =
      ((aToPln.Axis().Direction() * aBasePlane->Axis().Direction()) > 0) ? -theToAngle :
                                                                            theToAngle;
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
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aFromCutBuilder)));
    aResult = aFromCutBuilder->Shape();
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    }

    // Cutting revolution with to plane.
    BRepAlgoAPI_Cut* aToCutBuilder = new BRepAlgoAPI_Cut(aResult, aToSolid);
    aToCutBuilder->Build();
    if(!aToCutBuilder->IsDone()) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aToCutBuilder)));
    aResult = aToCutBuilder->Shape();
    TopoDS_Iterator aCheckIt(aResult);
    if(!aCheckIt.More()) {
      return;
    }
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    }
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
      aGeomShape->setImpl(new TopoDS_Shape(aResult));
      ListOfShape aResults;
      aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                         GeomAPI_Shape::COMPSOLID,
                                                         aResults);
      aResult = aGeomShape->impl<TopoDS_Shape>();
    }

    // If after cut we got more than one solids then take closest
    // to the center of mass of the base face.
    aResult = findClosest(aResult, aBaseCentre);

    // Setting naming.
    if(aShapeTypeToExp == TopAbs_COMPOUND) {
      storeGenerationHistory(this, aResult, TopAbs_EDGE, aRotatedToFace, aRotatedFromFace);
      storeGenerationHistory(this, aResult, TopAbs_FACE, aRotatedToFace, aRotatedFromFace);
    } else {
      storeGenerationHistory(this, aResult, aShapeTypeToExp, aRotatedToFace, aRotatedFromFace);
    }
  } else { //Case 3: When only one bounding plane was set.
    // Making revolution to the 360 angle.
    BRepPrimAPI_MakeRevol* aRevolBuilder =
      new BRepPrimAPI_MakeRevol(aBaseShape, anAxis, 2 * M_PI, Standard_True);
    if(!aRevolBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aRevolBuilder)));
    if(!aRevolBuilder->IsDone()) {
      return;
    }
    aResult = aRevolBuilder->Shape();

    // Getting bounding face.
    bool isFromFaceSet = false;
    std::shared_ptr<GeomAPI_Face> aGeomBoundingFace;
    if(theFromShape) {
      aGeomBoundingFace.reset(new GeomAPI_Face(theFromShape));
      isFromFaceSet = true;
    } else if(theToShape) {
      aGeomBoundingFace.reset(new GeomAPI_Face(theToShape));
    }
    TopoDS_Face aBoundingFace = TopoDS::Face(aGeomBoundingFace->impl<TopoDS_Shape>());

    // Getting plane from bounding face.
    GeomLib_IsPlanarSurface isBoundingPlanar(BRep_Tool::Surface(aBoundingFace));
    if(!isBoundingPlanar.IsPlanar()) { // non-planar shapes is not supported for revolution bounding
      return;
    }

    gp_Pln aBoundingPln = aGeomBoundingFace->getPlane()->impl<gp_Pln>();

    // Orienting bounding plane properly so that the center of mass of the base face stays
    // on the result shape after cut.
    aBoundingFace = makeFaceFromPlane(aBoundingPln, aBaseCentre);

    // Making solid from bounding plane.
    TopoDS_Shape aBoundingSolid = makeSolidFromShape(aBoundingFace);

    // Rotating bounding plane to the specified angle.
    double aBoundingRotAngle = isFromFaceSet ? theFromAngle : theToAngle;
    if(aBoundingPln.Axis().IsParallel(aBasePlane->Axis(), Precision::Confusion())) {
      if(isFromFaceSet) aBoundingRotAngle = -aBoundingRotAngle;
    } else {
      double aSign = (aBoundingPln.Axis().Direction() ^ aBasePlane->Axis().Direction()) *
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
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aBoundingCutBuilder)));
    aResult = aBoundingCutBuilder->Shape();
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    }

    // Setting naming.
    if(aShapeTypeToExp == TopAbs_FACE || aShapeTypeToExp == TopAbs_COMPOUND) {
      const TopTools_ListOfShape& aBndShapes = aBoundingCutBuilder->Modified(aBoundingFace);
      for(TopTools_ListIteratorOfListOfShape anIt(aBndShapes); anIt.More(); anIt.Next()) {
        GeomShapePtr aShape(new GeomAPI_Shape());
        aShape->setImpl(new TopoDS_Shape(anIt.Value()));
        fixOrientation(aShape);
        isFromFaceSet ? this->addFromShape(aShape) : this->addToShape(aShape);
      }
    }

    // Try to cut with base face. If it can not be done then keep result of cut with bounding plane.
    TopoDS_Shape aModifiedBaseShape;
    if(aShapeTypeToExp != TopAbs_FACE) {
      ListOfShape aList;
      GeomShapePtr aSh(new GeomAPI_Shape());
      aSh->setImpl(new TopoDS_Shape(aBaseShape));
      std::shared_ptr<GeomAPI_Pnt> theCenter(new GeomAPI_Pnt(aBasePlane->Location().X(),
                                                             aBasePlane->Location().Y(),
                                                             aBasePlane->Location().Z()));
      std::shared_ptr<GeomAPI_Dir> theNormal(new GeomAPI_Dir(aBasePlane->Axis().Direction().X(),
                                                             aBasePlane->Axis().Direction().Y(),
                                                             aBasePlane->Axis().Direction().Z()));
      GeomShapePtr aPln = GeomAlgoAPI_FaceBuilder::planarFace(theCenter, theNormal);
      aList.push_back(aSh);
      std::list<std::shared_ptr<GeomAPI_Pnt> >
        aBoundingPoints = GeomAlgoAPI_ShapeTools::getBoundingBox(aList);
      aSh = GeomAlgoAPI_ShapeTools::fitPlaneToBox(aPln, aBoundingPoints);
      aModifiedBaseShape = aSh->impl<TopoDS_Shape>();
    } else {
      aModifiedBaseShape = aBaseShape;
    }
    if(isFromFaceSet) {
      if(aModifiedBaseShape.ShapeType() == TopAbs_FACE) {
        aModifiedBaseShape.Orientation(TopAbs_REVERSED);
      } else {
        gp_Trsf aMirrorTrsf;
        aMirrorTrsf.SetMirror(aBasePlane->Position().Ax2());
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
      TopoDS_Iterator aCheckIt(aCutResult);
      if(aCheckIt.More()) {
        this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
          new GeomAlgoAPI_MakeShape(aBaseCutBuilder)));
        aResult = aCutResult;
        if(aResult.ShapeType() == TopAbs_COMPOUND) {
          aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
        }
        if(aShapeTypeToExp == TopAbs_FACE || aShapeTypeToExp == TopAbs_COMPOUND) {
          const TopTools_ListOfShape& aBsShapes = aBaseCutBuilder->Modified(aBoundingFace);
          for(TopTools_ListIteratorOfListOfShape anIt(aBsShapes); anIt.More(); anIt.Next()) {
            GeomShapePtr aShape(new GeomAPI_Shape());
            aShape->setImpl(new TopoDS_Shape(anIt.Value()));
            fixOrientation(aShape);
            isFromFaceSet ? this->addToShape(aShape) : this->addFromShape(aShape);
          }
        }
      }
    }

    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
      aGeomShape->setImpl(new TopoDS_Shape(aResult));
      ListOfShape aResults;
      aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                         GeomAPI_Shape::COMPSOLID,
                                                         aResults);
      aResult = aGeomShape->impl<TopoDS_Shape>();
    }

    // If after cut we got more than one solids then take
    // closest to the center of mass of the base face.
    aResult = findClosest(aResult, aBaseCentre);

    // Setting naming.
    if(aShapeTypeToExp == TopAbs_COMPOUND) {
      storeGenerationHistory(this, aResult, TopAbs_EDGE,
        aRotatedBoundingFace, aModifiedBaseShape, isFromFaceSet);
      storeGenerationHistory(this, aResult, TopAbs_FACE,
        aRotatedBoundingFace, aModifiedBaseShape, isFromFaceSet);
    } else {
      storeGenerationHistory(this, aResult, aShapeTypeToExp,
        aRotatedBoundingFace, aModifiedBaseShape, isFromFaceSet);
    }
  }

  // Setting result.
  if(aResult.IsNull()) {
    return;
  }
  aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  GeomShapePtr aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}

//==================================================================================================
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

//==================================================================================================
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

//================================================================================================
gp_Pnt centreOfMass(const TopoDS_Shape& theShape)
{
  TopAbs_ShapeEnum aShType = theShape.ShapeType();
  GProp_GProps aGProps;

  if(aShType == TopAbs_EDGE || aShType == TopAbs_WIRE) {
    BRepGProp::LinearProperties(theShape, aGProps);
  } else if(aShType == TopAbs_FACE || aShType == TopAbs_SHELL) {
    BRepGProp::SurfaceProperties(theShape, aGProps);
  } else if(aShType == TopAbs_SOLID || aShType == TopAbs_COMPSOLID) {
    BRepGProp::VolumeProperties(theShape, aGProps);
  }

  return aGProps.CentreOfMass();
}

//================================================================================================
TopoDS_Shape findClosest(const TopoDS_Shape& theShape, const gp_Pnt& thePoint)
{
  TopoDS_Shape aResult = theShape;

  if(theShape.ShapeType() == TopAbs_COMPOUND) {
    double aMinDistance = Precision::Infinite();
    double aCurDistance;
    gp_Pnt aCentr;
    for (TopoDS_Iterator anItr(theShape); anItr.More(); anItr.Next()) {
      TopoDS_Shape aValue = anItr.Value();
      aCentr = centreOfMass(aValue);
      aCurDistance = aCentr.Distance(thePoint);

      if(aCurDistance < aMinDistance) {
        aMinDistance = aCurDistance;
        aResult = aValue;
      }
    }
  }

  return aResult;
}

//================================================================================================
void storeGenerationHistory(GeomAlgoAPI_Revolution* theRevolutionAlgo,
                            const TopoDS_Shape& theBase,
                            const TopAbs_ShapeEnum theType,
                            BRepPrimAPI_MakeRevol* theRevolBuilder)
{
  for(TopExp_Explorer anExp(theBase, theType); anExp.More(); anExp.Next()) {
    const TopoDS_Shape& aShape = anExp.Current();
    GeomShapePtr aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
    aFromShape->setImpl(new TopoDS_Shape(theRevolBuilder->FirstShape(aShape)));
    aToShape->setImpl(new TopoDS_Shape(theRevolBuilder->LastShape(aShape)));
    theRevolutionAlgo->fixOrientation(aFromShape);
    theRevolutionAlgo->fixOrientation(aToShape);
    theRevolutionAlgo->addFromShape(aFromShape);
    theRevolutionAlgo->addToShape(aToShape);
  }
}

//================================================================================================
void storeGenerationHistory(GeomAlgoAPI_Revolution* theRevolutionAlgo,
                            const TopoDS_Shape& theResult,
                            const TopAbs_ShapeEnum theType,
                            const TopoDS_Shape& theToFace,
                            const TopoDS_Shape& theFromFace)
{
  for(TopExp_Explorer anExp(theResult, theType); anExp.More (); anExp.Next ()) {
    const TopoDS_Shape& aShape = anExp.Current();
    GeomShapePtr aGeomSh(new GeomAPI_Shape());
    if(theType == TopAbs_VERTEX) {
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
      IntTools_Context anIntTools;
      if(anIntTools.IsValidPointForFace(aPnt, TopoDS::Face(theToFace),
          Precision::Confusion()) == Standard_True) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theRevolutionAlgo->addToShape(aGeomSh);
      }
      if(anIntTools.IsValidPointForFace(aPnt, TopoDS::Face(theFromFace),
          Precision::Confusion()) == Standard_True) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theRevolutionAlgo->addFromShape(aGeomSh);
      }
    } else if(theType == TopAbs_EDGE) {
      TopoDS_Edge anEdge = TopoDS::Edge(aShape);
      BRepLib_CheckCurveOnSurface anEdgeCheck(anEdge, TopoDS::Face(theToFace));
      anEdgeCheck.Perform();
      if(anEdgeCheck.MaxDistance() < Precision::Confusion()) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theRevolutionAlgo->addToShape(aGeomSh);
      }
      anEdgeCheck.Init(anEdge, TopoDS::Face(theFromFace));
      anEdgeCheck.Perform();
      if(anEdgeCheck.MaxDistance() < Precision::Confusion()) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theRevolutionAlgo->addFromShape(aGeomSh);
      }
    } else {
      Handle(Geom_Surface) aFaceSurface = BRep_Tool::Surface(TopoDS::Face(aShape));
      Handle(Geom_Surface) aFromSurface = BRep_Tool::Surface(TopoDS::Face(theFromFace));
      Handle(Geom_Surface) aToSurface = BRep_Tool::Surface(TopoDS::Face(theToFace));
      if(aFaceSurface == aFromSurface) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theRevolutionAlgo->addFromShape(aGeomSh);
      }
      if(aFaceSurface == aToSurface) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theRevolutionAlgo->addToShape(aGeomSh);
      }
    }
  }
}

void storeGenerationHistory(GeomAlgoAPI_Revolution* theRevolutionAlgo,
                            const TopoDS_Shape& theResult,
                            const TopAbs_ShapeEnum theType,
                            const TopoDS_Shape& theRotatedBoundingFace,
                            const TopoDS_Shape& theModifiedBaseShape,
                            const bool theIsFromFaceSet)
{
  for(TopExp_Explorer anExp(theResult, theType); anExp.More (); anExp.Next ()) {
    const TopoDS_Shape& aShape = anExp.Current();
    GeomShapePtr aGeomSh(new GeomAPI_Shape());
    if(theType == TopAbs_VERTEX) {
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
      IntTools_Context anIntTools;
      if(anIntTools.IsValidPointForFace(aPnt, TopoDS::Face(theRotatedBoundingFace),
          Precision::Confusion()) == Standard_True) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theIsFromFaceSet ? theRevolutionAlgo->addFromShape(aGeomSh) :
                           theRevolutionAlgo->addToShape(aGeomSh);
      }
      if(anIntTools.IsValidPointForFace(aPnt, TopoDS::Face(theModifiedBaseShape),
         Precision::Confusion()) == Standard_True) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theIsFromFaceSet ? theRevolutionAlgo->addToShape(aGeomSh) :
                           theRevolutionAlgo->addFromShape(aGeomSh);
      }
    } else if(theType == TopAbs_EDGE) {
      TopoDS_Edge anEdge = TopoDS::Edge(aShape);
      BRepLib_CheckCurveOnSurface anEdgeCheck(anEdge, TopoDS::Face(theRotatedBoundingFace));
      anEdgeCheck.Perform();
      if(anEdgeCheck.MaxDistance() < Precision::Confusion()) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theIsFromFaceSet ? theRevolutionAlgo->addFromShape(aGeomSh) :
                           theRevolutionAlgo->addToShape(aGeomSh);
      }
      anEdgeCheck.Init(anEdge, TopoDS::Face(theModifiedBaseShape));
      anEdgeCheck.Perform();
      if(anEdgeCheck.MaxDistance() < Precision::Confusion()) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theIsFromFaceSet ? theRevolutionAlgo->addToShape(aGeomSh) :
                           theRevolutionAlgo->addFromShape(aGeomSh);
      }
    } else {
      Handle(Geom_Surface) aFaceSurface = BRep_Tool::Surface(TopoDS::Face(aShape));
      Handle(Geom_Surface) aBoundingSurface =
        BRep_Tool::Surface(TopoDS::Face(theRotatedBoundingFace));
      Handle(Geom_Surface) aBaseSurface = BRep_Tool::Surface(TopoDS::Face(theModifiedBaseShape));
      if(aFaceSurface == aBoundingSurface) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theIsFromFaceSet ? theRevolutionAlgo->addFromShape(aGeomSh) :
                           theRevolutionAlgo->addToShape(aGeomSh);
      }
      if(aFaceSurface == aBaseSurface) {
        aGeomSh->setImpl(new TopoDS_Shape(aShape));
        theRevolutionAlgo->fixOrientation(aGeomSh);
        theIsFromFaceSet ? theRevolutionAlgo->addToShape(aGeomSh) :
                           theRevolutionAlgo->addFromShape(aGeomSh);
      }
    }
  }
}
