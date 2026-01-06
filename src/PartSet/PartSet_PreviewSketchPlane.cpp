// Copyright (C) 2014-2026  CEA, EDF
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
#include "PartSet_SketcherMgr.h"
#include "PartSet_Tools.h"

#include <ModuleBase_IWorkshop.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_CompositeFeature.h>

#include <GeomAPI_AISObject.h>
#include <GeomAPI_Face.h>

#include <XGUI_Tools.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>

#include <Config_PropManager.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_SketchEntity.h>

#include <ModuleBase_IViewer.h>
#include <Aspect_Grid.hxx>
#include <Geom_Plane.hxx>
#include <AIS_PlaneTrihedron.hxx>
#include <Quantity_Color.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>

#include <cmath>
#include <algorithm>
#include <limits>
#include <QString>

#define PI 3.14159265358979323846

#ifdef SKETCH_ACCESSORY_DBG
#include <iostream>
#include <sstream>
#include <string>
const std::string  PREFIX  =  "PreviewSketchPlane: ";
const std::wstring WPREFIX = L"PreviewSketchPlane: ";
#endif
/*static*/ bool PartSet_PreviewSketchPlane::SketchAccessoryDbg(const QString& theString)
{
#ifdef SKETCH_ACCESSORY_DBG
  std::wcout << WPREFIX << theString.toStdWString() << std::endl;
  return true;
#else
  return false;
#endif
}
/*static*/ bool PartSet_PreviewSketchPlane::SketchAccessoryDbg(const char* src)
{
#ifdef SKETCH_ACCESSORY_DBG
  std::wcout << WPREFIX << std::wstring(src, src + strlen(src)) << std::endl;
  return true;
#else
  return false;
#endif;
}
/*static*/ bool PartSet_PreviewSketchPlane::SketchAccessoryDbg(const char* theCSDescription, const gp_Ax3& theCS)
{
#ifdef SKETCH_ACCESSORY_DBG
  std::stringstream s;
  s << PREFIX << theCSDescription << "\n";
  theCS.DumpJson(s);
  s << "\n";
  PartSet_PreviewSketchPlane::SketchAccessoryDbg(QString::fromStdString(s.str()));
  return true;
#else
  return false;
#endif
}
/*static*/ bool PartSet_PreviewSketchPlane::SketchAccessoryDbg(const char* theCSDescription, const gp_Pnt& thePnt)
{
#ifdef SKETCH_ACCESSORY_DBG
  std::stringstream s;
  s << PREFIX << theCSDescription;
  thePnt.DumpJson(s);
  s << "\n";
  PartSet_PreviewSketchPlane::SketchAccessoryDbg(QString::fromStdString(s.str()));
  return true;
#else
  return false;
#endif
}
/*static*/ bool PartSet_PreviewSketchPlane::SketchAccessoryDbg(const char* theCSDescription, const gp_Trsf& theMatrix)
{
#ifdef SKETCH_ACCESSORY_DBG
  std::stringstream s;
  s << PREFIX << theCSDescription << "\n";
  theMatrix.DumpJson(s);
  s << "\n";
  PartSet_PreviewSketchPlane::SketchAccessoryDbg(QString::fromStdString(s.str()));
  return true;
#else
  return false;
#endif
}


PartSet_PreviewSketchPlane::PartSet_PreviewSketchPlane(PartSet_SketcherMgr* theManager)
 : mySketcherMgr(theManager), myValid(false),
   mySketchDimensions(0, 0), mySketchDefaultSize(0),
   myShowTrihedron(false), myTrihedron(new AIS_PlaneTrihedron(new Geom_Plane(1, 0, 0, 0))),
   myShowSubstrate(false), mySubstrate(nullptr),
   myGridType(PartSet_Tools::SketchPlaneGridType::No), myGridDrawMode(Aspect_GridDrawMode::Aspect_GDM_Lines),
   mySnappingMode(GridSnappingMode::SnapAnyway),
   myRectangularGridSteps(1, 1), myCircularGridRadialStep(1)
{
  myTrihedron->SetXLabel("X'");
  myTrihedron->SetYLabel("Y'");
  myTrihedron->SetColor(Quantity_Color(Quantity_NameOfColor::Quantity_NOC_MAGENTA)); // The color does not coincide with colors of world axes.
}

void PartSet_PreviewSketchPlane::savePreferencesIntoSketchData(std::shared_ptr<ModelAPI_CompositeFeature> theSketch) const
{
  if (!theSketch || !this->isValid())
    return;

  PartSet_Tools::sketchPlaneAxesEnabled(theSketch)->setValue(myShowTrihedron);
  PartSet_Tools::sketchPlaneSubstrateEnabled(theSketch)->setValue(myShowSubstrate);
  PartSet_Tools::setSketchPlaneGridType(theSketch, myGridType);

  saveRectangularGridPreferencesIntoSketchData(theSketch);
}

void PartSet_PreviewSketchPlane::saveRectangularGridPreferencesIntoSketchData(std::shared_ptr<ModelAPI_CompositeFeature> theSketch) const
{
  if (!theSketch || !this->isValid())
    return;

  PartSet_Tools::sketchPlaneRectangularGridStepX(theSketch)->setValue(myRectangularGridSteps.first);
  PartSet_Tools::sketchPlaneRectangularGridStepY(theSketch)->setValue(myRectangularGridSteps.second);

  PartSet_Tools::sketchPlaneRectangularGridOffsetAngle(theSketch)->setValue(myRectangularGridOffsetAngle);
  PartSet_Tools::sketchPlaneRectangularGridOffsetX(theSketch)->setValue(myRectangularGridTransOffset.first);
  PartSet_Tools::sketchPlaneRectangularGridOffsetY(theSketch)->setValue(myRectangularGridTransOffset.second);
}

void PartSet_PreviewSketchPlane::saveCircularGridPreferencesIntoSketchData(std::shared_ptr<ModelAPI_CompositeFeature> theSketch) const
{
  if (!theSketch || !this->isValid())
    return;

  PartSet_Tools::sketchPlaneCircularGridStepR(theSketch)->setValue(myCircularGridRadialStep);
  PartSet_Tools::sketchPlaneCircularGridNumOfAngSegments(theSketch)->setValue(myCircularGridNumOfAngularSections);

  PartSet_Tools::sketchPlaneCircularGridOffsetAngle(theSketch)->setValue(myCircularGridOffsetAngle);
  PartSet_Tools::sketchPlaneCircularGridOffsetX(theSketch)->setValue(myCircularGridTransOffset.first);
  PartSet_Tools::sketchPlaneCircularGridOffsetY(theSketch)->setValue(myCircularGridTransOffset.second);
}

void PartSet_PreviewSketchPlane::setCSAndSize(const gp_Ax3& theCS, double theSize)
{
  SketchAccessoryDbg("setCSAndSize(_)");

  mySketchCS = theCS;
  mySketchDefaultSize = std::abs(theSize);
  mySketchDimensions.first  = mySketchDefaultSize;
  mySketchDimensions.second = mySketchDefaultSize;

  configureTrihedron();

  if (!mySubstrate)
    initSubstrate(nullptr);
  else {
    const auto face = GeomAlgoAPI_FaceBuilder::planarRectangularFace(mySketchCS, mySketchDimensions.first, mySketchDimensions.second);
    mySubstrate->createShape(face);
  }

  reconfigureGrid();
  myValid = true;
}

bool PartSet_PreviewSketchPlane::setAllUsingSketch(std::shared_ptr<ModelAPI_CompositeFeature> theSketch)
{
  auto makeSketchFace = [&](const std::shared_ptr<GeomAPI_Shape> sketchShape) -> std::shared_ptr<GeomAPI_Face> {
    // Check if a user explicitly set the size of the view.
    if (isUseSizeOfView() && (mySketchDefaultSize > Precision::Confusion())) {
      const std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(sketchShape));
      const std::shared_ptr<GeomAPI_Pln> aPlane = aFace->getPlane();
      if (aPlane.get()) {
        double anA, aB, aC, aD;
        aPlane->coefficients(anA, aB, aC, aD);
        const std::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
        std::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
        const std::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
        aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
        const std::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
        return GeomAlgoAPI_FaceBuilder::squareFace(anOrigPnt, aNormDir, mySketchDefaultSize);
      }
    }

    // Use selected shape as is
    return sketchShape->face();;
  };

  SketchAccessoryDbg("setAllUsingSketch(theSketch)");

  if (!theSketch || !PartSet_Tools::sketchPlane(theSketch)) {
    SketchAccessoryDbg("invalid sketch - nullptr or normal/center is undefined.");
    setInvalid();
    return false;
  }

  bool sketchIsBlank = false;
  const auto aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theSketch->data()->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
  if (aSelAttr) {
    std::shared_ptr<GeomAPI_Shape> sketchShape;
    if (aSelAttr->value()) {
      sketchShape = aSelAttr->value();
      SketchAccessoryDbg("aSelAttr->value()");
    }
    else if (aSelAttr->context()) {
      sketchShape = aSelAttr->context()->shape();
      SketchAccessoryDbg("aSelAttr->context()->shape()");
    }

    if (!sketchShape) {
      sketchIsBlank = true;
    }
    else {
      if (!sketchShape->isPlanar()) {
        SketchAccessoryDbg("invalid sketch - shape is not planar.");
        setInvalid();
        return false;
      }

      mySketchDefaultSize = PartSet_Tools::sketchPlaneDefaultSize(theSketch)->value();
      const std::shared_ptr<GeomAPI_Face> sketchFace = makeSketchFace(sketchShape);
      if (!sketchFace) {
        SketchAccessoryDbg("invalid sketch - shape is not face.");
        setInvalid();
        return false;
      }

      { // Define CS as one at the center (Uc, Vc) of parametric domain.
        // If the planar surface is non-convex, the center of parametric domain may
        // lay outside of the surface. If X'(U, V) or Y'(U, V) are non-linear,
        // X(Uc, Vc), Y(Uc, Vc) or its 1st derivative may not be defined.
        // But:
        //    1) The sketch widget only accepts Planes to start sketch.
        //    2) The set of planar surfaces, being used in the app, does not include those described above.
        double UMax, UMin, VMax, VMin;
        const bool success = sketchFace->optimalBounds(UMin, UMax, VMin, VMax);
        if (!success) {
          SketchAccessoryDbg("Can't get sketch CS: UV-domain is undefined.");
          setInvalid();
          return false;
        }

        // X' and Y' directions and sketch center in the sketch->data() are not coincide with
        // ones of the surface the sketch is started on.
        const auto sketchFaceCS = PartSet_Tools::getWorldCSAt(*sketchFace, (UMin+UMax)/2, (VMin+VMax)/2);
        if (!sketchFaceCS.first) {
          SketchAccessoryDbg("Can't get sketch CS: sketch face is not 1-differentiable as function of U and V at the center of UV-domain.");
          setInvalid();
          return false;
        }

        mySketchCS = sketchFaceCS.second;
      }

      { // Calculate sketch dimensions.
        // The only purpose of converting from UV to X'Y' during calculation of sketch dimensions
        // is to take into account scale factor or if the a non-orthogonal transformation is applied (if it even possible).
        const auto sketchFaceBox = PartSet_Tools::getBBoxAtCS(*sketchFace, mySketchCS);
        if (!sketchFaceBox.first) {
          SketchAccessoryDbg("Can't find bounding box for sketch face.");
          setInvalid();
          mySketchDimensions.first  = getDefaultSize();
          mySketchDimensions.second = getDefaultSize();
        }
        else {
          // Here X and Y are X' and Y', and Z is sketch normal.
          double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
          sketchFaceBox.second.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

          mySketchDimensions.first  = std::abs(Xmax - Xmin);
          mySketchDimensions.second = std::abs(Ymax - Ymin);
        }
      }

      if (!mySubstrate)
        initSubstrate(sketchFace);
      else
        mySubstrate->createShape(sketchFace);

#ifdef SKETCH_ACCESSORY_DBG
      auto sketchCS = gp_Ax3();
      {
        const auto aData = theSketch->data();
        const auto aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
        const auto aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
        const auto aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SketchPlugin_Sketch::NORM_ID()));
        try {
          sketchCS = gp_Ax3(aC->pnt()->impl<gp_Pnt>(), aX->dir()->impl<gp_Dir>(), aNorm->dir()->impl<gp_Dir>());
        }
        catch(...) {
          SketchAccessoryDbg("Sketch data contains invalid basis.");
        }
      }

      SketchAccessoryDbg("Sketch CS (sketch data):", sketchCS);
      SketchAccessoryDbg("Sketch face CS:", mySketchCS);
      SketchAccessoryDbg(QString("Sketch dimensions: ") + QString::number(mySketchDimensions.first) + ", " +  QString::number(mySketchDimensions.second));
#endif
    }
  }

  if (sketchIsBlank) {
    SketchAccessoryDbg(QString("setAllUsingSketch(theSketch). Empty sketch"));
    try
    {
      const auto sketchOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        theSketch->data()->attribute(SketchPlugin_Sketch::ORIGIN_ID())
      );

      const auto sketchNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        theSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID())
      );

      const auto sketchXDirection = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        theSketch->data()->attribute(SketchPlugin_Sketch::DIRX_ID())
      );

      mySketchCS.SetLocation(sketchOrigin->pnt()->impl<gp_Pnt>());

      const gp_Dir normal = sketchNormal->dir()->impl<gp_Dir>();
      mySketchCS.SetDirection(normal);
      // Chi is synonym for X'.
      const gp_Dir chiDirection = sketchXDirection->dir()->impl<gp_Dir>();
      mySketchCS.SetXDirection(chiDirection);

      mySketchDefaultSize = PartSet_Tools::sketchPlaneDefaultSize(theSketch)->value();
      const double defaultSize = getDefaultSize();
      mySketchDimensions.first  = defaultSize;
      mySketchDimensions.second = defaultSize;
    }
    catch(...) {
      SketchAccessoryDbg("Invalid basis in sketch data.");
      setInvalid();
      return false;
    }
  }

  configureTrihedron();

  if (sketchIsBlank) {
    if (!mySubstrate)
      initSubstrate(nullptr);
    else {
      const auto face = GeomAlgoAPI_FaceBuilder::planarRectangularFace(mySketchCS, mySketchDimensions.first, mySketchDimensions.second);
      mySubstrate->createShape(face);
    }
  }

  myValid = true;
  showAxes(PartSet_Tools::sketchPlaneAxesEnabled(theSketch)->value());
  showSubstrate(PartSet_Tools::sketchPlaneSubstrateEnabled(theSketch)->value());
  reconfigureGridUsingSketch(theSketch);
  return true;
}

bool PartSet_PreviewSketchPlane::reconfigureGrid()
{
  return reconfigureGridUsingSketch(nullptr);
}

bool PartSet_PreviewSketchPlane::reconfigureGridUsingSketch(std::shared_ptr<ModelAPI_CompositeFeature> theSketch)
{
  static const auto MARKER_COLOR = Quantity_Color(Quantity_NameOfColor::Quantity_NOC_TOMATO);
  static const auto MARKER_SIZE = 2.0;
  static const opencascade::handle<Graphic3d_AspectMarker3d> MARKER = new Graphic3d_AspectMarker3d(Aspect_TypeOfMarker::Aspect_TOM_BALL, MARKER_COLOR, MARKER_SIZE);
  static const double MARGINS = 1.005; // Makes grid surface bigger to display marginal grid lines.

  if (!isValid())
    return myGridType == PartSet_Tools::SketchPlaneGridType::No;

  const auto aV3DViewer = getV3DViewer();
  if (!aV3DViewer) {
    SketchAccessoryDbg("can't retrieve V3d_Viewer to configure grid.");
    return false;
  }

  const auto dimensions = getDimensions();

  if (theSketch) {
    { // Rectangular grid.
      const double stepXAttrVal = PartSet_Tools::sketchPlaneRectangularGridStepX(theSketch)->value();
      myRectangularGridSteps.first = stepXAttrVal < 0 ? dimensions.first  / PartSet_PreviewSketchPlane::DEFAULT_RELATIVE_STEP_INVERSE : stepXAttrVal;

      const double stepYAttrVal = PartSet_Tools::sketchPlaneRectangularGridStepY(theSketch)->value();
      myRectangularGridSteps.second = stepYAttrVal < 0 ? dimensions.second / PartSet_PreviewSketchPlane::DEFAULT_RELATIVE_STEP_INVERSE : stepYAttrVal;

      myRectangularGridTransOffset.first  = PartSet_Tools::sketchPlaneRectangularGridOffsetX(theSketch)->value();
      myRectangularGridTransOffset.second = PartSet_Tools::sketchPlaneRectangularGridOffsetY(theSketch)->value();
      myRectangularGridOffsetAngle = PartSet_Tools::sketchPlaneRectangularGridOffsetAngle(theSketch)->value();
    }

    { // Circular grid
      myCircularGridTransOffset.first  = PartSet_Tools::sketchPlaneCircularGridOffsetX(theSketch)->value();
      myCircularGridTransOffset.second = PartSet_Tools::sketchPlaneCircularGridOffsetY(theSketch)->value();
      myCircularGridOffsetAngle = PartSet_Tools::sketchPlaneCircularGridOffsetAngle(theSketch)->value();

      const double stepRAttrVal = PartSet_Tools::sketchPlaneCircularGridStepR(theSketch)->value();
      myCircularGridRadialStep = stepRAttrVal < 0 ?
        std::min(dimensions.first, dimensions.second) / PartSet_PreviewSketchPlane::DEFAULT_RELATIVE_STEP_INVERSE * 2 :
        stepRAttrVal;

      const int aNAS = PartSet_Tools::sketchPlaneCircularGridNumOfAngSegments(theSketch)->value();
      myCircularGridNumOfAngularSections = aNAS <= 0 ? PartSet_PreviewSketchPlane::DEFAULT_NUM_OF_ANGULAR_SECTIONS : aNAS;
    }

    myGridType = PartSet_Tools::getSketchPlaneGridType(theSketch);
  }

  aV3DViewer->SetGridEcho(MARKER);

  if (myGridType == PartSet_Tools::SketchPlaneGridType::Rectangular) {
    static const double MAX = std::numeric_limits<double>::max();

    auto steps = myRectangularGridSteps;
    if (steps.first <= Precision::Confusion())
      steps.first = MAX;

    if (steps.second <= Precision::Confusion())
      steps.second = MAX;

    // Chi and Upsilon are synonyms for X' and Y'.
    const double offsetChi     = steps.first  < MAX ? std::remainder(myRectangularGridTransOffset.first , steps.first)  : myRectangularGridTransOffset.first;
    const double offsetUpsilon = steps.second < MAX ? std::remainder(myRectangularGridTransOffset.second, steps.second) : myRectangularGridTransOffset.second;
    const double offsetAngleRad = myRectangularGridOffsetAngle * PI / 180;

    aV3DViewer->SetPrivilegedPlane(mySketchCS);
    aV3DViewer->SetRectangularGridValues(offsetChi, offsetUpsilon, steps.first, steps.second, offsetAngleRad);
#define V3DVIEWER_RECTANGULAR_GRID_OFFSET_BUGS
#ifdef V3DVIEWER_RECTANGULAR_GRID_OFFSET_BUGS
    /*
    How grid offsets should work? Imagine an invisible rectangular frame - only those elements of the the grid must be visible,
    which are inside of the frame.
    If an offset is adjusted, the frame remains intact and stays in place, but elements of the grid are shifted/rotated
    appropriate to the offset direction and value.
    The dimensions of the frame are ones of 2D bounding box of the sketch face.

    V3d_Viewer::SetRectangularGridGraphicValues adjusts grid elements' position, but also translates/rotates the frame of the grid!
    Following equations enlarge grid dimensions, so that the entire surface of the 2D BBox is covered with grid.
    */

    const auto outerDims = PartSet_PreviewSketchPlane::dimsOfRectangleFittingRectangle(
      dimensions.first, dimensions.second, -offsetAngleRad, offsetChi, offsetUpsilon
    );

    aV3DViewer->SetRectangularGridGraphicValues(outerDims.first / 2 * MARGINS, outerDims.second / 2 * MARGINS, 0);

    {
      const int chiDim     = outerDims.first  / 2 * MARGINS / steps.first  >= 1 ? 1 : 0;
      const int upsilonDim = outerDims.second / 2 * MARGINS / steps.second >= 1 ? 1 : 0;
      myRectangularGridDimOfNodeSpace = chiDim + upsilonDim;
    }
#else
    aV3DViewer->SetRectangularGridGraphicValues(dimensions.first / 2 * MARGINS, dimensions.second / 2 * MARGINS, 0);

    {
      const int chiDim     = dimensions.first  / 2 * MARGINS / steps.first  >= 1 ? 1 : 0;
      const int upsilonDim = dimensions.second / 2 * MARGINS / steps.second >= 1 ? 1 : 0;
      myRectangularGridDimOfNodeSpace = chiDim + upsilonDim;
    }
#endif
  }
  else if (myGridType == PartSet_Tools::SketchPlaneGridType::Circular) {
    const int& aNAS = myCircularGridNumOfAngularSections;
    const int divisionNumber = aNAS == 1 ? 0 : aNAS / 2 + aNAS % 2; // V3d_Viewer::SetCircularGridValues is weird.

    aV3DViewer->SetPrivilegedPlane(mySketchCS);
    aV3DViewer->SetCircularGridValues(
      myCircularGridTransOffset.first, myCircularGridTransOffset.second,
      myCircularGridRadialStep > Precision::Confusion() ? myCircularGridRadialStep : std::numeric_limits<double>::max(),
      divisionNumber,
      myCircularGridOffsetAngle * PI / 180
    );

    /* Circles of circular grid are not circles, but regular polygons and cover less surface than circles. */
    const double R = getCircularGridRaduis(dimensions);
    int numOfEdges = aNAS < 12 ? 12 : aNAS + aNAS % 2;
    const double polyR = R / std::cos(PI / numOfEdges) * MARGINS;
    aV3DViewer->SetCircularGridGraphicValues(polyR, 0);

    {
      const int angularDim = divisionNumber > 2 ? 2 : 1;
      const int radialDim = myCircularGridRadialStep > Precision::Confusion() && polyR / myCircularGridRadialStep >=1 ? 1 : 0;
      myRectangularGridDimOfNodeSpace = angularDim * radialDim;
    }
  }

  if (theSketch) {
    if (myGridType == PartSet_Tools::SketchPlaneGridType::No)
    {
      // Ensure the Grid is set, BEFORE we can deactivate it.
      if (aV3DViewer->IsGridActive())
        aV3DViewer->DeactivateGrid();
    }
    else {
      Aspect_GridType type = myGridType == PartSet_Tools::SketchPlaneGridType::Rectangular ?
        Aspect_GridType::Aspect_GT_Rectangular : Aspect_GridType::Aspect_GT_Circular;

      aV3DViewer->ActivateGrid(type, myGridDrawMode);
    }
  }

  return true;
}

bool PartSet_PreviewSketchPlane::showAxes(bool theShow)
{
  if (!isValid())
    return !theShow;

  XGUI_Displayer* const displayer = mySketcherMgr->workshop()->displayer();
  if (theShow)
    displayer->displayAIS(myTrihedron, false /*load object in selection*/, 1 /*shaded*/, false /*update viewer*/);
  else
    displayer->eraseAIS(myTrihedron, false /*update viewer*/);

  myShowTrihedron = theShow;
  return true;
}

bool PartSet_PreviewSketchPlane::showSubstrate(bool theShow)
{
  if (!isValid())
    return !theShow;

  XGUI_Displayer* const displayer = mySketcherMgr->workshop()->displayer();
  if (theShow)
    displayer->displayAIS(mySubstrate, false /*load object in selection*/, 1 /*shaded*/, false /*update viewer*/);
  else
    displayer->eraseAIS(mySubstrate, false /*update viewer*/);

  myShowSubstrate = theShow;
  return true;
}

bool PartSet_PreviewSketchPlane::setGridType(PartSet_Tools::SketchPlaneGridType::Enum theType)
{
  if (!isValid())
    return theType == PartSet_Tools::SketchPlaneGridType::No;

  const auto aV3DViewer = getV3DViewer();
  if (!aV3DViewer) {
    SketchAccessoryDbg("can't retrieve V3d_Viewer to show/hide grid.");
    return false;
  }

  myGridType = theType;
  reconfigureGrid();

  if (myGridType == PartSet_Tools::SketchPlaneGridType::No)
  {
    if (aV3DViewer->IsGridActive())
      aV3DViewer->DeactivateGrid();
  }
  else {
    Aspect_GridType type = myGridType == PartSet_Tools::SketchPlaneGridType::Rectangular ?
      Aspect_GridType::Aspect_GT_Rectangular : Aspect_GridType::Aspect_GT_Circular;

    aV3DViewer->ActivateGrid(type, myGridDrawMode);
  }

  return true;
}

void PartSet_PreviewSketchPlane::hideAll()
{
  showAxes(false);
  showSubstrate(false);
  setGridType(PartSet_Tools::SketchPlaneGridType::No);
}

std::pair<double, double> PartSet_PreviewSketchPlane::getDimensions() const
{
  const double defaultSize = getDefaultSize();
  // Chi and Upsilon are synonyms for X' and Y'.
  const bool chiSizeOk = mySketchDimensions.first > Precision::Confusion();
  const bool upsilonSizeOk = mySketchDimensions.second > Precision::Confusion();

  return std::pair<double, double>(
    chiSizeOk ? mySketchDimensions.first : upsilonSizeOk ? mySketchDimensions.second : defaultSize,
    upsilonSizeOk ? mySketchDimensions.second : chiSizeOk ? mySketchDimensions.first : defaultSize
  );
}

void PartSet_PreviewSketchPlane::resetRectangularGrid()
{
  const auto dimensions = getDimensions();
  myRectangularGridSteps.first  = dimensions.first  / PartSet_PreviewSketchPlane::DEFAULT_RELATIVE_STEP_INVERSE;
  myRectangularGridSteps.second = dimensions.second / PartSet_PreviewSketchPlane::DEFAULT_RELATIVE_STEP_INVERSE;
  myRectangularGridOffsetAngle = 0;
  myRectangularGridTransOffset.first  = 0;
  myRectangularGridTransOffset.second = 0;
}

void PartSet_PreviewSketchPlane::setRectangularGridStepX(double theStepX)
{
  myRectangularGridSteps.first  = theStepX;
}

void PartSet_PreviewSketchPlane::setRectangularGridStepY(double theStepY)
{
  myRectangularGridSteps.second = theStepY;
}

std::pair<std::pair<double, double>, double> PartSet_PreviewSketchPlane::getRectangularGridOffsets() const
{
  return std::pair<std::pair<double, double>, double>(myRectangularGridTransOffset, myRectangularGridOffsetAngle);
}

void PartSet_PreviewSketchPlane::setRectangularGridOffsetX(double theOffsetX)
{
  myRectangularGridTransOffset.first  = theOffsetX;
}

void PartSet_PreviewSketchPlane::setRectangularGridOffsetY(double theOffsetY)
{
  myRectangularGridTransOffset.second = theOffsetY;
}

void PartSet_PreviewSketchPlane::setRectangularGridOffsetA(double theOffsetAngle)
{
  myRectangularGridOffsetAngle = theOffsetAngle;
}

void PartSet_PreviewSketchPlane::resetCircularGrid()
{
  const auto d = getDimensions();
  double R = std::sqrt(std::pow(d.first, 2) + std::pow(d.second, 2)) / 2;

  myCircularGridRadialStep = R / PartSet_PreviewSketchPlane::DEFAULT_RELATIVE_STEP_INVERSE;
  myCircularGridNumOfAngularSections = PartSet_PreviewSketchPlane::DEFAULT_NUM_OF_ANGULAR_SECTIONS;
  myCircularGridOffsetAngle = 0;
  myCircularGridTransOffset.first  = 0;
  myCircularGridTransOffset.second = 0;
}

void PartSet_PreviewSketchPlane::setCircularGridRadialStep(double theStep)
{
  myCircularGridRadialStep = theStep;
}

void PartSet_PreviewSketchPlane::setCircularGridNumOfAngularSegments(int theNum)
{
  myCircularGridNumOfAngularSections = theNum;
}

std::pair<double, int> PartSet_PreviewSketchPlane::getCircularGrid_dR_and_NAS() const
{
  return std::pair<double, int>(myCircularGridRadialStep, myCircularGridNumOfAngularSections);
}

void PartSet_PreviewSketchPlane::setCircularGridOffsetX(double theOffset)
{
  myCircularGridTransOffset.first = theOffset;
}

void PartSet_PreviewSketchPlane::setCircularGridOffsetY(double theOffset)
{
  myCircularGridTransOffset.second = theOffset;
}

void PartSet_PreviewSketchPlane::setCircularGridOffsetA(double theOffset)
{
  myCircularGridOffsetAngle = theOffset;
}

std::pair<std::pair<double, double>, double> PartSet_PreviewSketchPlane::getCircularGridOffsets() const
{
  return std::pair<std::pair<double, double>, double>(myCircularGridTransOffset, myCircularGridOffsetAngle);
}

void PartSet_PreviewSketchPlane::configureTrihedron()
{
  const opencascade::handle<Geom_Plane> aGeom_Plane = new Geom_Plane(mySketchCS);
  const auto dimensions = getDimensions();
  myTrihedron->SetComponent(aGeom_Plane);
  myTrihedron->SetLength(std::min(dimensions.first, dimensions.second) / 2);
}

void PartSet_PreviewSketchPlane::initSubstrate(std::shared_ptr<GeomAPI_Face> theFace)
{
  if (!theFace) {
    const auto dimensions = getDimensions();
    theFace = GeomAlgoAPI_FaceBuilder::planarRectangularFace(mySketchCS, dimensions.first, dimensions.second);
  }

  AISObjectPtr aAIS = AISObjectPtr(new GeomAPI_AISObject());
  aAIS->createShape(theFace);
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
  mySubstrate = std::move(aAIS);
}

opencascade::handle<V3d_Viewer> PartSet_PreviewSketchPlane::getV3DViewer() const
{
  const ModuleBase_IViewer* const viewer = mySketcherMgr->workshop()->salomeViewer();
  if (!viewer)
    return nullptr;

  return viewer->v3dViewer();
}

bool PartSet_PreviewSketchPlane::isValid() const
{
  return myValid;
}

void PartSet_PreviewSketchPlane::setInvalid()
{
  myValid = false;

  XGUI_Displayer* const displayer = mySketcherMgr->workshop()->displayer();

  displayer->eraseAIS(myTrihedron, false /*update viewer*/);
  myShowTrihedron = false;

  if (mySubstrate && myShowSubstrate) {
    displayer->eraseAIS(mySubstrate, false /*update viewer*/);
    myShowSubstrate = false;
  }

  if (myGridType != PartSet_Tools::SketchPlaneGridType::No) {
    ModuleBase_IViewer* aViewer = mySketcherMgr->workshop()->salomeViewer();
    const auto aV3DViewer = getV3DViewer();
    if (aV3DViewer) {
      if (aV3DViewer->IsGridActive())
        aV3DViewer->DeactivateGrid();
      myGridType = PartSet_Tools::SketchPlaneGridType::No;
    }
    else {
      SketchAccessoryDbg("can't retrieve V3d_Viewer to deactivate grid.");
    }
  }
}

double PartSet_PreviewSketchPlane::getDefaultSize() const {
  const double defaultSize = mySketchDefaultSize;
  if (defaultSize <= Precision::Confusion())
    return PartSet_PreviewSketchPlane::defaultSketchSize();

  return defaultSize;
}

double PartSet_PreviewSketchPlane::getCircularGridRaduis(const std::pair<double, double>& theBBox2Dimensions) const
{
  // Find big enough R to cover entire 2D-bounding of sketch face.
  const double& Cx = myCircularGridTransOffset.first;
  const double& Cy = myCircularGridTransOffset.second;
  const double& W = theBBox2Dimensions.first  / 2;
  const double& H = theBBox2Dimensions.second / 2;

  /* Distances from center of circular grid to vertices of sketch face 2D rectangular box. */
  double squareDistances[4];
  squareDistances[0] = std::pow(Cx - W, 2) + std::pow(Cy - H, 2);
  squareDistances[1] = std::pow(Cx + W, 2) + std::pow(Cy - H, 2);
  squareDistances[2] = std::pow(Cx - W, 2) + std::pow(Cy + H, 2);
  squareDistances[3] = std::pow(Cx + W, 2) + std::pow(Cy + H, 2);
  double R2 = squareDistances[3];
  for (int i = 0; i < 3; i++) {
    if (squareDistances[i] > R2)
      R2 = squareDistances[i];
  }

  return std::sqrt(R2);
}

/*static*/ double PartSet_PreviewSketchPlane::defaultSketchSize()
{
  const double defaultSize  = Config_PropManager::real(SKETCH_TAB_NAME, "planes_size");
  if (defaultSize <= Precision::Confusion())
    return PartSet_PreviewSketchPlane::DEFAULT_SKETCH_SIZE;

  return defaultSize;
}

/*static*/ const double PartSet_PreviewSketchPlane::DEFAULT_SKETCH_SIZE = 200;
/*static*/ const double PartSet_PreviewSketchPlane::DEFAULT_RELATIVE_STEP_INVERSE = 20;
/*static*/ const int PartSet_PreviewSketchPlane::DEFAULT_NUM_OF_ANGULAR_SECTIONS = 18;
/*static*/ const int PartSet_PreviewSketchPlane::SNAP_PROXIMITY_P = 10;

/*static*/ std::pair<double, double> PartSet_PreviewSketchPlane::dimsOfRectangleFittingRectangle(
  double wInner, double hInner, double a, double cX, double cY
) {
  const auto center = gp_Pnt2d(cX, cY);
  const auto dirX = gp_Dir2d(std::cos(a), std::sin(a));
  const gp_Ax2d csOuter = gp_Ax2d(center, dirX);

  gp_Pnt2d vertexInner[4] =
    {gp_Pnt2d(-wInner/2, -hInner/2), gp_Pnt2d(-wInner/2, hInner/2), gp_Pnt2d(wInner/2, -hInner/2), gp_Pnt2d(wInner/2, hInner/2)};

  gp_Trsf2d trsf;
  trsf.SetTransformation(csOuter);

  double maxXP = 0, minXN = 0, maxYP = 0, minYN = 0;
  for (int i = 0; i < 4; i++) {
    vertexInner[i].Transform(trsf);
    const auto& v = vertexInner[i];
    if (v.X() > maxXP)
      maxXP = v.X();
    else if (v.X() < minXN)
      minXN = v.X();

    if (v.Y() > maxYP)
      maxYP = v.Y();
    else if (v.Y() < minYN)
      minYN = v.Y();
  }
  return std::pair<double, double>(std::max(maxXP, -minXN) * 2, std::max(maxYP, -minYN) * 2);
}