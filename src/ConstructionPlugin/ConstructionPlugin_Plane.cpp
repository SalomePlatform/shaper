// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Plane.cpp
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#include "ConstructionPlugin_Plane.h"

#include <Config_PropManager.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

static GeomShapePtr faceByThreeVertices(const std::shared_ptr<GeomAPI_Vertex> theV1,
                                        const std::shared_ptr<GeomAPI_Vertex> theV2,
                                        const std::shared_ptr<GeomAPI_Vertex> theV3);

//==================================================================================================
ConstructionPlugin_Plane::ConstructionPlugin_Plane()
{
}

//==================================================================================================
void ConstructionPlugin_Plane::initAttributes()
{
  data()->addAttribute(ConstructionPlugin_Plane::CREATION_METHOD(), ModelAPI_AttributeString::typeId());

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

  // By other plane.
  data()->addAttribute(CREATION_METHOD_BY_OTHER_PLANE_OPTION(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(PLANE(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(DISTANCE(), ModelAPI_AttributeDouble::typeId());
}

//==================================================================================================
void ConstructionPlugin_Plane::execute()
{
  GeomShapePtr aShape;

  std::string aCreationMethod = string(CREATION_METHOD())->value();
  if(aCreationMethod == CREATION_METHOD_BY_GENERAL_EQUATION()) {
    aShape = createByGeneralEquation();
  } else if(aCreationMethod == CREATION_METHOD_BY_THREE_POINTS()) {
    aShape = createByThreePoints();
  } else if(aCreationMethod == CREATION_METHOD_BY_LINE_AND_POINT()) {
    aShape = createByLineAndPoint();
  } else if(aCreationMethod == CREATION_METHOD_BY_OTHER_PLANE()) {
    std::string aCreationMethodOption = string(CREATION_METHOD_BY_OTHER_PLANE_OPTION())->value();
    if(aCreationMethodOption == CREATION_METHOD_BY_DISTANCE_FROM_OTHER()) {
      aShape = createByDistanceFromOther();
    }
  }


  if(!aShape.get()) {
    return;
  }

  ResultConstructionPtr aConstr = document()->createConstruction(data());
  aConstr->setInfinite(true);
  aConstr->setShape(aShape);
  setResult(aConstr);
}

//==================================================================================================
bool ConstructionPlugin_Plane::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
{
  std::vector<int> aColor;
  // get color from the attribute of the result
  if (theResult.get() != NULL && theResult->data()->attribute(ModelAPI_Result::COLOR_ID()).get() != NULL) {
    AttributeIntArrayPtr aColorAttr = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
    if (aColorAttr.get() && aColorAttr->size()) {
      aColor.push_back(aColorAttr->value(0));
      aColor.push_back(aColorAttr->value(1));
      aColor.push_back(aColorAttr->value(2));
    }
  }
  if (aColor.empty())
    aColor = Config_PropManager::color("Visualization", "construction_plane_color",
                                       ConstructionPlugin_Plane::DEFAULT_COLOR());

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
    std::string kDefaultPlaneSize = "200";
    double aSize = Config_PropManager::integer("Sketch planes", "planes_size", kDefaultPlaneSize);
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
  std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aLineShape));
  std::shared_ptr<GeomAPI_Vertex> aV1, aV2;
  GeomAlgoAPI_ShapeTools::findBounds(anEdge, aV1, aV2);


  // Get point.
  AttributeSelectionPtr aPointSelection = selection(POINT());
  GeomShapePtr aPointShape = aPointSelection->value();
  if(!aPointShape.get()) {
    aPointShape = aPointSelection->context()->shape();
  }
  std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aPointShape));

  GeomShapePtr aRes = faceByThreeVertices(aV1, aV2, aVertex);

  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape>  ConstructionPlugin_Plane::createByDistanceFromOther()
{
  AttributeSelectionPtr aFaceAttr = data()->selection(ConstructionPlugin_Plane::PLANE());
  AttributeDoublePtr aDistAttr = data()->real(ConstructionPlugin_Plane::DISTANCE());
  std::shared_ptr<GeomAPI_Shape> aPlane;
  if ((aFaceAttr.get() != NULL) &&
      (aDistAttr.get() != NULL) &&
      aFaceAttr->isInitialized() && aDistAttr->isInitialized()) {

    double aDist = aDistAttr->value();
    GeomShapePtr aShape = aFaceAttr->value();
    if (!aShape.get()) {
      aShape = aFaceAttr->context()->shape();
    }

    if(!aShape.get()) {
      return aPlane;
    }

    std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aShape));

    std::shared_ptr<GeomAPI_Pln> aPln = GeomAlgoAPI_FaceBuilder::plane(aFace);
    std::shared_ptr<GeomAPI_Pnt> aOrig = aPln->location();
    std::shared_ptr<GeomAPI_Dir> aDir = aPln->direction();

    aOrig->translate(aDir, aDist);
    std::shared_ptr<GeomAPI_Pln> aNewPln(new GeomAPI_Pln(aOrig, aDir));

    // Create rectangular face close to the selected
    double aXmin, aYmin, Zmin, aXmax, aYmax, Zmax;
    aFace->computeSize(aXmin, aYmin, Zmin, aXmax, aYmax, Zmax);

    // use all 8 points of the bounding box to find the 2D bounds
    bool isFirst = true;
    double aMinX2d, aMaxX2d, aMinY2d, aMaxY2d;
    for(int aXIsMin = 0; aXIsMin < 2; aXIsMin++) {
      for(int aYIsMin = 0; aYIsMin < 2; aYIsMin++) {
        for(int aZIsMin = 0; aZIsMin < 2; aZIsMin++) {
          std::shared_ptr<GeomAPI_Pnt> aPnt(new GeomAPI_Pnt(
            aXIsMin ? aXmin : aXmax, aYIsMin ? aYmin : aYmax, aZIsMin ? Zmin : Zmax));
          std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = aPnt->to2D(aNewPln);
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
    aPlane = GeomAlgoAPI_FaceBuilder::planarFace(aNewPln,
      aMinX2d - aWgap, aMinY2d - aHgap, aMaxX2d - aMinX2d + 2. * aWgap, aMaxY2d - aMinY2d + 2. * aHgap);

  }
  return aPlane;
}

//==================================================================================================
GeomShapePtr faceByThreeVertices(const std::shared_ptr<GeomAPI_Vertex> theV1,
                                 const std::shared_ptr<GeomAPI_Vertex> theV2,
                                 const std::shared_ptr<GeomAPI_Vertex> theV3)
{
  std::shared_ptr<GeomAPI_Face> aFace = GeomAlgoAPI_FaceBuilder::planarFaceByThreeVertices(theV1, theV2, theV3);

  ListOfShape anObjects;
  anObjects.push_back(theV1);
  anObjects.push_back(theV2);
  anObjects.push_back(theV3);
  std::list<std::shared_ptr<GeomAPI_Pnt> > aBoundingPoints = GeomAlgoAPI_ShapeTools::getBoundingBox(anObjects, 1.0);
  GeomShapePtr aRes = GeomAlgoAPI_ShapeTools::fitPlaneToBox(aFace, aBoundingPoints);

  return aRes;
}
