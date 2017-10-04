// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "PartSet_PreviewSketchPlane.h"
#include "PartSet_Tools.h"

#include <ModuleBase_IWorkshop.h>

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_AISObject.h>

#include <XGUI_Tools.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>

#include <Config_PropManager.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_SketchEntity.h>

PartSet_PreviewSketchPlane::PartSet_PreviewSketchPlane()
 : myPreviewIsDisplayed(false)
{
}

void PartSet_PreviewSketchPlane::eraseSketchPlane(ModuleBase_IWorkshop* theWorkshop,
                                                  const bool isClearPlane)
{
  if (myPreviewIsDisplayed) {
    XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
    aDisp->eraseAIS(myPlane, false);
    if (isClearPlane) {
      myPlane = std::shared_ptr<GeomAPI_AISObject>();
      myShape = std::shared_ptr<GeomAPI_Shape>();
    }
    myPreviewIsDisplayed = false;
  }
}

void PartSet_PreviewSketchPlane::createSketchPlane(const CompositeFeaturePtr& theSketch,
                                                   ModuleBase_IWorkshop* theWorkshop)
{
  // the preview plane has been already created and displayed
  if (myPreviewIsDisplayed)
    return;

  // plane is visualized only if sketch plane is filled
  if (!PartSet_Tools::sketchPlane(theSketch).get())
    return;

  if (!myPlane) { // If planes are not created
    // Create Preview
    // selected linear face parameters
    AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
      (theSketch->data()->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
    if (aSelAttr)
      myShape = aSelAttr->value();

    if (!myShape.get()) {
      // Create Preview for default planes
      std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
          theSketch->data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
      std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
          theSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
      myShape = GeomAlgoAPI_FaceBuilder::squareFace(anOrigin->pnt(), aNormal->dir(),
        Config_PropManager::integer(SKETCH_TAB_NAME, "planes_size"));
    }
    myPlane = createPreviewPlane();
  }

  XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
  aDisp->displayAIS(myPlane, true, 1/*shaded*/, false);
  myPreviewIsDisplayed = true;
}

AISObjectPtr PartSet_PreviewSketchPlane::createPreviewPlane()
{
  if (myPlane.get()) {
    myPlane->createShape(myShape);
    return myPlane;
  }
  else {
    AISObjectPtr aAIS = AISObjectPtr(new GeomAPI_AISObject());
    aAIS->createShape(myShape);
    std::vector<int> aColor = Config_PropManager::color("Visualization", "sketch_preview_plane");
    if (aColor.size() == 3)
      aAIS->setColor(aColor[0], aColor[1], aColor[2]);
    aAIS->setTransparensy(0.8);

    int aDispMode = 1; // shading
    Handle(AIS_InteractiveObject) anAISIO = aAIS->impl<Handle(AIS_InteractiveObject)>();
    if (!anAISIO.IsNull()) {
      anAISIO->Attributes()->SetFaceBoundaryDraw( Standard_True );
      anAISIO->SetDisplayMode(aDispMode);
    }
    return aAIS;
  }
}
