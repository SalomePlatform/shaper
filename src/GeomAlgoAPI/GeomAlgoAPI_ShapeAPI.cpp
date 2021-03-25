// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_ShapeAPI.h"

#include <GeomAlgoAPI_Box.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Cone.h>
#include <GeomAlgoAPI_ConeSegment.h>
#include <GeomAlgoAPI_Cylinder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_Rotation.h>
#include <GeomAlgoAPI_Scale.h>
#include <GeomAlgoAPI_Sphere.h>
#include <GeomAlgoAPI_Symmetry.h>
#include <GeomAlgoAPI_Torus.h>
#include <GeomAlgoAPI_Translation.h>

#include <GeomAPI_Lin.h>

#include <math.h>

static GeomShapePtr runAlgo(GeomAlgoAPI_MakeShape& theAlgo)
{
  if (!theAlgo.check())
    throw GeomAlgoAPI_Exception(theAlgo.getError());

  theAlgo.build();

  if (!theAlgo.isDone())
    throw GeomAlgoAPI_Exception(theAlgo.getError());

  return theAlgo.shape();
}

static GeomShapePtr runAlgoAndCheckShape(GeomAlgoAPI_MakeShape& theAlgo, const std::string& theMsg)
{
  if (!theAlgo.check())
    throw GeomAlgoAPI_Exception(theAlgo.getError());

  theAlgo.build();

  if (!theAlgo.isDone() || !theAlgo.checkValid(theMsg))
    throw GeomAlgoAPI_Exception(theAlgo.getError());

  return theAlgo.shape();
}

namespace GeomAlgoAPI_ShapeAPI
{
  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeBox(
    const double theDx, const double theDy,
    const double theDz)
  {
    static const std::string aMsg("Box builder with dimensions");
    GeomAlgoAPI_Box aBoxAlgo(theDx,theDy,theDz);
    return runAlgoAndCheckShape(aBoxAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeBox(
    std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
    std::shared_ptr<GeomAPI_Pnt> theSecondPoint)
  {
    static const std::string aMsg("Box builder with two points");
    GeomAlgoAPI_Box aBoxAlgo(theFirstPoint, theSecondPoint);
    return runAlgoAndCheckShape(aBoxAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCylinder(
    std::shared_ptr<GeomAPI_Pnt> theBasePoint, std::shared_ptr<GeomAPI_Edge> theEdge,
    double theRadius, double theHeight)
  {
    // Check if the base point is OK
    if (!theBasePoint) {
      throw GeomAlgoAPI_Exception("Cylinder builder :: the base point is not valid.");
    }
    // Check if the edge is OK
    if (!theEdge) {
      throw GeomAlgoAPI_Exception("Cylinder builder :: the axis is not valid.");
    }

    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(theBasePoint,
                                                          theEdge->line()->direction()));

    GeomAlgoAPI_Cylinder aCylinderAlgo(anAxis, theRadius, theHeight);

    static const std::string aMsg("Cylinder builder");
    return runAlgoAndCheckShape(aCylinderAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCylinder(
    std::shared_ptr<GeomAPI_Pnt> theBasePoint, std::shared_ptr<GeomAPI_Edge> theEdge,
    double theRadius, double theHeight, double theAngle)
  {
    // Check if the base point is OK
    if (!theBasePoint) {
      throw GeomAlgoAPI_Exception("Cylinder builder :: the base point is not valid.");
    }
    // Check if the edge is OK
    if (!theEdge) {
      throw GeomAlgoAPI_Exception("Cylinder builder :: the axis is not valid.");
    }

    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(theBasePoint,
                                                          theEdge->line()->direction()));

    GeomAlgoAPI_Cylinder aCylinderAlgo(anAxis, theRadius, theHeight, theAngle);

    static const std::string aMsg("Cylinder portion builder");
    return runAlgoAndCheckShape(aCylinderAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCylinder(
    double theRadius, double theHeight)
  {
    std::shared_ptr<GeomAPI_Pnt> aBasePoint =
      std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(0.,0.,0.));
    std::shared_ptr<GeomAPI_Edge> aEdge = GeomAlgoAPI_EdgeBuilder::line(0., 0., 1.);
    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(aBasePoint,
                                                          aEdge->line()->direction()));

    GeomAlgoAPI_Cylinder aCylinderAlgo(anAxis, theRadius, theHeight);

    static const std::string aMsg("Cylinder builder");
    return runAlgoAndCheckShape(aCylinderAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCylinder(
    double theRadius, double theHeight, double theAngle)
  {
    std::shared_ptr<GeomAPI_Pnt> aBasePoint =
      std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(0.,0.,0.));
    std::shared_ptr<GeomAPI_Edge> aEdge = GeomAlgoAPI_EdgeBuilder::line(0., 0., 1.);
    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(aBasePoint,
                                                          aEdge->line()->direction()));

    GeomAlgoAPI_Cylinder aCylinderAlgo(anAxis, theRadius, theHeight, theAngle);

    static const std::string aMsg("Cylinder portion builder");
    return runAlgoAndCheckShape(aCylinderAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSphere(
      std::shared_ptr<GeomAPI_Pnt> theCenterPoint, double theRadius)
  {
    static const std::string aMsg("Sphere builder");
    GeomAlgoAPI_Sphere aSphereAlgo(theCenterPoint, theRadius);
    return runAlgoAndCheckShape(aSphereAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSphere(double theRadius)
  {
    std::shared_ptr<GeomAPI_Pnt> aCenterPoint =
      std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(0.,0.,0.));

    GeomAlgoAPI_Sphere aSphereAlgo(aCenterPoint, theRadius);

    static const std::string aMsg("Sphere builder");
    return runAlgoAndCheckShape(aSphereAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTorus(
      std::shared_ptr<GeomAPI_Pnt> theBasePoint,
      std::shared_ptr<GeomAPI_Edge> theEdge,double theRadius, double theRingRadius)
  {
    // Check if the base point is OK
    if (!theBasePoint) {
      throw GeomAlgoAPI_Exception("Torus builder :: the base point is not valid.");
    }
    // Check if the edge is OK
    if (!theEdge) {
      throw GeomAlgoAPI_Exception("Torus builder :: the axis is not valid.");
    }

    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(theBasePoint,
                                                          theEdge->line()->direction()));

    GeomAlgoAPI_Torus aTorusAlgo(anAxis, theRadius, theRingRadius);

    static const std::string aMsg("Torus builder");
    return runAlgoAndCheckShape(aTorusAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTorus(double theRadius,
      double theRingRadius)
  {
    std::shared_ptr<GeomAPI_Pnt> aBasePoint =
      std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(0.,0.,0.));
    std::shared_ptr<GeomAPI_Edge> aEdge = GeomAlgoAPI_EdgeBuilder::line(0., 0., 1.);
    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(aBasePoint,
                                                          aEdge->line()->direction()));

    GeomAlgoAPI_Torus aTorusAlgo(anAxis, theRadius, theRingRadius);

    static const std::string aMsg("Torus builder");
    return runAlgoAndCheckShape(aTorusAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCone(
      std::shared_ptr<GeomAPI_Pnt> theBasePoint,
      std::shared_ptr<GeomAPI_Edge> theEdge,
      double theBaseRadius, double theTopRadius,
      double theHeight)
  {
    // Check if the base point is OK
    if (!theBasePoint) {
      throw GeomAlgoAPI_Exception("Cone builder :: the base point is not valid.");
    }
    // Check if the edge is OK
    if (!theEdge) {
      throw GeomAlgoAPI_Exception("Cone builder :: the axis is not valid.");
    }

    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(theBasePoint,
                                                          theEdge->line()->direction()));

    GeomAlgoAPI_Cone aConeAlgo(anAxis, theBaseRadius, theTopRadius, theHeight);

    static const std::string aMsg("Cone builder");
    return runAlgoAndCheckShape(aConeAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeCone(
      double theBaseRadius, double theTopRadius,
      double theHeight)
  {
    std::shared_ptr<GeomAPI_Pnt> aBasePoint =
      std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(0.,0.,0.));
    std::shared_ptr<GeomAPI_Edge> aEdge = GeomAlgoAPI_EdgeBuilder::line(0., 0., 1.);
    std::shared_ptr<GeomAPI_Ax2> anAxis;
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(aBasePoint,
                                                          aEdge->line()->direction()));

    GeomAlgoAPI_Cone aConeAlgo(anAxis, theBaseRadius, theTopRadius, theHeight);

    static const std::string aMsg("Cone builder");
    return runAlgoAndCheckShape(aConeAlgo, aMsg);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1>   theAxis,
    const double theDistance)
  {
    GeomAlgoAPI_Translation aTranslationAlgo(theSourceShape, theAxis, theDistance);
    return runAlgo(aTranslationAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    const double theDx,
    const double theDy,
    const double theDz)
  {
    GeomAlgoAPI_Translation aTranslationAlgo(theSourceShape, theDx, theDy, theDz);
    return runAlgo(aTranslationAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
    std::shared_ptr<GeomAPI_Pnt>   theEndPoint)
  {
    GeomAlgoAPI_Translation aTranslationAlgo(theSourceShape, theStartPoint, theEndPoint);
    return runAlgo(aTranslationAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeRotation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1> theAxis,
    const double theAngle)
  {
    GeomAlgoAPI_Rotation aRotationAlgo(theSourceShape, theAxis, theAngle);
    return runAlgo(aRotationAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeRotation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
    std::shared_ptr<GeomAPI_Pnt> theStartPoint,
    std::shared_ptr<GeomAPI_Pnt> theEndPoint)
  {
    GeomAlgoAPI_Rotation aRotationAlgo(theSourceShape, theCenterPoint, theStartPoint, theEndPoint);
    return runAlgo(aRotationAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSymmetry(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Pnt>   thePoint)
  {
    GeomAlgoAPI_Symmetry aSymmetryAlgo(theSourceShape, thePoint);
    return runAlgo(aSymmetryAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSymmetry(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1>   theAxis)
  {
    GeomAlgoAPI_Symmetry aSymmetryAlgo(theSourceShape, theAxis);
    return runAlgo(aSymmetryAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSymmetry(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax2>   thePlane)
  {
    GeomAlgoAPI_Symmetry aSymmetryAlgo(theSourceShape, thePlane);
    return runAlgo(aSymmetryAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeScale(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
    const double                   theScaleFactor)
  {
    GeomAlgoAPI_Scale aScaleAlgo(theSourceShape, theCenterPoint, theScaleFactor);
    return runAlgo(aScaleAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeScale(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
    const double                   theScaleFactorX,
    const double                   theScaleFactorY,
    const double                   theScaleFactorZ)
  {
    GeomAlgoAPI_Scale aScaleAlgo(theSourceShape, theCenterPoint,
                                 theScaleFactorX, theScaleFactorY, theScaleFactorZ);
    return runAlgo(aScaleAlgo);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeMultiTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1> theAxis,
    const double theStep,
    const int theNumber)
  {
    if (!theAxis) {
      std::string aError = "Multitranslation builder ";
      aError+=":: the first axis is not valid";
      throw GeomAlgoAPI_Exception(aError);
    }

    if (theNumber <=0) {
      std::string aError = "Multitranslation builder ";
      aError+=":: the number of copies for the first direction is null or negative.";
      throw GeomAlgoAPI_Exception(aError);
    }

    ListOfShape aListOfShape;
    for (int i=0; i<theNumber; i++) {
      aListOfShape.
        push_back(GeomAlgoAPI_ShapeAPI::makeTranslation(theSourceShape, theAxis, i*theStep));
    }
    return GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeMultiTranslation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1> theFirstAxis,
    const double theFirstStep,
    const int theFirstNumber,
    std::shared_ptr<GeomAPI_Ax1> theSecondAxis,
    const double theSecondStep,
    const int theSecondNumber)
  {
    if (!theFirstAxis) {
      std::string aError = "Multitranslation builder ";
      aError+=":: the first axis is not valid";
      throw GeomAlgoAPI_Exception(aError);
    }

    if (!theSecondAxis) {
      std::string aError = "Multitranslation builder ";
      aError+=":: the second axis is not valid";
      throw GeomAlgoAPI_Exception(aError);
    }

    if (theFirstNumber <=0) {
      std::string aError = "Multitranslation builder ";
      aError+=":: the number of copies for the first direction is null or negative.";
      throw GeomAlgoAPI_Exception(aError);
    }

    if (theSecondNumber <=0) {
      std::string aError = "Multitranslation builder ";
      aError+=":: the number of copies for the second direction is null or negative.";
      throw GeomAlgoAPI_Exception(aError);
    }

    // Coord theFirstAxis
    double x1 = theFirstAxis->dir()->x();
    double y1 = theFirstAxis->dir()->y();
    double z1 = theFirstAxis->dir()->z();
    double norm1 = sqrt(x1*x1 + y1*y1 + z1*z1);

    // Coord theSecondAxis
    double x2 = theSecondAxis->dir()->x();
    double y2 = theSecondAxis->dir()->y();
    double z2 = theSecondAxis->dir()->z();
    double norm2 = sqrt(x2*x2 + y2*y2 + z2*z2);

    ListOfShape aListOfShape;
    for (int j=0; j<theSecondStep; j++) {
      for (int i=0; i<theFirstNumber; i++) {
        double dx = i*theFirstStep*x1/norm1+j*theSecondStep*x2/norm2;
        double dy = i*theFirstStep*y1/norm1+j*theSecondStep*y2/norm2;
        double dz = i*theFirstStep*z1/norm1+j*theSecondStep*z2/norm2;
        aListOfShape.push_back(GeomAlgoAPI_ShapeAPI::makeTranslation(theSourceShape, dx, dy, dz));
      }
    }
    return GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeMultiRotation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1> theAxis,
    const int theNumber)
  {
    if (!theAxis) {
      std::string aError = "Multirotation builder ";
      aError+=":: the axis is not valid";
      throw GeomAlgoAPI_Exception(aError);
    }

    if (theNumber <=0) {
      std::string aError = "Multirotation builder ";
      aError+=":: the number of copies is null or negative.";
      throw GeomAlgoAPI_Exception(aError);
    }

    double anAngle = 360./theNumber;

    ListOfShape aListOfShape;
    for (int i=0; i<theNumber; i++) {
      aListOfShape.
        push_back(GeomAlgoAPI_ShapeAPI::makeRotation(theSourceShape, theAxis, i*anAngle));
    }
    return GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeMultiRotation(
    std::shared_ptr<GeomAPI_Shape> theSourceShape,
    std::shared_ptr<GeomAPI_Ax1> theAxis,
    const double theStep,
    const int theNumber)
  {
    if (!theAxis) {
      std::string aError = "Multirotation builder ";
      aError+=":: the axis is not valid";
      throw GeomAlgoAPI_Exception(aError);
    }

    if (theNumber <=0) {
      std::string aError = "Multirotation builder ";
      aError+=":: the number of copies is null or negative.";
      throw GeomAlgoAPI_Exception(aError);
    }

    ListOfShape aListOfShape;
    for (int i=0; i<theNumber; i++) {
      aListOfShape.
        push_back(GeomAlgoAPI_ShapeAPI::makeRotation(theSourceShape, theAxis, i*theStep));
    }
    return GeomAlgoAPI_CompoundBuilder::compound(aListOfShape);
  }

  //===============================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeConeSegment(
    const double theRMin1, const double theRMax1,
    const double theRMin2, const double theRMax2,
    const double theZ,
    const double theStartPhi, const double theDeltaPhi)
  {
    GeomAlgoAPI_ConeSegment aConeSegmentAlgo(theRMin1, theRMax1, theRMin2, theRMax2,
                                             theZ, theStartPhi, theDeltaPhi);

    static const std::string aMsg("Cone Segment builder");
    return runAlgoAndCheckShape(aConeSegmentAlgo, aMsg);
  }
}
