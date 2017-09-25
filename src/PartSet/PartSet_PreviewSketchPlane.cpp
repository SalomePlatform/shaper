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

#include <GeomAlgoAPI_ShapeTools.h>

#include <XGUI_Tools.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>

#include <Config_PropManager.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_SketchEntity.h>

#include <BRepBndLib.hxx>

double maximumSize(double theXmin, double theYmin, double theZmin,
                   double theXmax, double theYmax, double theZmax)
{
  double aSize = fabs(theXmax - theXmin);
  double aSizeToCompare = fabs(theYmax - theYmin);
  if (aSizeToCompare > aSize)
    aSize = aSizeToCompare;
  aSizeToCompare = fabs(theZmax - theZmin);
  if (aSizeToCompare > aSize)
    aSize = aSizeToCompare;

  return aSize;
}

PartSet_PreviewSketchPlane::PartSet_PreviewSketchPlane()
 : mySketchDisplayed(false), myOtherSketchSize(0)
{
}

void PartSet_PreviewSketchPlane::setOtherSketchParameters(const GeomShapePtr& theOtherSketchFace)
{
  myOtherSketchOrigin = std::shared_ptr<GeomAPI_Pnt>();
  myOtherSketchSize = 0;
  if (!theOtherSketchFace)
    return;

  getShapeParameters(theOtherSketchFace, myOtherSketchOrigin, myOtherSketchSize);
}

void PartSet_PreviewSketchPlane::eraseSketchPlane(ModuleBase_IWorkshop* theWorkshop,
                                                  const bool isClearPlane)
{
  if (mySketchDisplayed) {
    XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
    aDisp->eraseAIS(myPlane, false);
    if (isClearPlane) {
      myPlane = NULL;
      myOrigin = NULL;
      myNormal = NULL;
    }
    mySketchDisplayed = false;
  }
}

void PartSet_PreviewSketchPlane::createSketchPlane(const CompositeFeaturePtr& theSketch,
                                                   ModuleBase_IWorkshop* theWorkshop)
{
  if (mySketchDisplayed)
    return;

  // plane is visualized only if sketch plane is filled
  std::shared_ptr<GeomAPI_Pln> aPlane = PartSet_Tools::sketchPlane(theSketch);
  if (!aPlane.get())
    return;

  if (!myPlane) { // If planes are not created
    std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        theSketch->data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        theSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));

    // Create Preview
    // default planes parameters
    std::shared_ptr<GeomAPI_Pnt> anOriginPnt = anOrigin->pnt();
    double aSize = 10;//Config_PropManager::integer(SKETCH_TAB_NAME, "planes_size");
    // another sketch parameters
    if (myOtherSketchOrigin) {
      anOriginPnt = myOtherSketchOrigin;
      aSize = myOtherSketchSize;
      setOtherSketchParameters(GeomShapePtr());
    }
    else {
      // selected linear face parameters
      AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                                (theSketch->data()->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
      if (aSelAttr) {
        std::shared_ptr<GeomAPI_Shape> aSketchExternalFace = aSelAttr->value();
        if (aSketchExternalFace)
          getShapeParameters(aSketchExternalFace, anOriginPnt, aSize);
      }
    }
    double aSketchSize = getSketchBoundingBoxSize(theSketch, anOriginPnt);
    if (aSketchSize > 0)
      aSize = aSketchSize;

    myOrigin = anOriginPnt;
    myNormal = aNormal->dir();
    myPlane = createPreviewPlane(aSize);
  }

  XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
  aDisp->displayAIS(myPlane, true, 1/*shaded*/, false);
  mySketchDisplayed = true;
}

void PartSet_PreviewSketchPlane::updatePlaneSize(const CompositeFeaturePtr& theSketch,
                                                 ModuleBase_IWorkshop* theWorkshop)
{
  std::shared_ptr<GeomAPI_Pnt> anOriginPnt;
  double aSize = getSketchBoundingBoxSize(theSketch, anOriginPnt);
  if (aSize <= 0)
    return;
  myOrigin = anOriginPnt;
  createPreviewPlane(aSize);
}

AISObjectPtr PartSet_PreviewSketchPlane::createPreviewPlane(double theSize)
{
  std::vector<int> aYZRGB(3, 0);
#ifdef SET_PLANES_COLOR_IN_PREFERENCES
    aYZRGB = Config_PropManager::color("Visualization", "yz_plane_color");
#else
    aYZRGB[0] = 225;
#endif
    int aR[] = {aYZRGB[0], aYZRGB[1], aYZRGB[2]};

  std::shared_ptr<GeomAPI_Shape> aFace =
    GeomAlgoAPI_FaceBuilder::squareFace(myOrigin, myNormal, theSize);

  if (myPlane.get()) {
    myPlane->createShape(aFace);
    return myPlane;
  }
  else {
    AISObjectPtr aAIS = AISObjectPtr(new GeomAPI_AISObject());
    aAIS->createShape(aFace);
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

void PartSet_PreviewSketchPlane::getShapeParameters(const GeomShapePtr& theShape,
                                                    std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                                                    double& theSize)
{
  theOrigin = GeomAlgoAPI_ShapeTools::centreOfMass(theShape);

  // Create rectangular face close to the selected
  double aXmin, anYmin, aZmin, aXmax, anYmax, aZmax;
  theShape->computeSize(aXmin, anYmin, aZmin, aXmax, anYmax, aZmax);

  theSize = maximumSize(aXmin, anYmin, aZmin, aXmax, anYmax, aZmax);
}

double PartSet_PreviewSketchPlane::getSketchBoundingBoxSize(
                                 const std::shared_ptr<ModelAPI_CompositeFeature>& theSketch,
                                 std::shared_ptr<GeomAPI_Pnt>& theCentralPnt)
{
  if (!theSketch.get() || theSketch->numberOfSubs() == 0)
    return 0;

  Bnd_Box aBox;
  for (int aSubFeatureId = 0; aSubFeatureId < theSketch->numberOfSubs(); aSubFeatureId++) {
    FeaturePtr aFeature = theSketch->subFeature(aSubFeatureId);
    if (!aFeature.get())
      continue;

    std::list<ResultPtr> aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aResultIt;
    for (aResultIt = aResults.begin(); aResultIt != aResults.end(); ++aResultIt) {
      ResultPtr aResult = *aResultIt;
      std::shared_ptr<GeomAPI_Shape> aShapePtr = aResult->shape();
      if (aShapePtr.get()) {
        TopoDS_Shape aShape = aShapePtr->impl<TopoDS_Shape>();
        if (aShape.IsNull())
          continue;
        BRepBndLib::Add(aShape, aBox);
      }
    }
  }
  if (aBox.IsVoid())
    return 0;

  double aXmin, aXmax, anYmin, anYmax, aZmin, aZmax;
  aBox.Get(aXmin, anYmin, aZmin, aXmax, anYmax, aZmax);

  double aSize = maximumSize(aXmin, anYmin, aZmin, aXmax, anYmax, aZmax);
  if (aSize > 0) {
    gp_Pnt aCentre(aXmax-fabs(aXmax-aXmin)/2., anYmax-fabs(anYmax-anYmin)/2.,
                   aZmax - fabs(aZmax-aZmin)/2.);
    theCentralPnt = std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCentre.X(), aCentre.Y(),
                                                                 aCentre.Z()));
  }
  return aSize;
}