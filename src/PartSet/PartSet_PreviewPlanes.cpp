// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "PartSet_PreviewPlanes.h"

#include <ModuleBase_IWorkshop.h>

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultPart.h>

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
    // result constructions should not be taken as a body
    if (aResult.get() != NULL &&
      ((aResult->groupName() == ModelAPI_ResultBody::group()) ||
      ((aResult->groupName() == ModelAPI_ResultPart::group()))) ) {
      GeomShapePtr aShape = aResult->shape();
      if (aShape.get()) {
        // vertices, edges should not be taken as a body
        aBodyIsVisualized = aShape->shapeType() <= GeomAPI_Shape::FACE;
        if (aBodyIsVisualized)
          break;
      }
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
      if (aFeature.get() && aFeature->getKind() == SketchPlugin_Sketch::ID()) {
        ResultConstructionPtr aCResult =  std::dynamic_pointer_cast<ModelAPI_ResultConstruction>
                                                                                      (aResult);
        if (aCResult.get() && aCResult->facesNum() > 0) {
          aSketchIsVisualized = true;
          break;
        }
      }
      break;
    }
  }
  return aSketchIsVisualized;
}

bool PartSet_PreviewPlanes::isPreviewShape(std::shared_ptr<GeomAPI_Shape> theShape)
{
  return (myYZPlane->getShape()->isEqual(theShape) ||
          myXZPlane->getShape()->isEqual(theShape) ||
          myXYPlane->getShape()->isEqual(theShape));
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

    std::vector<int> aYZRGB(3, 0), aXZRGB(3, 0), aXYRGB(3, 0);
#ifdef SET_PLANES_COLOR_IN_PREFERENCES
    aYZRGB = Config_PropManager::color("Visualization", "yz_plane_color");
    aXZRGB = Config_PropManager::color("Visualization", "xz_plane_color");
    aXYRGB = Config_PropManager::color("Visualization", "xy_plane_color");
#else
    aYZRGB[0] = 225;
    aXZRGB[1] = 225;
    aXYRGB[2] = 225;
#endif
    int aR[] = {aYZRGB[0], aYZRGB[1], aYZRGB[2]};
    int aG[] = {aXZRGB[0], aXZRGB[1], aXZRGB[2]};
    int aB[] = {aXYRGB[0], aXYRGB[1], aXYRGB[2]};

    myYZPlane = createPreviewPlane(anOrigin, aYZDir, aR);
    myXZPlane = createPreviewPlane(anOrigin, aXZDir, aG);
    myXYPlane = createPreviewPlane(anOrigin, aXYDir, aB);
  }
  XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
  aDisp->displayAIS(myYZPlane, true, 0, false);
  aDisp->displayAIS(myXZPlane, true, 0, false);
  aDisp->displayAIS(myXYPlane, true, 0, false);
  myPreviewDisplayed = true;
}

AISObjectPtr PartSet_PreviewPlanes::createPreviewPlane(std::shared_ptr<GeomAPI_Pnt> theOrigin,
                                                       std::shared_ptr<GeomAPI_Dir> theNorm,
                                                       const int theRGB[3])
{
  double aSize = Config_PropManager::real(SKETCH_TAB_NAME, "planes_size");
  if (aSize <= Precision::Confusion())
    aSize = 200;   // Set default value

  std::shared_ptr<GeomAPI_Shape> aFace =
    GeomAlgoAPI_FaceBuilder::squareFace(theOrigin, theNorm, aSize);
  AISObjectPtr aAIS = AISObjectPtr(new GeomAPI_AISObject());
  aAIS->createShape(aFace);
  aAIS->setWidth(Config_PropManager::real(SKETCH_TAB_NAME, "planes_thickness"));
  aAIS->setColor(theRGB[0], theRGB[1], theRGB[2]);
  return aAIS;
}
