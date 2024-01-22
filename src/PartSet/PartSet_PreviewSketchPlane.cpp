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

#include "PartSet_PreviewSketchPlane.h"
#include "PartSet_Tools.h"

#include <ModuleBase_IWorkshop.h>

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_AISObject.h>
#include <GeomAPI_Pnt.h>

#include <XGUI_Tools.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>

#include <Config_PropManager.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_SketchEntity.h>

#include <BRepBndLib.hxx>

PartSet_PreviewSketchPlane::PartSet_PreviewSketchPlane()
 : myPreviewIsDisplayed(false), mySizeOfView(0), myIsUseSizeOfView(false)
{
}

void PartSet_PreviewSketchPlane::eraseSketchPlane(ModuleBase_IWorkshop* theWorkshop,
                                                  const bool isClearPlane)
{
  if (myPreviewIsDisplayed) {
    XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
    aDisp->eraseAIS(myPlane, false);
    myPreviewIsDisplayed = false;
  }
  if (isClearPlane) clearPlanePreview();
}

void PartSet_PreviewSketchPlane::displaySketchPlane(ModuleBase_IWorkshop* theWorkshop)
{
  if (myPlane.get() && (!myPreviewIsDisplayed)) {
    XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
    aDisp->displayAIS(myPlane, false/*load object in selection*/, 1/*shaded*/, false);
    myPreviewIsDisplayed = true;
  }
}


void PartSet_PreviewSketchPlane::clearPlanePreview()
{
  myPlane = std::shared_ptr<GeomAPI_AISObject>();
  myShape = std::shared_ptr<GeomAPI_Shape>();
}


void PartSet_PreviewSketchPlane::createSketchPlane(const CompositeFeaturePtr& theSketch,
                                                   ModuleBase_IWorkshop* theWorkshop)
{
  // plane is visualized only if sketch plane is filled
  if (!PartSet_Tools::sketchPlane(theSketch).get())
    return;

  AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
    (theSketch->data()->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
  if (!aSelAttr)
    return;

  if (myShape.get() && myShape->isSame(aSelAttr->value()) && myPlane.get())
    return;

  XGUI_Displayer* aDisp = XGUI_Tools::workshop(theWorkshop)->displayer();
  if (myPreviewIsDisplayed) {
    aDisp->eraseAIS(myPlane, false);
  }

  // Create Preview
  // selected linear face parameters
  myShape = aSelAttr->value();
  // this case is needed by constructing sketch on a plane, where result shape is equal
  // to context result, therefore value() returns NULL and we should use shape of context.
  if (!myShape.get() && aSelAttr->context().get())
    myShape = aSelAttr->context()->shape();

  if (!myShape.get()) {
    // Create Preview for default planes
    std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        theSketch->data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        theSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));

    double aFaceSize = myIsUseSizeOfView ? mySizeOfView
      : Config_PropManager::real(SKETCH_TAB_NAME, "planes_size");
    if (aFaceSize <= Precision::Confusion())
      aFaceSize = 200;   // Set default value

    myShape = GeomAlgoAPI_FaceBuilder::squareFace(
      myViewCentralPoint.get() ? myViewCentralPoint : anOrigin->pnt(), aNormal->dir(), aFaceSize);
  }
  else if (myIsUseSizeOfView && (mySizeOfView > 0)) {
    std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(myShape));
    std::shared_ptr<GeomAPI_Pln> aPlane = aFace->getPlane();
    if (aPlane.get()) {
      double anA, aB, aC, aD;
      aPlane->coefficients(anA, aB, aC, aD);
      std::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
      std::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
      std::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
      aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
      std::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
      myShape = GeomAlgoAPI_FaceBuilder::squareFace(
        myViewCentralPoint.get() ? myViewCentralPoint : anOrigPnt, aNormDir, mySizeOfView);
    }
  }
  myPlane = createPreviewPlane();

  aDisp->displayAIS(myPlane, false/*load object in selection*/, 1/*shaded*/, false);
  myPreviewIsDisplayed = true;
}

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

bool PartSet_PreviewSketchPlane::getDefaultSizeOfView(
  const CompositeFeaturePtr& theSketch, double& theSizeOfView,
  std::shared_ptr<GeomAPI_Pnt>& theCentralPnt)
{
  if (!PartSet_Tools::sketchPlane(theSketch).get())
    return false;

  AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
    (theSketch->data()->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
  if (aSelAttr) {
    myShape = aSelAttr->value();
    // this case is needed by constructing sketch on a plane, where result shape is equal
    // to context result, therefore value() returns NULL and we should use shape of context.
    if (!myShape.get() && aSelAttr->context().get())
      myShape = aSelAttr->context()->shape();
  }

  if (myShape.get())
    return false;

  Bnd_Box aBox;
  int aNumberOfSubs = theSketch->numberOfSubs();
  for (int aSubFeatureId = 0; aSubFeatureId < aNumberOfSubs; aSubFeatureId++) {
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

  theSizeOfView = maximumSize(aXmin, anYmin, aZmin, aXmax, anYmax, aZmax);
  if (theSizeOfView > 0) {
    gp_Pnt aCentre(aXmax-fabs(aXmax-aXmin)/2., anYmax-fabs(anYmax-anYmin)/2.,
                   aZmax - fabs(aZmax-aZmin)/2.);
    theCentralPnt = std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCentre.X(), aCentre.Y(),
                                                                 aCentre.Z()));
  }
  return true;
}

void PartSet_PreviewSketchPlane::setSizeOfView(double theSizeOfView, bool isUseSizeOfView,
  const std::shared_ptr<GeomAPI_Pnt>& theCentralPoint)
{
  mySizeOfView = theSizeOfView;
  myIsUseSizeOfView = isUseSizeOfView;

  myViewCentralPoint = theCentralPoint;
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
      //anAISIO->SetInfiniteState(Standard_True);
      anAISIO->Attributes()->SetFaceBoundaryDraw( Standard_True );
      anAISIO->SetDisplayMode(aDispMode);
    }
    return aAIS;
  }
}
