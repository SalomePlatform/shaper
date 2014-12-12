// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Plane.cpp
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#include "ConstructionPlugin_Plane.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>
#include <GeomAlgoAPI_FaceBuilder.h>


#define PLANE_SIZE 300

ConstructionPlugin_Plane::ConstructionPlugin_Plane()
{
}

void ConstructionPlugin_Plane::initAttributes()
{
  data()->addAttribute(FACE_ATTR,  ModelAPI_AttributeSelection::type());
  data()->addAttribute(DISTANCE_ATTR, ModelAPI_AttributeDouble::type());
}

void ConstructionPlugin_Plane::execute()
{
  AttributeSelectionPtr aFaceAttr = data()->selection(FACE_ATTR);
  AttributeDoublePtr aDistAttr = data()->real(DISTANCE_ATTR);
  if ((aFaceAttr.get() != NULL) && (aDistAttr.get() != NULL) && 
    aFaceAttr->isInitialized() && aDistAttr->isInitialized()) {

    double aDist = aDistAttr->value();
    GeomShapePtr aShape = aFaceAttr->value();
    if (aShape.get() != NULL) {
      std::shared_ptr<GeomAPI_Pln> aPln = GeomAlgoAPI_FaceBuilder::plane(aShape);
      std::shared_ptr<GeomAPI_Pnt> aOrig = aPln->location();
      std::shared_ptr<GeomAPI_Dir> aDir = aPln->direction();

      aOrig->translate(aDir, aDist);
      std::shared_ptr<GeomAPI_Shape> aPlane = 
        GeomAlgoAPI_FaceBuilder::square(aOrig, aDir, PLANE_SIZE);
      ResultConstructionPtr aConstr = document()->createConstruction(data());
      aConstr->setShape(aPlane);
      setResult(aConstr);
    }
  }
}

void ConstructionPlugin_Plane::customisePresentation(AISObjectPtr thePrs)
{
  thePrs->setColor(50, 255, 50);
  thePrs->setTransparensy(0.6);
}