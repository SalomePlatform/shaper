// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeAPI.cpp
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#include "GeomAlgoAPI_ShapeAPI.h"

#include <GeomAlgoAPI_Box.h>
#include <GeomAlgoAPI_Cylinder.h>
#include <GeomAlgoAPI_ConeSegment.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_Symmetry.h>
#include <GeomAlgoAPI_Translation.h>

#include <GeomAPI_Lin.h>

namespace GeomAlgoAPI_ShapeAPI
{
  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeBox(
    const double theDx, const double theDy,
    const double theDz) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Box aBoxAlgo(theDx,theDy,theDz);

    if (!aBoxAlgo.check()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }

    aBoxAlgo.build();

    if(!aBoxAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    if (!aBoxAlgo.checkValid("Box builder with dimensions")) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    return aBoxAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeBox(
    std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
    std::shared_ptr<GeomAPI_Pnt> theSecondPoint) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Box aBoxAlgo(theFirstPoint, theSecondPoint);

    if (!aBoxAlgo.check()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }

    aBoxAlgo.build();

    if(!aBoxAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    if (!aBoxAlgo.checkValid("Box builder with two points")) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    return aBoxAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCylinder(
    std::shared_ptr<GeomAPI_Pnt> theBasePoint, std::shared_ptr<GeomAPI_Edge> theEdge,
    double theRadius, double theHeight) throw (GeomAlgoAPI_Exception)
  {
    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(theBasePoint,
                                                          theEdge->line()->direction()));

    GeomAlgoAPI_Cylinder aCylinderAlgo(anAxis, theRadius, theHeight);

    if (!aCylinderAlgo.check()) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }

    aCylinderAlgo.build();

    if(!aCylinderAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }
    if (!aCylinderAlgo.checkValid("Cylinder builder")) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }
    return aCylinderAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCylinder(
    std::shared_ptr<GeomAPI_Pnt> theBasePoint, std::shared_ptr<GeomAPI_Edge> theEdge,
    double theRadius, double theHeight, double theAngle) throw (GeomAlgoAPI_Exception)
  {
    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(theBasePoint,
                                                          theEdge->line()->direction()));

    GeomAlgoAPI_Cylinder aCylinderAlgo(anAxis, theRadius, theHeight, theAngle);

    if (!aCylinderAlgo.check()) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }

    aCylinderAlgo.build();

    if(!aCylinderAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }
    if (!aCylinderAlgo.checkValid("Cylinder portion builder")) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }
    return aCylinderAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCylinder(
    double theRadius, double theHeight) throw (GeomAlgoAPI_Exception)
  {
    std::shared_ptr<GeomAPI_Pnt> aBasePoint =
      std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(0.,0.,0.));
    std::shared_ptr<GeomAPI_Edge> aEdge = GeomAlgoAPI_EdgeBuilder::line(0., 0., 1.);
    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(aBasePoint,
                                                          aEdge->line()->direction()));

    GeomAlgoAPI_Cylinder aCylinderAlgo(anAxis, theRadius, theHeight);

    if (!aCylinderAlgo.check()) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }

    aCylinderAlgo.build();

    if(!aCylinderAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }
    if (!aCylinderAlgo.checkValid("Cylinder builder")) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }
    return aCylinderAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCylinder(
    double theRadius, double theHeight, double theAngle) throw (GeomAlgoAPI_Exception)
  {
    std::shared_ptr<GeomAPI_Pnt> aBasePoint =
      std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(0.,0.,0.));
    std::shared_ptr<GeomAPI_Edge> aEdge = GeomAlgoAPI_EdgeBuilder::line(0., 0., 1.);
    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(aBasePoint,
                                                          aEdge->line()->direction()));

    GeomAlgoAPI_Cylinder aCylinderAlgo(anAxis, theRadius, theHeight, theAngle);

    if (!aCylinderAlgo.check()) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }

    aCylinderAlgo.build();

    if(!aCylinderAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }
    if (!aCylinderAlgo.checkValid("Cylinder portion builder")) {
      throw GeomAlgoAPI_Exception(aCylinderAlgo.getError());
    }
    return aCylinderAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1>   theAxis,
    const double theDistance) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Translation aTranslationAlgo(theSourceShape, theAxis, theDistance);

    if (!aTranslationAlgo.check()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }

    aTranslationAlgo.build();

    if(!aTranslationAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    if (!aTranslationAlgo.checkValid("Translation builder with axis and distance")) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    return aTranslationAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    const double theDx,
    const double theDy,
    const double theDz) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Translation aTranslationAlgo(theSourceShape, theDx, theDy, theDz);

    if (!aTranslationAlgo.check()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }

    aTranslationAlgo.build();

    if(!aTranslationAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    if (!aTranslationAlgo.checkValid("Translation builder with dimensions")) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    return aTranslationAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
    std::shared_ptr<GeomAPI_Pnt>   theEndPoint) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Translation aTranslationAlgo(theSourceShape, theStartPoint, theEndPoint);

    if (!aTranslationAlgo.check()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }

    aTranslationAlgo.build();

    if(!aTranslationAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    if (!aTranslationAlgo.checkValid("Translation builder with two points")) {
      throw GeomAlgoAPI_Exception(aTranslationAlgo.getError());
    }
    return aTranslationAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSymmetry(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Pnt>   thePoint) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Symmetry aSymmetryAlgo(theSourceShape, thePoint);

    if (!aSymmetryAlgo.check()) {
      throw GeomAlgoAPI_Exception(aSymmetryAlgo.getError());
    }

    aSymmetryAlgo.build();

    if(!aSymmetryAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aSymmetryAlgo.getError());
    }
    if (!aSymmetryAlgo.checkValid("Symmetry builder by a point")) {
      throw GeomAlgoAPI_Exception(aSymmetryAlgo.getError());
    }
    return aSymmetryAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSymmetry(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1>   theAxis) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Symmetry aSymmetryAlgo(theSourceShape, theAxis);

    if (!aSymmetryAlgo.check()) {
      throw GeomAlgoAPI_Exception(aSymmetryAlgo.getError());
    }

    aSymmetryAlgo.build();

    if(!aSymmetryAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aSymmetryAlgo.getError());
    }
    if (!aSymmetryAlgo.checkValid("Symmetry builder by an axis")) {
      throw GeomAlgoAPI_Exception(aSymmetryAlgo.getError());
    }
    return aSymmetryAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSymmetry(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax2>   thePlane) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Symmetry aSymmetryAlgo(theSourceShape, thePlane);

    if (!aSymmetryAlgo.check()) {
      throw GeomAlgoAPI_Exception(aSymmetryAlgo.getError());
    }

    aSymmetryAlgo.build();

    if(!aSymmetryAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aSymmetryAlgo.getError());
    }
    if (!aSymmetryAlgo.checkValid("Symmetry builder by a plane")) {
      throw GeomAlgoAPI_Exception(aSymmetryAlgo.getError());
    }
    return aSymmetryAlgo.shape();
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeConeSegment(
    const double theRMin1, const double theRMax1,
    const double theRMin2, const double theRMax2,
    const double theZ,
    const double theStartPhi, const double theDeltaPhi) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_ConeSegment aConeSegmentAlgo(theRMin1, theRMax1, theRMin2, theRMax2,
                                             theZ, theStartPhi, theDeltaPhi);

    if (!aConeSegmentAlgo.check()) {
      throw GeomAlgoAPI_Exception(aConeSegmentAlgo.getError());
    }

    aConeSegmentAlgo.build();

    if(!aConeSegmentAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aConeSegmentAlgo.getError());
    }
    if (!aConeSegmentAlgo.checkValid("Cone Segment builder")) {
      throw GeomAlgoAPI_Exception(aConeSegmentAlgo.getError());
    }
    return aConeSegmentAlgo.shape();
  }
}
