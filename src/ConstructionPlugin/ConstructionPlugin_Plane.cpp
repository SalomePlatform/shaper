// Copyright (C) 2014-2024  CEA, EDF
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

#include "ConstructionPlugin_Plane.h"

#include <Config_PropManager.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_Rotation.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XYZ.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <cfloat>


static GeomShapePtr faceByThreeVertices(const std::shared_ptr<GeomAPI_Vertex> theV1,
                                        const std::shared_ptr<GeomAPI_Vertex> theV2,
                                        const std::shared_ptr<GeomAPI_Vertex> theV3);
static std::shared_ptr<GeomAPI_Face> makeRectangularFace(
  const std::shared_ptr<GeomAPI_Face> theFace,
  const std::shared_ptr<GeomAPI_Pln> thePln);

//==================================================================================================
ConstructionPlugin_Plane::ConstructionPlugin_Plane()
{
}

//==================================================================================================
void ConstructionPlugin_Plane::initAttributes()
{
  data()->addAttribute(ConstructionPlugin_Plane::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  data()->addAttribute(PLANE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(DISTANCE(), ModelAPI_AttributeDouble::typeId());
  // By general equation.
  data()->addAttribute(A(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(B(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(C(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(D(), ModelAPI_AttributeDouble::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), A());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), B());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), C());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), D());

  // By three points.
  data()->addAttribute(POINT1(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(POINT2(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(POINT3(), ModelAPI_AttributeSelection::typeId());

  // By line and point.
  data()->addAttribute(LINE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(POINT(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PERPENDICULAR(), ModelAPI_AttributeBoolean::typeId());

  // By other plane.
  data()->addAttribute(CREATION_METHOD_BY_OTHER_PLANE_OPTION(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(REVERSE(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(COINCIDENT_POINT(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(AXIS(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(ANGLE(), ModelAPI_AttributeDouble::typeId());

  // By two parallel planes.
  data()->addAttribute(PLANE1(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PLANE2(), ModelAPI_AttributeSelection::typeId());

  // By other plane.
  AttributeIntegerPtr aNbCopies = std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(
    data()->addAttribute(NB_COPIES(), ModelAPI_AttributeInteger::typeId()));

  if (!aNbCopies->isInitialized())
    aNbCopies->setValue(1);
}

//==================================================================================================
void ConstructionPlugin_Plane::execute()
{
  ListOfShape aShapes;

  std::string aCreationMethod = string(CREATION_METHOD())->value();
  if(aCreationMethod == CREATION_METHOD_BY_GENERAL_EQUATION() ||
      aCreationMethod == "PlaneByGeneralEquation") {
    aShapes.push_back(createByGeneralEquation());
  } else if(aCreationMethod == CREATION_METHOD_BY_THREE_POINTS()) {
    aShapes.push_back(createByThreePoints());
  } else if(aCreationMethod == CREATION_METHOD_BY_LINE_AND_POINT()) {
    aShapes.push_back(createByLineAndPoint());
  } else if(aCreationMethod == CREATION_METHOD_BY_OTHER_PLANE()) {
    std::string aCreationMethodOption = string(CREATION_METHOD_BY_OTHER_PLANE_OPTION())->value();
    if(aCreationMethodOption == CREATION_METHOD_BY_DISTANCE_FROM_OTHER()) {
      createByDistanceFromOther(aShapes);
    } else if(aCreationMethodOption == CREATION_METHOD_BY_COINCIDENT_TO_POINT()) {
      aShapes.push_back(createByCoincidentPoint());
    } else if(aCreationMethodOption == CREATION_METHOD_BY_ROTATION()) {
      createByRotation(aShapes);
    }
  } else if(aCreationMethod == CREATION_METHOD_BY_TWO_PARALLEL_PLANES()) {
    aShapes.push_back(createByTwoParallelPlanes());
  } else {
    setError("Error: Plane creation method \"" + aCreationMethod + "\" not supported.");
    return;
  }

  if(aShapes.size() == 0) {
    setError("Error: Could not create a plane.");
    return;
  }

  int anIndex = 0;
  for (auto aShapeIter = aShapes.begin(); aShapeIter != aShapes.end(); ++aShapeIter, ++anIndex)
  {
    if (!aShapeIter->get())
    {
      setError("Error: Could not create a plane.");
      continue;
    }
    ResultConstructionPtr aConstr = document()->createConstruction(data(), anIndex);
    aConstr->setInfinite(true);
    aConstr->setShape(*aShapeIter);
    setResult(aConstr, anIndex);
  }
  removeResults(anIndex);
}

//==================================================================================================
bool ConstructionPlugin_Plane::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs)
{
  std::vector<int> aColor;
  // get color from the attribute of the result
  if (theResult.get() != NULL &&
      theResult->data()->attribute(ModelAPI_Result::COLOR_ID()).get() != NULL) {
    AttributeIntArrayPtr aColorAttr = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
    if (aColorAttr.get() && aColorAttr->size()) {
      aColor.push_back(aColorAttr->value(0));
      aColor.push_back(aColorAttr->value(1));
      aColor.push_back(aColorAttr->value(2));
    }
  }
  if (aColor.empty())
    aColor = Config_PropManager::color("Visualization", COLOR_NAME());

  bool isCustomized = false;
  if (aColor.size() == 3)
    isCustomized = thePrs->setColor(aColor[0], aColor[1], aColor[2]);

  isCustomized = thePrs->setTransparensy(0.6) || isCustomized;

  return isCustomized;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> ConstructionPlugin_Plane::createByGeneralEquation()
{
  AttributeDoublePtr anAttrA = real(ConstructionPlugin_Plane::A());
  AttributeDoublePtr anAttrB = real(ConstructionPlugin_Plane::B());
  AttributeDoublePtr anAttrC = real(ConstructionPlugin_Plane::C());
  AttributeDoublePtr anAttrD = real(ConstructionPlugin_Plane::D());
  std::shared_ptr<GeomAPI_Shape> aPlaneFace;
  if ((anAttrA.get() != NULL) && (anAttrB.get() != NULL) &&
      (anAttrC.get() != NULL) && (anAttrD.get() != NULL) &&
      anAttrA->isInitialized() && anAttrB->isInitialized() &&
      anAttrC->isInitialized() && anAttrD->isInitialized() ) {
    double aA = anAttrA->value(), aB = anAttrB->value(),
           aC = anAttrC->value(), aD = anAttrD->value();
    std::shared_ptr<GeomAPI_Pln> aPlane =
      std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(aA, aB, aC, aD));
    double aSize = Config_PropManager::real(SKETCH_TAB_NAME, "planes_size");
    if (aSize <= 1.e-7)
      aSize = 200;  // Set default value
    aSize *= 4.;
    aPlaneFace = GeomAlgoAPI_FaceBuilder::squareFace(aPlane, aSize);
  }
  return aPlaneFace;
}
//==================================================================================================
std::shared_ptr<GeomAPI_Shape> ConstructionPlugin_Plane::createByThreePoints()
{
  // Get first point.
  AttributeSelectionPtr aPointSelection1 = selection(POINT1());
  GeomShapePtr aPointShape1 = aPointSelection1->value();
  if(!aPointShape1.get()) {
    aPointShape1 = aPointSelection1->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex1(new GeomAPI_Vertex(aPointShape1));

  // Get second point.
  AttributeSelectionPtr aPointSelection2 = selection(POINT2());
  GeomShapePtr aPointShape2 = aPointSelection2->value();
  if(!aPointShape2.get()) {
    aPointShape2 = aPointSelection2->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex2(new GeomAPI_Vertex(aPointShape2));

  // Get third point.
  AttributeSelectionPtr aPointSelection3 = selection(POINT3());
  GeomShapePtr aPointShape3 = aPointSelection3->value();
  if(!aPointShape3.get()) {
    aPointShape3 = aPointSelection3->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex3(new GeomAPI_Vertex(aPointShape3));

  GeomShapePtr aRes = faceByThreeVertices(aVertex1, aVertex2, aVertex3);

  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> ConstructionPlugin_Plane::createByLineAndPoint()
{
  // Get edge.
  AttributeSelectionPtr anEdgeSelection = selection(LINE());
  GeomShapePtr aLineShape = anEdgeSelection->value();
  if(!aLineShape.get()) {
    aLineShape = anEdgeSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> anEdge;
  if (aLineShape->isEdge()) {
    anEdge = aLineShape->edge();
  }
  else if (aLineShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(aLineShape);
    anEdge = anIt.current()->edge();
  }
  if (!anEdge)
    return GeomShapePtr();

  // Get point.
  AttributeSelectionPtr aPointSelection = selection(POINT());
  GeomShapePtr aPointShape = aPointSelection->value();
  if(!aPointShape.get()) {
    aPointShape = aPointSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aPointShape));

  // Get perpendicular flag.
  bool anIsPerpendicular= boolean(PERPENDICULAR())->value();

  GeomShapePtr aRes;
  if(anIsPerpendicular) {
    std::shared_ptr<GeomAPI_Lin> aLin = anEdge->line();
    std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
    std::shared_ptr<GeomAPI_Pln> aNewPln(new GeomAPI_Pln(aPnt, aLin->direction()));
    double aSize = aLin->distance(aPnt) * 2;
    // point may belong to line, so for the face size use maximum distance between point and line
    // and the line size (distance between the start and end point)
    double aDistance = anEdge->firstPoint()->distance(anEdge->lastPoint());
    aRes = GeomAlgoAPI_FaceBuilder::squareFace(aNewPln, aSize > aDistance ? aSize : aDistance);
  } else {
    std::shared_ptr<GeomAPI_Vertex> aV1, aV2;
    GeomAlgoAPI_ShapeTools::findBounds(anEdge, aV1, aV2);
    aRes = faceByThreeVertices(aV1, aV2, aVertex);
  }

  return aRes;
}

//==================================================================================================
void ConstructionPlugin_Plane::createByDistanceFromOther(ListOfShape& theShapes)
{
  AttributeSelectionPtr aFaceAttr = data()->selection(ConstructionPlugin_Plane::PLANE());
  AttributeDoublePtr aDistAttr = data()->real(ConstructionPlugin_Plane::DISTANCE());
  AttributeIntegerPtr aNbCopyAttr = data()->integer(ConstructionPlugin_Plane::NB_COPIES());
  if ((aFaceAttr.get() != NULL) &&
      (aDistAttr.get() != NULL) && (aNbCopyAttr.get() != NULL) &&
      aFaceAttr->isInitialized() && aDistAttr->isInitialized()) {

    double aDist = aDistAttr->value();
    bool anIsReverse = boolean(REVERSE())->value();
    int aNumOfCopies = aNbCopyAttr->value();
    if(anIsReverse) aDist = -aDist;
    GeomShapePtr aShape = aFaceAttr->value();
    if (!aShape.get() && aFaceAttr->context()) {
      aShape = aFaceAttr->context()->shape();
    }

    if(!aShape.get()) {
      return;
    }

    std::shared_ptr<GeomAPI_Face> aFace;
    if (aShape->isFace()) {
      aFace = aShape->face();
    }
    else if (aShape->isCompound()) {
      GeomAPI_ShapeIterator anIt(aShape);
      aFace = anIt.current()->face();
    }
    if (!aFace)
      return;

    std::shared_ptr<GeomAPI_Pln> aPln = aFace->getPlane();
    std::shared_ptr<GeomAPI_Pnt> aOrig = aPln->location();
    std::shared_ptr<GeomAPI_Dir> aDir = aPln->direction();

    for (int aNbCopy = 0; aNbCopy < aNumOfCopies; ++aNbCopy)
    {
      std::shared_ptr<GeomAPI_Shape> aPlane;
      aOrig->translate(aDir, aDist);
      std::shared_ptr<GeomAPI_Pln> aNewPln(new GeomAPI_Pln(aOrig, aDir));

      aPlane = makeRectangularFace(aFace, aNewPln);
      theShapes.push_back(aPlane);
    }
  }
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> ConstructionPlugin_Plane::createByCoincidentPoint()
{
  // Get face.
  AttributeSelectionPtr aFaceSelection = selection(PLANE());
  GeomShapePtr aFaceShape = aFaceSelection->value();
  if(!aFaceShape.get()) {
    aFaceShape = aFaceSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Face> aFace;
  if (aFaceShape->isFace()) {
    aFace = aFaceShape->face();
  }
  else if (aFaceShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(aFaceShape);
    aFace = anIt.current()->face();
  }

  // Get point.
  AttributeSelectionPtr aPointSelection = selection(COINCIDENT_POINT());
  GeomShapePtr aPointShape = aPointSelection->value();
  if(!aPointShape.get()) {
    aPointShape = aPointSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex = aPointShape->vertex();
  if (!aVertex)
    return GeomShapePtr();

  std::shared_ptr<GeomAPI_Pnt> anOrig = aVertex->point();
  std::shared_ptr<GeomAPI_Pln> aPln = aFace->getPlane();
  std::shared_ptr<GeomAPI_Dir> aDir = aPln->direction();

  std::shared_ptr<GeomAPI_Pln> aNewPln(new GeomAPI_Pln(anOrig, aDir));

  return makeRectangularFace(aFace, aNewPln);
}

//==================================================================================================
void ConstructionPlugin_Plane::createByRotation(ListOfShape& theShapes)
{
  // Get face.
  AttributeSelectionPtr aFaceSelection = selection(PLANE());
  GeomShapePtr aFaceShape = aFaceSelection->value();
  if(!aFaceShape.get()) {
    aFaceShape = aFaceSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Face> aFace;
  if (aFaceShape->isFace()) {
    aFace = aFaceShape->face();
  }
  else if (aFaceShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(aFaceShape);
    aFace = anIt.current()->face();
  }
  if (!aFace)
    return;
  aFace = makeRectangularFace(aFace, aFace->getPlane());

  // Get axis.
  AttributeSelectionPtr anAxisSelection = selection(AXIS());
  GeomShapePtr anAxisShape = anAxisSelection->value();
  if(!anAxisShape.get()) {
    anAxisShape = anAxisSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Edge> anEdge;
  if (anAxisShape->isEdge()) {
    anEdge = anAxisShape->edge();
  }
  else if (anAxisShape->isCompound()) {
    GeomAPI_ShapeIterator anIt(anAxisShape);
    anEdge = anIt.current()->edge();
  }
  if (!anEdge)
    return;

  AttributeIntegerPtr aNbCopyAttr = data()->integer(ConstructionPlugin_Plane::NB_COPIES());
  int aNBCopy;
  if (!aNbCopyAttr.get())
    return;

  aNBCopy = aNbCopyAttr->value();

  std::shared_ptr<GeomAPI_Ax1> anAxis =
    std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(),
                                                 anEdge->line()->direction()));

  // Getting angle.
  double anAngle = real(ANGLE())->value();

  for (int anIndex = 1; anIndex <= aNBCopy; ++anIndex)
  {
    std::shared_ptr<GeomAlgoAPI_Rotation> aRotationAlgo(
      new GeomAlgoAPI_Rotation(aFace, anAxis, anAngle * anIndex));
    // Checking that the algorithm worked properly.
    std::string anError;
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aRotationAlgo, getKind(), anError)) {
      setError("Error: Failed to rotate plane");
      return;
    }

    std::shared_ptr<GeomAPI_Face> aRes(new GeomAPI_Face(aRotationAlgo->shape()));
    theShapes.push_back(aRes);
  }
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> ConstructionPlugin_Plane::createByTwoParallelPlanes()
{
  // Get plane 1.
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
    return GeomShapePtr();
  std::shared_ptr<GeomAPI_Pln> aPln1 = aFace1->getPlane();

  // Get plane 2.
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
    return GeomShapePtr();
  std::shared_ptr<GeomAPI_Pln> aPln2 = aFace2->getPlane();

  std::shared_ptr<GeomAPI_Pnt> anOrig1 = aPln1->location();
  std::shared_ptr<GeomAPI_Pnt> aPntOnPln2 = aPln2->project(anOrig1);

  std::shared_ptr<GeomAPI_Pnt> aNewOrig(new GeomAPI_Pnt(anOrig1->xyz()->added(
    aPntOnPln2->xyz())->multiplied(0.5)));

  std::shared_ptr<GeomAPI_Pln> aNewPln(new GeomAPI_Pln(aNewOrig, aPln1->direction()));

  std::shared_ptr<GeomAPI_Face> aRes = makeRectangularFace(aFace1, aNewPln);

  return aRes;
}

//==================================================================================================
GeomShapePtr faceByThreeVertices(const std::shared_ptr<GeomAPI_Vertex> theV1,
                                 const std::shared_ptr<GeomAPI_Vertex> theV2,
                                 const std::shared_ptr<GeomAPI_Vertex> theV3)
{
  std::shared_ptr<GeomAPI_Face> aFace =
    GeomAlgoAPI_FaceBuilder::planarFaceByThreeVertices(theV1, theV2, theV3);

  ListOfShape anObjects;
  anObjects.push_back(theV1);
  anObjects.push_back(theV2);
  anObjects.push_back(theV3);
  std::list<std::shared_ptr<GeomAPI_Pnt> > aBoundingPoints =
    GeomAlgoAPI_ShapeTools::getBoundingBox(anObjects, 1.0);
  GeomShapePtr aRes = GeomAlgoAPI_ShapeTools::fitPlaneToBox(aFace, aBoundingPoints);

  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Face> makeRectangularFace(const std::shared_ptr<GeomAPI_Face> theFace,
                                                  const std::shared_ptr<GeomAPI_Pln> thePln)
{
  // Create rectangular face close to the selected
  double aXmin, aYmin, Zmin, aXmax, aYmax, Zmax;
  theFace->computeSize(aXmin, aYmin, Zmin, aXmax, aYmax, Zmax);

  // use all 8 points of the bounding box to find the 2D bounds
  bool isFirst = true;
  double aMinX2d = DBL_MAX, aMaxX2d = -DBL_MAX;
  double aMinY2d = DBL_MAX, aMaxY2d = -DBL_MAX;
  for(int aXIsMin = 0; aXIsMin < 2; aXIsMin++) {
    for(int aYIsMin = 0; aYIsMin < 2; aYIsMin++) {
      for(int aZIsMin = 0; aZIsMin < 2; aZIsMin++) {
        std::shared_ptr<GeomAPI_Pnt> aPnt(new GeomAPI_Pnt(
          aXIsMin ? aXmin : aXmax, aYIsMin ? aYmin : aYmax, aZIsMin ? Zmin : Zmax));
        std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = aPnt->to2D(thePln);
        if (isFirst || aPnt2d->x() < aMinX2d)
          aMinX2d = aPnt2d->x();
        if (isFirst || aPnt2d->y() < aMinY2d)
          aMinY2d = aPnt2d->y();
        if (isFirst || aPnt2d->x() > aMaxX2d)
          aMaxX2d = aPnt2d->x();
        if (isFirst || aPnt2d->y() > aMaxY2d)
          aMaxY2d = aPnt2d->y();
        if (isFirst)
          isFirst = !isFirst;
      }
    }
  }
  double aWgap = (aMaxX2d - aMinX2d) * 0.1;
  double aHgap = (aMaxY2d - aMinY2d) * 0.1;
  std::shared_ptr<GeomAPI_Face> aResFace = GeomAlgoAPI_FaceBuilder::planarFace(thePln,
    aMinX2d - aWgap, aMinY2d - aHgap, aMaxX2d - aMinX2d + 2. * aWgap,
    aMaxY2d - aMinY2d + 2. * aHgap);

  return aResFace;
}
