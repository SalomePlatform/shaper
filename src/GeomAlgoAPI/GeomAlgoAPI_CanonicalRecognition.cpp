// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_CanonicalRecognition.h"

#include <Standard_Version.hxx>
// code from KERNEL_SRC/src/Basics/Basics_OCCTVersion.hxx
#ifdef OCC_VERSION_SERVICEPACK
#  define OCC_VERSION_LARGE (OCC_VERSION_MAJOR << 24 | OCC_VERSION_MINOR << 16 | OCC_VERSION_MAINTENANCE << 8 | OCC_VERSION_SERVICEPACK)
#else
#  ifdef OCC_VERSION_DEVELOPMENT
#    define OCC_VERSION_LARGE ((OCC_VERSION_MAJOR << 24 | OCC_VERSION_MINOR << 16 | OCC_VERSION_MAINTENANCE << 8)-1)
#  else
#    define OCC_VERSION_LARGE (OCC_VERSION_MAJOR << 24 | OCC_VERSION_MINOR << 16 | OCC_VERSION_MAINTENANCE << 8)
#  endif
#endif

#if OCC_VERSION_LARGE > 0x07050303
#include <ShapeAnalysis_CanonicalRecognition.hxx>
#endif

#include <TopoDS_Shape.hxx>

#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>

static bool isValidDirection(const std::vector<double>& theDir)
{
  return (theDir.size() == 3) && (gp_Vec(theDir[0], theDir[1], theDir[2]).Magnitude() > 0.);
}

bool GeomAlgoAPI_CanonicalRecognition::isPlane(const GeomShapePtr& theShape, double theTolerance,
  std::vector<double>& theNormal, std::vector<double>& theOrigin)
{
  if (!theShape.get())
    return false;

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;

  bool aIsValidNormal = isValidDirection(theNormal);
  bool aIsValidOrigin = theOrigin.size() == 3;
  gp_Pln aPln;
  if (aIsValidNormal && aIsValidOrigin) {
    aPln = gp_Pln(gp_Pnt(theOrigin[0], theOrigin[1], theOrigin[2]),
      gp_Dir(theNormal[0], theNormal[1], theNormal[2]));
  }

  bool aResult = false;

#if OCC_VERSION_LARGE > 0x07050303
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  try {
    if (aRecognition.GetStatus() == 0)
      aResult = aRecognition.IsPlane(theTolerance, aPln);
  }
  catch (...) {
    return false;
  }
#endif

  gp_Pnt aOrig = aPln.Location();
  if (theOrigin.size() != 3)
    theOrigin.resize(3);
  theOrigin[0] = aOrig.X();
  theOrigin[1] = aOrig.Y();
  theOrigin[2] = aOrig.Z();

  gp_Dir aNorm = aPln.Axis().Direction();
  if (theNormal.size() != 3)
    theNormal.resize(3);
  theNormal[0] = aNorm.X();
  theNormal[1] = aNorm.Y();
  theNormal[2] = aNorm.Z();

  return aResult;
}

bool GeomAlgoAPI_CanonicalRecognition::isSphere(const GeomShapePtr& theShape, double theTolerance,
  std::vector<double>& theOrigin, double& theRadius)
{
  if (!theShape.get())
    return false;

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;

  bool aIsValidOrigin = theOrigin.size() == 3;
  bool aIsValidRadius = theRadius > 0;
  gp_Sphere aSphere;
  if (aIsValidOrigin && aIsValidRadius)
  {
    aSphere.SetLocation(gp_Pnt(theOrigin[0], theOrigin[1], theOrigin[2]));
    aSphere.SetRadius(theRadius);
  }
  else
    aSphere.SetRadius(1.0);

  bool aResult = false;

#if OCC_VERSION_LARGE > 0x07050303
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  try {
    if (aRecognition.GetStatus() == 0)
      aResult = aRecognition.IsSphere(theTolerance, aSphere);
  }
  catch (...) {
    return false;
  }
#endif

  gp_Pnt aLoc = aSphere.Location();
  if (theOrigin.size() != 3)
    theOrigin.resize(3);
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();
  theRadius = aSphere.Radius();

  return aResult;
}

bool GeomAlgoAPI_CanonicalRecognition::isCone(const GeomShapePtr& theShape, double theTolerance,
  std::vector<double>& theAxis, std::vector<double>& theApex,
  double& theHalfAngle)
{
  if (!theShape.get())
    return false;

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;

  bool aIsValidAxis = isValidDirection(theAxis);
  bool aIsValidApex = theApex.size() == 3;
  bool aIsValidAngle = theHalfAngle > 0;
  gp_Cone aCone;
  if (aIsValidAxis && aIsValidApex && aIsValidAngle)
  {
    gp_Pnt aLoc(theApex[0], theApex[1], theApex[2]);
    gp_Ax3 aAx3(aLoc, gp_Dir(theAxis[0], theAxis[1], theAxis[2]));
    aCone.SetPosition(aAx3);
  }
  else
    aCone.SetRadius(1.0);

  bool aResult = false;

#if OCC_VERSION_LARGE > 0x07050303
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  try {
    if (aRecognition.GetStatus() == 0)
      aResult = aRecognition.IsCone(theTolerance, aCone);
  }
  catch (...) {
    return false;
  }
#endif

  gp_Dir aDir = aCone.Axis().Direction();
  if (theAxis.size() != 3)
    theAxis.resize(3);
  theAxis[0] = aDir.X();
  theAxis[1] = aDir.Y();
  theAxis[2] = aDir.Z();

  gp_Pnt aApex = aCone.Apex();
  if (theApex.size() != 3)
    theApex.resize(3);
  theApex[0] = aApex.X();
  theApex[1] = aApex.Y();
  theApex[2] = aApex.Z();

  theHalfAngle = aCone.SemiAngle();

  return aResult;
}

bool GeomAlgoAPI_CanonicalRecognition::isCylinder(const GeomShapePtr& theShape, double theTolerance,
  std::vector<double>& theAxis, std::vector<double>& theOrigin,
  double& theRadius)
{
  if (!theShape.get())
    return false;

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;

  bool aIsValidAxis = isValidDirection(theAxis);
  bool aIsValidOrigin = theOrigin.size() == 3;
  bool aIsValidRadius = theRadius > 0;
  gp_Cylinder aCylinder;
  if (aIsValidAxis && aIsValidOrigin && aIsValidRadius)
  {
    gp_Pnt aLoc(theOrigin[0], theOrigin[0], theOrigin[0]);
    gp_Ax3 aAx3(aLoc, gp_Dir(theAxis[0], theAxis[1], theAxis[2]));
    aCylinder.SetPosition(aAx3);
    aCylinder.SetRadius(theRadius);
  }
  else
    aCylinder.SetRadius(1.0);

  bool aResult = false;

#if OCC_VERSION_LARGE > 0x07050303
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  try {
    if (aRecognition.GetStatus() == 0)
      aResult = aRecognition.IsCylinder(theTolerance, aCylinder);
  }
  catch (...) {
    return false;
  }
#endif

  gp_Dir aDir = aCylinder.Axis().Direction();
  if (theAxis.size() != 3)
    theAxis.resize(3);
  theAxis[0] = aDir.X();
  theAxis[1] = aDir.Y();
  theAxis[2] = aDir.Z();

  gp_Pnt aLoc = aCylinder.Location();
  if (theOrigin.size() != 3)
    theOrigin.resize(3);
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();

  theRadius = aCylinder.Radius();

  return aResult;
}

bool GeomAlgoAPI_CanonicalRecognition::isLine(const GeomShapePtr& theEdge, double theTolerance,
  std::vector<double>& theDir, std::vector<double>& theOrigin)
{
  if (!theEdge.get())
    return false;

  const TopoDS_Shape& aShape = theEdge->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;

  bool aIsValidDir = isValidDirection(theDir);
  bool aIsValidOrigin = theOrigin.size() == 3;
  gp_Lin aLine;
  if (aIsValidDir && aIsValidOrigin)
  {
    aLine.SetLocation(gp_Pnt(theOrigin[0], theOrigin[1], theOrigin[2]));
    aLine.SetDirection(gp_Dir(theDir[0], theDir[1], theDir[2]));
  }

  bool aResult = false;

#if OCC_VERSION_LARGE > 0x07050303
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  try {
    if (aRecognition.GetStatus() == 0)
      aResult = aRecognition.IsLine(theTolerance, aLine);
  }
  catch (...) {
    return false;
  }
#endif

  gp_Pnt aLoc = aLine.Location();
  if (theOrigin.size() != 3)
    theOrigin.resize(3);
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();

  gp_Dir aDir = aLine.Direction();
  if (theDir.size() != 3)
    theDir.resize(3);
  theDir[0] = aDir.X();
  theDir[1] = aDir.Y();
  theDir[2] = aDir.Z();

  return aResult;
}

bool GeomAlgoAPI_CanonicalRecognition::isCircle(const GeomShapePtr& theEdge, double theTolerance,
  std::vector<double>& theNormal, std::vector<double>& theOrigin,
  double& theRadius)
{
  if (!theEdge.get())
    return false;

  const TopoDS_Shape& aShape = theEdge->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;

  bool aIsValidNormal = isValidDirection(theNormal);
  bool aIsValidOrigin = theOrigin.size() == 3;
  bool aIsValidRadius = theRadius > 0;
  gp_Circ aCircle;
  if (aIsValidNormal && aIsValidOrigin && aIsValidRadius)
  {
    gp_Ax2 aAx2(gp_Pnt(theOrigin[0], theOrigin[1], theOrigin[2]),
      gp_Dir(theNormal[0], theNormal[1], theNormal[2]));
    aCircle.SetPosition(aAx2);
    aCircle.SetRadius(theRadius);
  }
  else
    aCircle.SetRadius(1.0);

  bool aResult = false;

#if OCC_VERSION_LARGE > 0x07050303
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  try {
    if (aRecognition.GetStatus() == 0)
      aResult = aRecognition.IsCircle(theTolerance, aCircle);
  }
  catch (...) {
    return false;
  }
#endif

  gp_Pnt aLoc = aCircle.Location();
  if (theOrigin.size() != 3)
    theOrigin.resize(3);
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();

  gp_Dir aDir = aCircle.Axis().Direction();
  if (theNormal.size() != 3)
    theNormal.resize(3);
  theNormal[0] = aDir.X();
  theNormal[1] = aDir.Y();
  theNormal[2] = aDir.Z();
  theRadius = aCircle.Radius();
  return aResult;
}

bool GeomAlgoAPI_CanonicalRecognition::isEllipse(const GeomShapePtr& theEdge, double theTolerance,
  std::vector<double>& theNormal, std::vector<double>& theDirX,
  std::vector<double>& theOrigin,
  double& theMajorRadius, double& theMinorRadius)
{
  if (!theEdge.get())
    return false;

  const TopoDS_Shape& aShape = theEdge->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;

  bool aIsValidNormal = isValidDirection(theNormal);
  bool aIsValidOrigin = theOrigin.size() == 3;
  bool aIsValidDirX = isValidDirection(theDirX);
  bool aIsValidRad1 = (theMajorRadius > 0) && (theMajorRadius > theMinorRadius);
  bool aIsValidRad2 = (theMinorRadius > 0) && (theMajorRadius > theMinorRadius);

  gp_Elips aElips;
  if (aIsValidNormal && aIsValidOrigin && aIsValidDirX && aIsValidRad1 && aIsValidRad2)
  {
    gp_Pnt anOrigin(theOrigin[0], theOrigin[1], theOrigin[2]);
    gp_XYZ aNormal(theNormal[0], theNormal[1], theNormal[2]);
    gp_XYZ aDirX(theDirX[0], theDirX[1], theDirX[2]);
    Standard_Boolean isCollinear =
        aNormal.CrossSquareMagnitude(aDirX) < Precision::SquareConfusion();
    gp_Ax2 aAx2 = isCollinear ? gp_Ax2(anOrigin, aNormal) : gp_Ax2(anOrigin, aNormal, aDirX);
    aElips = gp_Elips(aAx2, theMajorRadius, theMinorRadius);
  }
  else
    aElips.SetMajorRadius(1.0);

  bool aResult = false;

#if OCC_VERSION_LARGE > 0x07050303
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  try {
    if (aRecognition.GetStatus() == 0)
      aResult = aRecognition.IsEllipse(theTolerance, aElips);
  }
  catch (...) {
    return false;
  }
#endif

  gp_Pnt aLoc = aElips.Position().Location();
  if (theOrigin.size() != 3)
    theOrigin.resize(3);
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();

  gp_Dir aNorm = aElips.Position().Direction();
  if (theNormal.size() != 3)
    theNormal.resize(3);
  theNormal[0] = aNorm.X();
  theNormal[1] = aNorm.Y();
  theNormal[2] = aNorm.Z();

  gp_Dir aDirX = aElips.Position().XDirection();
  if (theDirX.size() != 3)
    theDirX.resize(3);
  theDirX[0] = aDirX.X();
  theDirX[1] = aDirX.Y();
  theDirX[2] = aDirX.Z();

  theMajorRadius = aElips.MajorRadius();
  theMinorRadius = aElips.MinorRadius();

  return aResult;
}

bool GeomAlgoAPI_CanonicalRecognition::isImplemented()
{
#if OCC_VERSION_LARGE > 0x07050303
  return true;
#else
  return false;
#endif
}
