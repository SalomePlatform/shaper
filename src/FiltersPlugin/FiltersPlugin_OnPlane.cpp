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

#include "FiltersPlugin_OnPlane.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Curve.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Dir.h>


#define LIN_TOLERANCE 1.e-7

bool FiltersPlugin_OnPlane::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  switch (theType) {
  case GeomAPI_Shape::SHELL:
  case GeomAPI_Shape::FACE:
  case GeomAPI_Shape::WIRE:
  case GeomAPI_Shape::EDGE:
  case GeomAPI_Shape::VERTEX:
    return true;
  }
  return false;
}

GeomPlanePtr getPlane(AttributeSelectionPtr theAttr)
{
  GeomShapePtr aGeom = theAttr->value();
  GeomFacePtr aFace(new GeomAPI_Face(aGeom));
  return aFace->getPlane();
}

bool FiltersPlugin_OnPlane::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                 const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr aAttr = theArgs.argument("OnPlane");
  AttributeSelectionListPtr aList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aAttr);
  if (!aList.get())
    return false;
  if (theShape->isVertex()) {
    GeomVertexPtr aVertex(new GeomAPI_Vertex(theShape));
    GeomPointPtr aPnt = aVertex->point();
    for (int i = 0; i < aList->size(); i++) {
      GeomPlanePtr aPlane = getPlane(aList->value(i));
      if (aPlane->distance(aPnt) < LIN_TOLERANCE)
        return true;
    }
  }
  else if (theShape->isEdge()) {
    GeomEdgePtr aEdge(new GeomAPI_Edge(theShape));
    if (aEdge->isLine()) {
      GeomPointPtr aPnt1 = aEdge->firstPoint();
      GeomPointPtr aPnt2 = aEdge->lastPoint();
      for (int i = 0; i < aList->size(); i++) {
        GeomPlanePtr aPlane = getPlane(aList->value(i));
        if ((aPlane->distance(aPnt1) < LIN_TOLERANCE) && (aPlane->distance(aPnt2) < LIN_TOLERANCE))
          return true;
      }
    }
    else if (aEdge->isCircle()) {
      GeomCurvePtr aCurve(new GeomAPI_Curve(aEdge));
      GeomCirclePtr aCircle(new GeomAPI_Circ(aCurve));
      GeomDirPtr aDir = aCircle->normal();
      GeomPointPtr aPnt = aCircle->center();
      for (int i = 0; i < aList->size(); i++) {
        GeomPlanePtr aPlane = getPlane(aList->value(i));
        if ((aPlane->direction()->isParallel(aDir)) && (aPlane->distance(aPnt) < LIN_TOLERANCE))
          return true;
      }
    }
  }
  else if (theShape->isFace()) {
    GeomFacePtr aFace(new GeomAPI_Face(theShape));
    if (aFace->isPlanar()) {
      GeomPlanePtr aPln = aFace->getPlane();
      for (int i = 0; i < aList->size(); i++) {
        AttributeSelectionPtr aAttr = aList->value(i);
        GeomShapePtr aGeom = aAttr->value();
        GeomPlanePtr aPlane = getPlane(aAttr);
        if (aPlane->isCoincident(aPln))
          return true;
      }
    }
  }
  return false;
}

std::string FiltersPlugin_OnPlane::xmlRepresentation() const
{
  return xmlFromFile("filter-OnPlane.xml");
}

void FiltersPlugin_OnPlane::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("OnPlane", ModelAPI_AttributeSelectionList::typeId());
}
