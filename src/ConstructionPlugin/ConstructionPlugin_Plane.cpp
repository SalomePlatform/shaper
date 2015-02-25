// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Plane.cpp
// Created:     12 Dec 2014
// Author:      Vitaly Smetannikov

#include "ConstructionPlugin_Plane.h"

#include <Config_PropManager.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <GeomAPI_Pnt2d.h>


#define PLANE_SIZE 300

ConstructionPlugin_Plane::ConstructionPlugin_Plane()
{
}

void ConstructionPlugin_Plane::initAttributes()
{
  data()->addAttribute(ConstructionPlugin_Plane::FACE(),  ModelAPI_AttributeSelection::type());
  data()->addAttribute(ConstructionPlugin_Plane::DISTANCE(), ModelAPI_AttributeDouble::type());
}

void ConstructionPlugin_Plane::execute()
{
  AttributeSelectionPtr aFaceAttr = data()->selection(ConstructionPlugin_Plane::FACE());
  AttributeDoublePtr aDistAttr = data()->real(ConstructionPlugin_Plane::DISTANCE());
  if ((aFaceAttr.get() != NULL) && (aDistAttr.get() != NULL) && 
    aFaceAttr->isInitialized() && aDistAttr->isInitialized()) {

    double aDist = aDistAttr->value();
    GeomShapePtr aShape = aFaceAttr->value();
    if (aShape.get() != NULL) {
      std::shared_ptr<GeomAPI_Pln> aPln = GeomAlgoAPI_FaceBuilder::plane(aShape);
      std::shared_ptr<GeomAPI_Pnt> aOrig = aPln->location();
      std::shared_ptr<GeomAPI_Dir> aDir = aPln->direction();

      aOrig->translate(aDir, aDist);
      std::shared_ptr<GeomAPI_Pln> aNewPln = 
        std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(aOrig, aDir));

      // Create rectangular face close to the selected
      double aXmin, aYmin, Zmin, aXmax, aYmax, Zmax;
      aShape->computeSize(aXmin, aYmin, Zmin, aXmax, aYmax, Zmax);

      std::shared_ptr<GeomAPI_Pnt> aPnt1 = 
        std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aXmin, aYmin, Zmin));
      std::shared_ptr<GeomAPI_Pnt> aPnt2 = 
        std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aXmax, aYmax, Zmax));

      std::shared_ptr<GeomAPI_Pnt2d> aPnt2d1 = aPnt1->to2D(aNewPln);
      std::shared_ptr<GeomAPI_Pnt2d> aPnt2d2 = aPnt2->to2D(aNewPln);

      double aWidth = aPnt2d2->x() - aPnt2d1->x();
      double aHeight = aPnt2d2->y() - aPnt2d1->y();
      double aWgap = aWidth * 0.1;
      double aHgap = aHeight * 0.1;

      std::shared_ptr<GeomAPI_Shape> aPlane = 
        GeomAlgoAPI_FaceBuilder::planarFace(aNewPln, aPnt2d1->x() - aWgap, aPnt2d1->y() - aHgap, 
                                            aWidth + 2 * aWgap, aHeight + 2 * aHgap);
      ResultConstructionPtr aConstr = document()->createConstruction(data());
      aConstr->setShape(aPlane);
      setResult(aConstr);
    }
  }
}

void ConstructionPlugin_Plane::customisePresentation(AISObjectPtr thePrs)
{
  std::vector<int> aRGB = Config_PropManager::color("Visualization", "construction_plane_color",
                                                    ConstructionPlugin_Plane::DEFAULT_COLOR());
  thePrs->setColor(aRGB[0], aRGB[1], aRGB[2]);
  thePrs->setTransparensy(0.6);
}
