// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_PreviewPlanes.cpp
// Created:     19 Jun 2015
// Author:      Natalia ERMOLAEVA

#include "PartSet_PreviewPlanes.h"

#include <ModuleBase_IWorkshop.h>

#include <ModelAPI_ResultBody.h>

#include <XGUI_Tools.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>

#include <Config_PropManager.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <SketchPlugin_Sketch.h>

PartSet_PreviewPlanes::PartSet_PreviewPlanes()
 : myPreviewDisplayed(false)
{
}

bool PartSet_PreviewPlanes::hasVisualizedBodies(ModuleBase_IWorkshop* theWorkshop)
{
  bool aBodyIsVisualized = false;

  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(theWorkshop);
  XGUI_Displayer* aDisp = aWorkshop->displayer();
  QObjectPtrList aDisplayed = aDisp->displayedObjects();
  foreach (ObjectPtr anObj, aDisplayed) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (aResult.get() != NULL) {
      aBodyIsVisualized = aResult->groupName() == ModelAPI_ResultBody::group();
      if (aBodyIsVisualized)
        break;
    }
  }
  return aBodyIsVisualized;
}

bool PartSet_PreviewPlanes::hasVisualizedSketch(ModuleBase_IWorkshop* theWorkshop)
{
  bool aSketchIsVisualized = false;

  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(theWorkshop);
  XGUI_Displayer* aDisp = aWorkshop->displayer();
  QObjectPtrList aDisplayed = aDisp->displayedObjects();
  foreach (ObjectPtr anObj, aDisplayed) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (aResult.get() != NULL) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aResult);
      aSketchIsVisualized = aFeature.get() && aFeature->getKind() == SketchPlugin_Sketch::ID();
      if (aSketchIsVisualized)
        break;
    }
  }
  return aSketchIsVisualized;
}

void PartSet_PreviewPlanes::erasePreviewPlanes(ModuleBase_IWorkshop* theWorkshop)
{
  if (myPreviewDisplayed) {
    XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
    aDisp->eraseAIS(myYZPlane, false);
    aDisp->eraseAIS(myXZPlane, false);
    aDisp->eraseAIS(myXYPlane, false);
    myPreviewDisplayed = false;
  }
}

void PartSet_PreviewPlanes::showPreviewPlanes(ModuleBase_IWorkshop* theWorkshop)
{
  if (myPreviewDisplayed)
    return;

  if (!myYZPlane) { // If planes are not created
    // Create Preview
    std::shared_ptr<GeomAPI_Pnt> anOrigin(new GeomAPI_Pnt(0, 0, 0));
    std::shared_ptr<GeomAPI_Dir> aYZDir(new GeomAPI_Dir(1, 0, 0));
    // -1, not 1 for correct internal sketch coords (issue 898)
    std::shared_ptr<GeomAPI_Dir> aXZDir(new GeomAPI_Dir(0, -1, 0));
    std::shared_ptr<GeomAPI_Dir> aXYDir(new GeomAPI_Dir(0, 0, 1));

    std::vector<int> aYZRGB, aXZRGB, aXYRGB;
    aYZRGB = Config_PropManager::color("Visualization", "yz_plane_color",
                                                        YZ_PLANE_COLOR);
    aXZRGB = Config_PropManager::color("Visualization", "xz_plane_color",
                                                        XZ_PLANE_COLOR);
    aXYRGB = Config_PropManager::color("Visualization", "xy_plane_color",
                                                        XY_PLANE_COLOR);
    int aR[] = {aYZRGB[0], aYZRGB[1], aYZRGB[2]};
    int aG[] = {aXZRGB[0], aXZRGB[1], aXZRGB[2]};
    int aB[] = {aXYRGB[0], aXYRGB[1], aXYRGB[2]};

    myYZPlane = createPreviewPlane(anOrigin, aYZDir, aR);
    myXZPlane = createPreviewPlane(anOrigin, aXZDir, aG);
    myXYPlane = createPreviewPlane(anOrigin, aXYDir, aB);
  }
  XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
  aDisp->displayAIS(myYZPlane, true, false);
  aDisp->displayAIS(myXZPlane, true, false);
  aDisp->displayAIS(myXYPlane, true, false);
  myPreviewDisplayed = true;
}

AISObjectPtr PartSet_PreviewPlanes::createPreviewPlane(std::shared_ptr<GeomAPI_Pnt> theOrigin, 
                                                       std::shared_ptr<GeomAPI_Dir> theNorm, 
                                                       const int theRGB[3])
{
  double aSize = Config_PropManager::integer("Sketch planes", "planes_size", PLANE_SIZE);
  std::shared_ptr<GeomAPI_Shape> aFace = GeomAlgoAPI_FaceBuilder::square(theOrigin, theNorm, aSize);
  AISObjectPtr aAIS = AISObjectPtr(new GeomAPI_AISObject());
  aAIS->createShape(aFace);
  aAIS->setWidth(Config_PropManager::integer("Sketch planes", "planes_thickness", SKETCH_WIDTH));
  aAIS->setColor(theRGB[0], theRGB[1], theRGB[2]);
  return aAIS;
}
