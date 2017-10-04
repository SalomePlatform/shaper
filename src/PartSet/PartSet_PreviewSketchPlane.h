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

private:
  /// Create a square face by parameters
  std::shared_ptr<GeomAPI_AISObject> createPreviewPlane();

private:
  bool myPreviewIsDisplayed;
  std::shared_ptr<GeomAPI_AISObject> myPlane; //! visualized presentation
  std::shared_ptr<GeomAPI_Shape> myShape; //! current shape to be displayed
};

#endif