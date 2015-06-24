// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Plane.cpp
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#include "ConstructionPlugin_Plane.h"

#include <Config_PropManager.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <GeomAPI_Pnt2d.h>

ConstructionPlugin_Plane::ConstructionPlugin_Plane()
{
}

void ConstructionPlugin_Plane::initAttributes()
{
  data()->addAttribute(ConstructionPlugin_Plane::METHOD(), ModelAPI_AttributeString::typeId());
  // Face & Distance
  data()->addAttribute(ConstructionPlugin_Plane::FACE(),  ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(ConstructionPlugin_Plane::DISTANCE(), ModelAPI_AttributeDouble::typeId());
  // General equation
  data()->addAttribute(ConstructionPlugin_Plane::A(),  ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Plane::B(),  ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Plane::C(),  ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ConstructionPlugin_Plane::D(),  ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ConstructionPlugin_Plane::A());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ConstructionPlugin_Plane::B());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ConstructionPlugin_Plane::C());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ConstructionPlugin_Plane::D());
}

void ConstructionPlugin_Plane::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(ConstructionPlugin_Plane::METHOD());
  std::string aMethodType = aMethodTypeAttr->value();
  std::shared_ptr<GeomAPI_Shape> aPlaneFace;
  if (aMethodType == "PlaneByFaceAndDistance") {
    aPlaneFace = createPlaneByFaceAndDistance();
  } else if (aMethodType == "PlaneByGeneralEquation") {
    aPlaneFace = createPlaneByGeneralEquation();
  }
  if (!aPlaneFace.get())
    return;
  ResultConstructionPtr aConstr = document()->createConstruction(data());
  aConstr->setInfinite(true);
  aConstr->setShape(aPlaneFace);
  setResult(aConstr);
}

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

std::shared_ptr<GeomAPI_Shape>  ConstructionPlugin_Plane::createPlaneByFaceAndDistance()
{
  AttributeSelectionPtr aFaceAttr = data()->selection(ConstructionPlugin_Plane::FACE());
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

    if (aShape.get() != NULL) {
      std::shared_ptr<GeomAPI_Pln> aPln = GeomAlgoAPI_FaceBuilder::plane(aShape);
      std::shared_ptr<GeomAPI_Pnt> aOrig = aPln->location();
      std::shared_ptr<GeomAPI_Dir> aDir = aPln->direction();

      aOrig->translate(aDir, aDist);
      std::shared_ptr<GeomAPI_Pln> aNewPln = std::shared_ptr<GeomAPI_Pln>(
          new GeomAPI_Pln(aOrig, aDir));

      // Create rectangular face close to the selected
      double aXmin, aYmin, Zmin, aXmax, aYmax, Zmax;
      aShape->computeSize(aXmin, aYmin, Zmin, aXmax, aYmax, Zmax);

      std::shared_ptr<GeomAPI_Pnt> aPnt1 = std::shared_ptr<GeomAPI_Pnt>(
          new GeomAPI_Pnt(aXmin, aYmin, Zmin));
      std::shared_ptr<GeomAPI_Pnt> aPnt2 = std::shared_ptr<GeomAPI_Pnt>(
          new GeomAPI_Pnt(aXmax, aYmax, Zmax));

      std::shared_ptr<GeomAPI_Pnt2d> aPnt2d1 = aPnt1->to2D(aNewPln);
      std::shared_ptr<GeomAPI_Pnt2d> aPnt2d2 = aPnt2->to2D(aNewPln);

      double aWidth = aPnt2d2->x() - aPnt2d1->x();
      double aHeight = aPnt2d2->y() - aPnt2d1->y();
      double aWgap = aWidth * 0.1;
      double aHgap = aHeight * 0.1;

      aPlane = GeomAlgoAPI_FaceBuilder::planarFace(aNewPln,
                                                   aPnt2d1->x() - aWgap,
                                                   aPnt2d1->y() - aHgap,
                                                   aWidth + 2 * aWgap,
                                                   aHeight + 2 * aHgap);
    }
  }
  return aPlane;
}

std::shared_ptr<GeomAPI_Shape> ConstructionPlugin_Plane::createPlaneByGeneralEquation()
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
    aPlaneFace = GeomAlgoAPI_FaceBuilder::square(aPlane, aSize);
  }
  return aPlaneFace;
}

