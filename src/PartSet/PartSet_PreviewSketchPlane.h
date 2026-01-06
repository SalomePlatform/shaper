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

#ifndef PartSet_PreviewSketchPlane_H
#define PartSet_PreviewSketchPlane_H

#include <memory>
#include <utility>
#include <Standard_Handle.hxx>
#include <Aspect_GridType.hxx>
#include <Aspect_GridDrawMode.hxx>
#include <ModelAPI_CompositeFeature.h>
#include <gp_Ax3.hxx>

#include "PartSet_Tools.h"

class PartSet_SketcherMgr;
class ModelAPI_CompositeFeature;
class GeomAPI_AISObject;
class GeomAPI_Face;
class AIS_PlaneTrihedron;
class V3d_Viewer;
class QString;
class gp_Trsf;

// #define SKETCH_ACCESSORY_DBG;

/**
* \class PartSet_PreviewSketchPlane
* \ingroup Modules
* Visualization of 2D-bluebrint' accessories: translucent rectangular substrate, basis axes, grid.
* All methods do not modify sketch data, unless otherwise is stated.
*/
class PartSet_PreviewSketchPlane
{
public:
  inline static bool SketchAccessoryDbg()
  {
#ifdef SKETCH_ACCESSORY_DBG
    return true;
#else
  return false;
#endif
  };

  static bool SketchAccessoryDbg(const QString& theString);
  static bool SketchAccessoryDbg(const char* theString);
  static bool SketchAccessoryDbg(const char* theCSDescription, const gp_Ax3& theCS);
  static bool SketchAccessoryDbg(const char* theCSDescription, const gp_Pnt& thePoint);
  static bool SketchAccessoryDbg(const char* theCSDescription, const gp_Trsf& theMatrix);

  enum GridSnappingMode {
    Off,
    SnapAnyway,
    SnapInProximity
  };

  /*! \param theManager must not be nullptr. */
  PartSet_PreviewSketchPlane(PartSet_SketcherMgr* theManager);
  PartSet_PreviewSketchPlane(const PartSet_PreviewSketchPlane&) = delete;
  PartSet_PreviewSketchPlane& operator=(const PartSet_PreviewSketchPlane&) = delete;
  ~PartSet_PreviewSketchPlane() = default;

  void savePreferencesIntoSketchData(std::shared_ptr<ModelAPI_CompositeFeature> theSketch) const;
  void saveRectangularGridPreferencesIntoSketchData(std::shared_ptr<ModelAPI_CompositeFeature> theSketch) const;
  void saveCircularGridPreferencesIntoSketchData(std::shared_ptr<ModelAPI_CompositeFeature> theSketch) const;

  /*! \brief Updates CS (coordinate system), dimensions and accessories' configuration. */
  void setCSAndSize(const gp_Ax3& theCS, double theSize);

  /*! \brief Updates CS (coordinate system), dimensions and accessories' configuration.
  Updates visibility of accessories - visibility preferences are also retrieved from theSketch data.
  If sketch data contains uninitialized grid steps, the method assigns default ones.
  For rectangular grid default step is a fraction of corresponding dimension.
  \returns true, on success (if theSketch is valid). */
  bool setAllUsingSketch(std::shared_ptr<ModelAPI_CompositeFeature> theSketch);

  /*! \brief Call it, after any preference of grid, except grid type, is changed using methods of this instance.
  Updates grid configuration. Does not affect visibility of grid.
  \returns true, if grid is successfully configured. */
  bool reconfigureGrid();

  /*! \brief Call it, after any preference of grid is changed in theSketch data directly.
  Updates grid configuration. Updates grid visibility - preference is also retrieved from theSketch data.
  If sketch data contains uninitialized grid steps, the method assigns default ones.
  For rectangular grid default step is a fraction of corresponding dimension.
  \returns true, if grid is successfully configured. */
  bool reconfigureGridUsingSketch(std::shared_ptr<ModelAPI_CompositeFeature> theSketch);

  /*! \returns true on success. */
  bool showAxes(bool theShow);
  /*! \returns true on success. */
  bool showSubstrate(bool theShow);
  /*! \brief Changes grid type and reconfigures grid. Shows grid, if theType != SketchPlaneGridType::No.
  \returns true on success. */
  bool setGridType(PartSet_Tools::SketchPlaneGridType::Enum theType);

  PartSet_Tools::SketchPlaneGridType::Enum getGridType() const { return myGridType; }

  /*! \brief Hides all accessories. */
  void hideAll();

  /*! \brief Substrate dimensions. Substrate dimension coincides with one of the sketch face, if the latter is non-zero. */
  std::pair<double, double> getDimensions() const;

  bool isShowAxes() const { return myShowTrihedron; }
  bool isShowSubstrate() const { return myShowSubstrate; }
  bool isShowGrid() const { return myGridType != PartSet_Tools::SketchPlaneGridType::No; }

  GridSnappingMode getGridSnappingMode() const { return mySnappingMode; };
  void setGridSnappingMode(GridSnappingMode theMode) { mySnappingMode = theMode; };

  /*! \brief Sets default steps and zero offsets. */
  void resetRectangularGrid();

  void setRectangularGridStepX(double theStepX);
  void setRectangularGridStepY(double theStepY);
  std::pair<double, double> getRectangularGridSteps() const { return myRectangularGridSteps; }

  void setRectangularGridOffsetX(double theOffsetX);
  void setRectangularGridOffsetY(double theOffsetY);
  void setRectangularGridOffsetA(double theOffsetAngle);
  std::pair<std::pair<double, double>, double> getRectangularGridOffsets() const;
  int getRectangularGridDimOfNodeSpace() const { return myRectangularGridDimOfNodeSpace; }

  /*! \brief Sets default steps and zero offsets. */
  void resetCircularGrid();

  void setCircularGridRadialStep(double theStep);
  void setCircularGridNumOfAngularSegments(int theNum);
  std::pair<double, int> getCircularGrid_dR_and_NAS() const;

  void setCircularGridOffsetX(double theOffset);
  void setCircularGridOffsetY(double theOffset);
  void setCircularGridOffsetA(double theOffset);
  std::pair<std::pair<double, double>, double> getCircularGridOffsets() const;
  int getCircularGridDimOfNodeSpace() const { return myCircularGridDimOfNodeSpace; }

  void setUseSizeOfView(const bool isUseSizeOfView) { myIsUseSizeOfView = isUseSizeOfView; }
  bool isUseSizeOfView() const { return myIsUseSizeOfView; }

private:
  void configureTrihedron();
  void initSubstrate(std::shared_ptr<GeomAPI_Face> theFace);

  opencascade::handle<V3d_Viewer> getV3DViewer() const;
  bool isValid() const;
  void setInvalid();

  /*! \returns Default size from sketch data (if > 0), or the default size from Config_PropManager (if > 0), or DEFAULT_SKETCH_SIZE. */
  double getDefaultSize() const;

  double getCircularGridRaduis(const std::pair<double, double>& theBBox2Dimensions) const;

  /*! \returns Dimensions of outer rectangle, center of which is shifted from center of inner rectangle by (cX, cY);
  outer rectangle is rotated by angle (counterclockwise is positive) relative to inner one. */
  static std::pair<double, double> dimsOfRectangleFittingRectangle(double wInner, double hInner, double angle, double cX, double cY);

public:
  /*! \returns Default size from Config_PropManager (if > 0), or DEFAULT_SKETCH_SIZE. */
  static double defaultSketchSize();

  static const double DEFAULT_RELATIVE_STEP_INVERSE;
  static const int DEFAULT_NUM_OF_ANGULAR_SECTIONS;
  static const int SNAP_PROXIMITY_P; /// Distance [pixel], at which snapping to grid engages in SnapInProximity mode.

private:
  /** Is used to create default substrate, if plane size is 0 both in the sketch data and in Config_PropManager. */
  static const double DEFAULT_SKETCH_SIZE;

  const PartSet_SketcherMgr* const mySketcherMgr;

  bool myValid;
  gp_Ax3 mySketchCS;
  std::pair<double, double> mySketchDimensions; // Width and height of sketch bounding box, aligned with sketch axes.
  double mySketchDefaultSize;
  bool myIsUseSizeOfView = false; //! state if the size is custom or from preferences

  bool myShowTrihedron;
  opencascade::handle<AIS_PlaneTrihedron> myTrihedron; // Visualization of sketch axes and origin.

  bool myShowSubstrate;
  std::shared_ptr<GeomAPI_AISObject> mySubstrate; // Translucent substrate.

  PartSet_Tools::SketchPlaneGridType::Enum myGridType;
  Aspect_GridDrawMode myGridDrawMode;
  GridSnappingMode mySnappingMode;

  std::pair<double, double> myRectangularGridSteps;
  std::pair<double, double> myRectangularGridTransOffset;
  double myRectangularGridOffsetAngle;
  int myRectangularGridDimOfNodeSpace; // Minimal dimension of space which can fit displayed grid nodes.

  double myCircularGridRadialStep;
  int    myCircularGridNumOfAngularSections;
  std::pair<double, double> myCircularGridTransOffset;
  double myCircularGridOffsetAngle;
  int myCircularGridDimOfNodeSpace; // Minimal dimension of space which can fit displayed grid nodes.
};

#endif