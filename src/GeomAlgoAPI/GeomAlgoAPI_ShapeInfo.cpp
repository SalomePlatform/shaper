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

#include "GeomAlgoAPI_ShapeInfo.h"
#include "GeomAlgoAPI_BoundingBox.h"

#include <GeomAPI_Circ.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_Wire.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Sphere.h>
#include <GeomAPI_Cone.h>
#include <GeomAPI_Cylinder.h>
#include <GeomAPI_Torus.h>
#include <GeomAPI_Shell.h>
#include <GeomAPI_Box.h>

#include <Precision.hxx>

#include <sstream>

std::string GeomAlgoAPI_ShapeInfo::html(Translator* theTranslator)
{
  Values aVal(TYPE_HTML);
  if (theTranslator)
    aVal.setTranslator(theTranslator);
  processShape(aVal);
  return aVal.html();
}

std::list<GeomAlgoAPI_InfoValue> GeomAlgoAPI_ShapeInfo::values()
{
  Values aVal(TYPE_VALS);
  processShape(aVal);
  return aVal.values();
}

void GeomAlgoAPI_ShapeInfo::processShape(GeomAlgoAPI_ShapeInfo::Values& theVals)
{
  switch (myShape->shapeType()) {
  case GeomAPI_Shape::VERTEX:
  {
    GeomPointPtr aPoint = myShape->vertex()->point();
    setShapeType("Vertex");
    theVals.addPoint("Coordinates", myShape->vertex()->point());
    break;
  }
  case GeomAPI_Shape::EDGE:
  {
    GeomEdgePtr anEdge = myShape->edge();
    processEdge(theVals, anEdge);
    break;
  }
  case GeomAPI_Shape::FACE:
  {
    GeomFacePtr aFace = myShape->face();
    processFace(theVals, aFace);
    break;
  }
  case GeomAPI_Shape::SOLID: // nearly same as SHELL if it is only one sub
  {
    ListOfShape aShells = myShape->subShapes(GeomAPI_Shape::SHELL);
    if (aShells.size() == 1)
    {
      GeomShapePtr aTmpShape = myShape;
      myShape = aShells.front();
      processShape(theVals);
      myShape = aTmpShape;
    }
    if (myShapeType.empty() || myShapeType == "Shell")
      setShapeType("Solid");
    break;
  }
  case GeomAPI_Shape::SHELL:
  {
    GeomShellPtr aShell = myShape->shell();
    if (GeomBoxPtr aBox = aShell->getParallelepiped())
    {
      if (aBox->isAxesAligned()) {
        setShapeType("Box");
      }
      else {
        setShapeType("Rotated Box");
      }
    }
    else { // check single face
      ListOfShape aFaces = myShape->subShapes(GeomAPI_Shape::FACE);
      if (aFaces.size() == 1)
      {
        GeomFacePtr aFace = aFaces.front()->face();
        processFace(theVals, aFace);
      }
      if (myShapeType.empty() || myShapeType == "Face")
        setShapeType("Shell");
    }
    break;
  }
  case GeomAPI_Shape::WIRE:
  {
    theVals.addNamedValue("Closed", myShape->wire()->isClosed());

    // check the wire is a polygon
    std::list<GeomPointPtr> aPolygonPoints;
    if (myShape->wire()->isPolygon(aPolygonPoints)) {
      setShapeType("Polygon");
      std::string aPointTr = theVals.translator()->translate("Point");
      std::list<GeomPointPtr>::const_iterator aPtIt = aPolygonPoints.begin();
      for (int aCornerIndex = 0; aPtIt != aPolygonPoints.end(); ++aPtIt) {
        std::ostringstream aStr;
        aStr << " " << (++aCornerIndex);
        theVals.addPoint(aStr.str().c_str(), *aPtIt);
      }
    }
    else // Common case
      setShapeType("Wire");
    break;
  }
  case GeomAPI_Shape::COMPSOLID:
  case GeomAPI_Shape::COMPOUND:
  {
    setShapeType(myShape->shapeType() == GeomAPI_Shape::COMPSOLID ? "CompSolid" : "Compound");
    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    if (myShape->computeSize(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax))
    {
      theVals.addGroupName("Bounding box");
      GeomPointPtr aMinPoint(new GeomAPI_Pnt(aXmin, aYmin, aZmin));
      theVals.addPoint("Minimal corner", aMinPoint);
      GeomPointPtr aMaxPoint(new GeomAPI_Pnt(aXmax, aYmax, aZmax));
      theVals.addPoint("Maximal corner", aMaxPoint);
    }
    break;
  }
  default: // nothing to fill
    break;
  }
}

void GeomAlgoAPI_ShapeInfo::processEdge(Values& theVals, GeomEdgePtr theEdge)
{
  setShapeType("Edge"); // Common case
  if (theEdge->isDegenerated()) {
    theVals.addNamedValue("Degenerated", true);
    return;
  }

  GeomPointPtr aStartPnt = theEdge->firstPoint();
  GeomPointPtr aEndPnt = theEdge->lastPoint();
  bool addStartEndPoints = false;

  if (theEdge->isLine()) {
    setShapeType("Line segment");
    addStartEndPoints = true;
  }
  else if (GeomCirclePtr aCircle = theEdge->circle()) {
    addStartEndPoints = aStartPnt->distance(aEndPnt) >= Precision::Confusion();
    setShapeType(addStartEndPoints ? "Arc of circle" : "Circle");
    theVals.addPoint("Center", aCircle->center());
    theVals.addDirection("Normal", aCircle->normal());
    theVals.addGroupName("Dimensions");
    theVals.addNamedValue("Radius", aCircle->radius());
  }
  else if (GeomEllipsePtr anEllipse = theEdge->ellipse()) {
    addStartEndPoints = aStartPnt->distance(aEndPnt) >= Precision::Confusion();
    setShapeType(addStartEndPoints ? "Arc of ellipse" : "Ellipse");
    theVals.addPoint("Center", anEllipse->center());
    theVals.addDirection("Normal", anEllipse->normal());
    theVals.addGroupName("Dimensions");
    theVals.addNamedValue("Major radius", anEllipse->majorRadius());
    theVals.addNamedValue("Minor radius", anEllipse->minorRadius());
  }

  if (addStartEndPoints) {
    theVals.addPoint("Start point", aStartPnt);
    theVals.addPoint("End point", aEndPnt);
  }
}

void GeomAlgoAPI_ShapeInfo::processFace(Values& theVals, GeomFacePtr theFace)
{
  // Plane and planar face
  if (GeomPlanePtr aPlane = theFace->getPlane()) {
    bool isCommonCase = true;
    // Check face bounded by circle or ellipse
    std::list<GeomShapePtr> aSubs = theFace->subShapes(GeomAPI_Shape::EDGE);
    if (aSubs.size() == 1) {
      GeomEdgePtr anEdge = aSubs.front()->edge();
      if (anEdge->isCircle() || anEdge->isEllipse()) {
        processEdge(theVals, anEdge);
        isCommonCase = false;
        setShapeType("Disk");
      }
    }
    else {
      // Check face bounded by a single wire which is rectangle
      aSubs = theFace->subShapes(GeomAPI_Shape::WIRE);
      if (aSubs.size() == 1) {
        std::list<GeomPointPtr> aCorners;
        if (aSubs.front()->wire()->isRectangle(aCorners)) {
          GeomPointPtr aBaseCorner = aCorners.front();
          aCorners.pop_front();

          double aWidth = aBaseCorner->distance(aCorners.front());
          double aHeight = aBaseCorner->distance(aCorners.back());

          setShapeType("Rectangle");
          theVals.addPoint("Corner", aBaseCorner);
          theVals.addDirection("Normal", aPlane->direction());
          theVals.addGroupName("Dimensions");
          theVals.addNamedValue("Width", aWidth);
          theVals.addNamedValue("Height", aHeight);
          isCommonCase = false;
        }
      }
    }

    if (isCommonCase) {
      setShapeType("Plane");
      theVals.addPoint("Origin", aPlane->location());
      theVals.addDirection("Normal", aPlane->direction());
    }
  }
  // Sphere
  else if (GeomSpherePtr aSphere = theFace->getSphere()) {
    setShapeType("Sphere");
    theVals.addPoint("Center", aSphere->center());
    theVals.addGroupName("Dimensions");
    theVals.addNamedValue("Radius", aSphere->radius());
  }
  // Cylinder
  else if (GeomCylinderPtr aCylinder = theFace->getCylinder())
  {
    setShapeType("Cylinder");
    theVals.addPoint("Position", aCylinder->location());
    theVals.addDirection("Axis", aCylinder->axis());
    theVals.addGroupName("Dimensions");
    theVals.addNamedValue("Radius", aCylinder->radius());
    theVals.addNamedValue("Height", aCylinder->height());
  }
  // Cone
  else if (GeomConePtr aCone = theFace->getCone()) {
    setShapeType("Cone");
    theVals.addPoint("Position", aCone->location());
    theVals.addDirection("Axis", aCone->axis());
    theVals.addGroupName("Dimensions");
    theVals.addNamedValue("Radius 1", aCone->radius1());
    theVals.addNamedValue("Radius 2", aCone->radius2());
    theVals.addNamedValue("Height", aCone->height());

  }
  // Torus
  else if (GeomTorusPtr aTorus = theFace->getTorus()) {
    setShapeType("Torus");
    theVals.addPoint("Center", aTorus->center());
    theVals.addDirection("Axis", aTorus->direction());
    theVals.addGroupName("Dimensions");
    theVals.addNamedValue("Major radius", aTorus->majorRadius());
    theVals.addNamedValue("Minor radius", aTorus->minorRadius());
  }
  // Common case
  else
    setShapeType("Face");
}

GeomAlgoAPI_ShapeInfo::Values::Values(const GeomAlgoAPI_ShapeInfo::InfoType theType)
  : myType(theType)
{
  static GeomAlgoAPI_ShapeInfo::Translator* kDefaultTr = new GeomAlgoAPI_ShapeInfo::Translator;
  myTr = kDefaultTr;
}


GeomAlgoAPI_ShapeInfo::Values& GeomAlgoAPI_ShapeInfo::Values::operator<< (double theValue)
{
  switch (myType)
  {
  case TYPE_HTML:
  {
    std::ostringstream aStr;
    aStr << theValue;
    myStr += aStr.str();
    break;
  }
  case TYPE_VALS:
    myVals.push_back(GeomAlgoAPI_InfoValue(theValue));
    break;
  }
  return *this;
}

GeomAlgoAPI_ShapeInfo::Values& GeomAlgoAPI_ShapeInfo::Values::operator << (bool theValue)
{
  switch (myType)
  {
  case TYPE_HTML:
    myStr += myTr->translate(theValue ? "True" : "False");
    break;
  case TYPE_VALS:
    myVals.push_back(GeomAlgoAPI_InfoValue(theValue));
    break;
  }
  return *this;
}

GeomAlgoAPI_ShapeInfo::Values& GeomAlgoAPI_ShapeInfo::Values::operator<< (const char* theStr)
{
  if (myType == TYPE_HTML)
      myStr += myTr->translate(theStr);
  return *this;
}

void GeomAlgoAPI_ShapeInfo::Values::addPoint(const char* theTitle, const GeomPointPtr& thePoint)
{
  switch (myType)
  {
  case TYPE_HTML:
  {
    std::ostringstream aStr;
    aStr << "<b>" << myTr->translate(theTitle) << "</b>"
      << "<br> X: " << thePoint->x()
      << "<br> Y: " << thePoint->y()
      << "<br> Z: " << thePoint->z() << "<br>";
    myStr += aStr.str();
    break;
  }
  case TYPE_VALS:
    myVals.push_back(GeomAlgoAPI_InfoValue(thePoint->x()));
    myVals.push_back(GeomAlgoAPI_InfoValue(thePoint->y()));
    myVals.push_back(GeomAlgoAPI_InfoValue(thePoint->z()));
    break;
  }
}

void GeomAlgoAPI_ShapeInfo::Values::addDirection(const char* theTitle, const GeomDirPtr& theDir)
{
  switch (myType)
  {
  case TYPE_HTML:
  {
    std::ostringstream aStr;
    aStr << "<b>" << myTr->translate(theTitle) << "</b>"
      << "<br> DX: " << theDir->x()
      << "<br> DY: " << theDir->y()
      << "<br> DZ: " << theDir->z() << "<br>";
    myStr += aStr.str();
    break;
  }
  case TYPE_VALS:
    myVals.push_back(GeomAlgoAPI_InfoValue(theDir->x()));
    myVals.push_back(GeomAlgoAPI_InfoValue(theDir->y()));
    myVals.push_back(GeomAlgoAPI_InfoValue(theDir->z()));
    break;
  }
}

void GeomAlgoAPI_ShapeInfo::Values::addNamedValue(const char* theName, const double theValue)
{
  switch (myType)
  {
  case TYPE_HTML:
  {
    std::ostringstream aStr;
    aStr << myTr->translate(theName) << ": " << theValue << "<br>";
    myStr += aStr.str();
    break;
  }
  case TYPE_VALS:
    *this << theValue; // just value, no name
    break;
  }
}

void GeomAlgoAPI_ShapeInfo::Values::addNamedValue(const char* theName, const bool theValue)
{
  switch (myType)
  {
  case TYPE_HTML:
  {
    std::ostringstream aStr;
    aStr << myTr->translate(theName) << ": " << (theValue ? "True" : "False") << "<br>";
    myStr += aStr.str();
    break;
  }
  case TYPE_VALS:
    *this << theValue; // just value, no name
    break;
  }
}

void GeomAlgoAPI_ShapeInfo::Values::addGroupName(const char* theName)
{
  if (myType == TYPE_HTML)
  {
    myStr += "<b>" + myTr->translate(theName) + "</b>" + "<br>";
  }
}
