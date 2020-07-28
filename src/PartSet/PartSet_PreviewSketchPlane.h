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

#ifndef PartSet_PreviewSketchPlane_H
#define PartSet_PreviewSketchPlane_H

#include <GeomAPI_Pnt.h>

class GeomAPI_AISObject;
class GeomAPI_Shape;

class ModuleBase_IWorkshop;
class ModelAPI_CompositeFeature;

#include <memory>

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

  /// Erase preview planes
  /// \param theWorkshop the application workshop
  /// \param isClearPlane flag whether the plane, origin and normal should be nullified
  void eraseSketchPlane(ModuleBase_IWorkshop* theWorkshop, const bool isClearPlane = true);

  /// Show preview planes
  /// \param theSketch source sketch to initialize plane
  /// \param theWorkshop the application workshop
  void createSketchPlane(const std::shared_ptr<ModelAPI_CompositeFeature>& theSketch,
                         ModuleBase_IWorkshop* theWorkshop);

  /// Returns bounding box size covered the sketch sub-elements.
  /// If the sketch uses extenal face, it will not have default size and returns false.
  /// \param theSketch sources sketch
  /// \param [out] theSizeOfView maximum value in X, Y or Z direction
  /// \param theCentralPoint central point of the sketch sub features
  /// \return boolean value
  bool getDefaultSizeOfView(const std::shared_ptr<ModelAPI_CompositeFeature>& theSketch,
                            double& theSizeOfView,
                            std::shared_ptr<GeomAPI_Pnt>& theCentralPnt);

  /// Returns whether custom size of view is set
  /// \return boolean value
  bool isUseSizeOfView() const { return myIsUseSizeOfView; }

  /// Sets the size of default created face
  /// \param theSizeOfView value
  /// \param isUseSizeOfView state whether the size should be used
  void setSizeOfView(double theSizeOfView, bool isUseSizeOfView,
    const std::shared_ptr<GeomAPI_Pnt>& theCentralPoint = std::shared_ptr<GeomAPI_Pnt>());

  bool isPlaneCreated() const {
    return myPlane.get();
  }

  void displaySketchPlane(ModuleBase_IWorkshop* theWorkshop);

private:
  /// Create a square face by parameters
  std::shared_ptr<GeomAPI_AISObject> createPreviewPlane();

private:
  bool myPreviewIsDisplayed;
  std::shared_ptr<GeomAPI_AISObject> myPlane; //! visualized presentation
  std::shared_ptr<GeomAPI_Shape> myShape; //! current shape to be displayed
  std::shared_ptr<GeomAPI_Pnt> myViewCentralPoint; //! central point of the default view

  double mySizeOfView; //! size that should be used by creating a default face
  bool myIsUseSizeOfView; //! state if the size is custom or from preferences
  std::shared_ptr<GeomAPI_Pnt> myViewOrigin; //! origin point of sketch if default view is used
};

#endif