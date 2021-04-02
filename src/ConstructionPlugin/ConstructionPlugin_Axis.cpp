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

#include "ConstructionPlugin_Axis.h"

#include <Config_PropManager.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XYZ.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>

#include <math.h>

#ifdef _DEBUG
#include <iostream>
#endif

static const double defaultAxisSize = 100;

ConstructionPlugin_Axis::ConstructionPlugin_Axis()
{
}

void ConstructionPlugin_Axis::initAttributes()
{
  data()->addAttribute(ConstructionPlugin_Axis::METHOD(),
                       ModelAPI_AttributeString::typeId());

  // Attributes needed to build the axis using the "two points" method
  data()->addAttribute(ConstructionPlugin_Axis::POINT_FIRST(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::POINT_SECOND(),
                       ModelAPI_AttributeSelection::typeId());

  // Attributes needed to build the axis using the "cylindrical face" method"
  data()->addAttribute(ConstructionPlugin_Axis::CYLINDRICAL_FACE(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::X_DIRECTION(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::Y_DIRECTION(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::Z_DIRECTION(),
                       ModelAPI_AttributeDouble::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
      ConstructionPlugin_Axis::X_DIRECTION());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
      ConstructionPlugin_Axis::Y_DIRECTION());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
      ConstructionPlugin_Axis::Z_DIRECTION());

  //Attributes needed to build the axis using the "three dimensions" method
  data()->addAttribute(ConstructionPlugin_Axis::DX(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::DY(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Axis::DZ(),
                       ModelAPI_AttributeDouble::typeId());

  /// Attributes for axis by line.
  data()->addAttribute(LINE(), ModelAPI_AttributeSelection::typeId());

  /// Attributes for axis by plane and point.
  data()->addAttribute(PLANE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(POINT(), ModelAPI_AttributeSelection::typeId());

  /// Attributes for axis by two planes.
  data()->addAttribute(PLANE1(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(USE_OFFSET1(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(OFFSET1(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(REVERSE_OFFSET1(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(PLANE2(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(USE_OFFSET2(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(OFFSET2(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(REVERSE_OFFSET2(), ModelAPI_AttributeBoolean::typeId());
}

void ConstructionPlugin_Axis::createAxisByTwoPoints()
{
  AttributeSelectionPtr aRef1 = data()->selection(ConstructionPlugin_Axis::POINT_FIRST());
  AttributeSelectionPtr aRef2 = data()->selection(ConstructionPlugin_Axis::POINT_SECOND());
  if ((aRef1.get() != NULL) && (aRef2.get() != NULL)) {
    GeomShapePtr aShape1 = aRef1->value();
    if (!aShape1.get())
      aShape1 = aRef1->context()->shape();
    GeomShapePtr aShape2 = aRef2->value();
    if (!aShape2.get())
      aShape2 = aRef2->context()->shape();
    if (aShape1->isVertex() && aShape2->isVertex() && (!aShape1->isEqual(aShape2))) {
      std::shared_ptr<GeomAPI_Pnt> aStart = GeomAlgoAPI_PointBuilder::point(aShape1);
      std::shared_ptr<GeomAPI_Pnt> anEnd = GeomAlgoAPI_PointBuilder::point(aShape2);
      if (aStart->distance(anEnd) > ConstructionPlugin_Axis::MINIMAL_LENGTH()) {
        std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);

        ResultConstructionPtr aConstr = document()->createConstruction(data());
        aConstr->setInfinite(true);
        aConstr->setShape(anEdge);
        setResult(aConstr);
      }
    }
  }
}


void ConstructionPlugin_Axis::createAxisByPointAndDirection()
{
  AttributeSelectionPtr aRef1 = data()->selection(ConstructionPlugin_Axis::POINT_FIRST());
  AttributeDoublePtr aXAttr = data()->real(ConstructionPlugin_Axis::X_DIRECTION());
  AttributeDoublePtr aYAttr = data()->real(ConstructionPlugin_Axis::Y_DIRECTION());
  AttributeDoublePtr aZAttr = data()->real(ConstructionPlugin_Axis::Z_DIRECTION());
  if ((aRef1.get() != NULL) && (aXAttr.get() != NULL) &&
      (aYAttr.get() != NULL) && (aZAttr.get() != NULL)) {
    GeomShapePtr aShape1 = aRef1->value();
    if (!aShape1.get())
      aShape1 = aRef1->context()->shape();

    std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aXAttr->value(),
                                                               aYAttr->value(),
                                                               aZAttr->value()));
    if (aShape1->isVertex() &&
        (fabs(aXAttr->value()) > MINIMAL_LENGTH() ||
         fabs(aYAttr->value()) > MINIMAL_LENGTH() ||
         fabs(aZAttr->value()) > MINIMAL_LENGTH())) {
      std::shared_ptr<GeomAPI_Pnt> aStart = GeomAlgoAPI_PointBuilder::point(aShape1);
      std::shared_ptr<GeomAPI_Pnt> anEnd(new GeomAPI_Pnt(aStart->x() + aXAttr->value(),
                                                         aStart->y() + aYAttr->value(),
                                                         aStart->z() + aZAttr->value()));

      std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);

      ResultConstructionPtr aConstr = document()->createConstruction(data());
      aConstr->setInfinite(true);
      aConstr->setShape(anEdge);
      setResult(aConstr);
    }
  }
}


void ConstructionPlugin_Axis::createAxisByCylindricalFace()
{
  std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(CYLINDRICAL_FACE())->value();
    // update arguments due to the selection value

  if (!aSelection.get() || aSelection->isNull()) {
    return;
  }

  if (aSelection->isCompound()) {
    GeomAPI_ShapeIterator anIt(aSelection);
    aSelection = anIt.current();
  }

  if (aSelection->isFace()) {
    std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::cylinderAxis(aSelection);

    ResultConstructionPtr aConstr = document()->createConstruction(data());
    aConstr->setInfinite(true);
    aConstr->setShape(anEdge);
    setResult(aConstr);
  }
}

void ConstructionPlugin_Axis::createAxisByDimensions()
{
  // Start by getting these dimensions
  double aDX = data()->real(DX())->value();
  double aDY = data()->real(DY())->value();
  double aDZ = data()->real(DZ())->value();

  if (fabs(aDX) < MINIMAL_LENGTH()  && fabs(aDY) < MINIMAL_LENGTH() &&
      fabs(aDZ) < MINIMAL_LENGTH()) {
    setError("Axis builder with dimensions  :: all dimensions are null", false);
    return ;
  }

  // Make the axis, build the ResultConstructionPtr and write it
  std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aDX, aDY, aDZ);
  ResultConstructionPtr aConstr = document()->createConstruction(data());
  aConstr->setInfinite(true);
  aConstr->setShape(anEdge);
  setResult(aConstr);
}

void ConstructionPlugin_Axis::createAxisByLine()
{
  // Get edge.
  AttributeSelectionPtr anEdgeSelection = selection(LINE());
  GeomShapePtr aLineShape = anEdgeSelection->value();
  if(!aLineShape.get()) {
    aLineShape = anEdgeSelection->context()->shape();
  }
  GeomEdgePtr anEdge;
  if (aLineShape->isEdge()) {
    anEdge = aLineShape->edge();
  }
  else if (aLineShape->isCompound()) {
    // create an edge which covers all edges from compounds (they are on the same line)
    GeomPointPtr aFirst, aLast;
    GeomXYZPtr aLineVec;
    for(GeomAPI_ShapeIterator anIt(aLineShape); anIt.more(); anIt.next()) {
      GeomEdgePtr aSub = anIt.current()->edge();
      if (aSub.get() && aSub->isLine()) {
        if (!aLineVec.get()) {
          aFirst = aSub->firstPoint();
          aLast = aSub->lastPoint();
        } else { // set aFirst and aLast by extreme points
          GeomXYZPtr aFirstVec = aSub->firstPoint()->xyz()->decreased(aFirst->xyz());
          bool aSameDirection =
            aSub->lastPoint()->xyz()->decreased(aSub->firstPoint()->xyz())->dot(aLineVec) > 0;
          if (aLineVec->dot(aFirstVec) < -1.e-7) { // first point is changed
            aFirst = aSameDirection ? aSub->firstPoint() : aSub->lastPoint();
          } else { // last point is changed
            aLast = aSameDirection ? aSub->lastPoint() : aSub->firstPoint();
          }
        }
        aLineVec = aLast->xyz()->decreased(aFirst->xyz());
      }
    }
    if (aLineVec.get())
      anEdge = GeomAlgoAPI_EdgeBuilder::line(aFirst, aLast);
  }

  ResultConstructionPtr aConstr = document()->createConstruction(data());
  aConstr->setInfinite(true);
  aConstr->setShape(anEdge);
  setResult(aConstr);
}

void ConstructionPlugin_Axis::createAxisByPlaneAndPoint()
{
  // Get face.
  AttributeSelectionPtr aFaceSelection = selection(PLANE());
  GeomShapePtr aFaceShape = aFaceSelection->value();
  if(!aFaceShape.get()) {
    aFaceShape = aFaceSelection->context()->shape();
  }
  GeomFacePtr aFace;
  if (aFaceShape->isFace()) {
    aFace = aFaceShape->face();
  }
  else if (aFaceShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(aFaceShape);
    aFace = anIt.current()->face();
  }
  if (!aFace)
    return;
  std::shared_ptr<GeomAPI_Pln> aPln = aFace->getPlane();

  // Get point.
  AttributeSelectionPtr aPointSelection = selection(POINT());
  GeomShapePtr aPointShape = aPointSelection->value();
  if(!aPointShape.get()) {
    aPointShape = aPointSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aPointShape));
  std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();

  std::shared_ptr<GeomAPI_Pnt> aProjPnt = aPln->project(aPnt);

  if(aProjPnt->isEqual(aPnt)) {
    aPnt->translate(aPln->direction(), defaultAxisSize);
  }

  std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aPnt, aProjPnt);

  ResultConstructionPtr aConstr = document()->createConstruction(data());
  aConstr->setInfinite(true);
  aConstr->setShape(anEdge);
  setResult(aConstr);
}

void ConstructionPlugin_Axis::createAxisByTwoPlanes()
{
  // Get face 1.
  AttributeSelectionPtr aFaceSelection1 = selection(PLANE1());
  GeomShapePtr aFaceShape1 = aFaceSelection1->value();
  if(!aFaceShape1.get()) {
    aFaceShape1 = aFaceSelection1->context()->shape();
  }
  std::shared_ptr<GeomAPI_Face> aFace1;
  if (aFaceShape1->isFace()) {
    aFace1 = aFaceShape1->face();
  }
  else if (aFaceShape1->isCompound()) {
    GeomAPI_ShapeIterator anIt(aFaceShape1);
    aFace1 = anIt.current()->face();
  }
  if (!aFace1)
    return;
  std::shared_ptr<GeomAPI_Pln> aPln1 = aFace1->getPlane();

  std::string useOffset1 = string(USE_OFFSET1())->value();
  if(!useOffset1.empty()) {
    double anOffset1 = real(OFFSET1())->value();
    bool reverseOffset1 = boolean(REVERSE_OFFSET1())->value();
    if(reverseOffset1) {
      anOffset1 = -anOffset1;
    }
    aPln1->translate(aPln1->direction(), anOffset1);
  }

  // Get face 2.
  AttributeSelectionPtr aFaceSelection2 = selection(PLANE2());
  GeomShapePtr aFaceShape2 = aFaceSelection2->value();
  if(!aFaceShape2.get()) {
    aFaceShape2 = aFaceSelection2->context()->shape();
  }
  std::shared_ptr<GeomAPI_Face> aFace2;
  if (aFaceShape2->isFace()) {
    aFace2 = aFaceShape2->face();
  }
  else if (aFaceShape2->isCompound()) {
    GeomAPI_ShapeIterator anIt(aFaceShape2);
    aFace2 = anIt.current()->face();
  }
  if (!aFace2)
    return;
  std::shared_ptr<GeomAPI_Pln> aPln2 = aFace2->getPlane();

  std::string useOffset2 = string(USE_OFFSET2())->value();
  if(!useOffset2.empty()) {
    double anOffset2 = real(OFFSET2())->value();
    bool reverseOffset2 = boolean(REVERSE_OFFSET2())->value();
    if(reverseOffset2) {
      anOffset2 = -anOffset2;
    }
    aPln2->translate(aPln2->direction(), anOffset2);
  }

  std::shared_ptr<GeomAPI_Lin> aLin = aPln1->intersect(aPln2);
  std::shared_ptr<GeomAPI_Pnt> aPnt1 = aLin->location();
  std::shared_ptr<GeomAPI_Pnt> aPnt2 = aLin->location();
  aPnt2->translate(aLin->direction(), defaultAxisSize);

  std::shared_ptr<GeomAPI_Edge> anEdge = GeomAlgoAPI_EdgeBuilder::line(aPnt1, aPnt2);

  ResultConstructionPtr aConstr = document()->createConstruction(data());
  aConstr->setInfinite(true);
  aConstr->setShape(anEdge);
  setResult(aConstr);
}

void ConstructionPlugin_Axis::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(ConstructionPlugin_Axis::METHOD());
  std::string aMethodType = aMethodTypeAttr->value();
  if (aMethodType == CREATION_METHOD_BY_TWO_POINTS()) {
    createAxisByTwoPoints();
  } else if (aMethodType == CREATION_METHOD_BY_CYLINDRICAL_FACE()) {
    createAxisByCylindricalFace();
  } else if (aMethodType == CREATION_METHOD_BY_POINT_AND_DIRECTION()) {
    createAxisByPointAndDirection();
  } else if (aMethodType == CREATION_METHOD_BY_DIMENSIONS()) {
    createAxisByDimensions();
  } else if(aMethodType == CREATION_METHOD_BY_LINE()) {
    createAxisByLine();
  } else if(aMethodType == CREATION_METHOD_BY_PLANE_AND_POINT()) {
    createAxisByPlaneAndPoint();
  } else if(aMethodType == CREATION_METHOD_BY_TWO_PLANES()) {
    createAxisByTwoPlanes();
  }
}

bool ConstructionPlugin_Axis::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs)
{
  bool isCustomized = false;

  isCustomized = thePrs->setLineStyle(3) || isCustomized;
  isCustomized = thePrs->setWidth(2) || isCustomized;

  return isCustomized;
}
