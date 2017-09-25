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

#ifndef PartSet_PreviewSketchPlane_H
#define PartSet_PreviewSketchPlane_H

#include <GeomAPI_AISObject.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_AISObject.h>

class ModuleBase_IWorkshop;
class ModelAPI_CompositeFeature;

/**
* \class PartSet_PreviewSketchPlane
* \ingroup Modules
* A class to show/hide sketch preview plane
*/
class PartSet_PreviewSketchPlane
{
public:
  /// Constructor
  PartSet_PreviewSketchPlane();

  ~PartSet_PreviewSketchPlane() {};

  /// Returns if the sketch plane was displayed
  /// \return boolean value
  bool isSketchDisplayed() const { return mySketchDisplayed; }

  /// Sets parameters of another sketch to be used in createSketchPlane().
  /// \param theOtherSketchFace a shape of other selected sketch. It is a planar face.
  void setOtherSketchParameters(const std::shared_ptr<GeomAPI_Shape>& theOtherSketchFace);

  /// Erase preview planes
  /// \param theWorkshop the application workshop
  /// \param isClearPlane flag whether the plane, origin and normal should be nullified
  void eraseSketchPlane(ModuleBase_IWorkshop* theWorkshop, const bool isClearPlane = true);

  /// Show preview planes
  /// \param theSketch source sketch to initialize plane
  /// \param theWorkshop the application workshop
  void createSketchPlane(const std::shared_ptr<ModelAPI_CompositeFeature>& theSketch,
                         ModuleBase_IWorkshop* theWorkshop);

  /// Resize plane size by sketch content
  /// \param theSketch source sketch to build sketch sub features bounding box
  /// \param theWorkshop the application workshop
  void updatePlaneSize(const std::shared_ptr<ModelAPI_CompositeFeature>& theSketch,
                       ModuleBase_IWorkshop* theWorkshop);

private:
  /// Create a square face by parameters
  /// \param theSize a size of created square
  AISObjectPtr createPreviewPlane(double theSize);

  /// Finds origin and maximum size of the shape
  /// \param theShape source shape
  /// \param theOrigin a shape center of mass
  /// \param theSize maximum of delta X, Y or Z
  void getShapeParameters(const std::shared_ptr<GeomAPI_Shape>& theShape,
                          std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                          double& theSize);

  /// Calculate maximum bounding box size of sketch sub features
  /// \param theSketch source sketch to build sketch sub features bounding box
  /// \param theCentralPoint central point of the sketch sub features
  /// \return double value
  double getSketchBoundingBoxSize(const std::shared_ptr<ModelAPI_CompositeFeature>& theSketch,
                                  std::shared_ptr<GeomAPI_Pnt>& theCentralPnt);

private:
  bool mySketchDisplayed;
  AISObjectPtr myPlane;
  std::shared_ptr<GeomAPI_Pnt> myOrigin; //! an origin of the plane
  std::shared_ptr<GeomAPI_Dir> myNormal; //! a normal vector of the plane

  std::shared_ptr<GeomAPI_Pnt> myOtherSketchOrigin;
  double myOtherSketchSize;
};

#endif