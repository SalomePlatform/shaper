// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef PartSet_PreviewPlanes_H
#define PartSet_PreviewPlanes_H

#include <GeomAPI_AISObject.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_AISObject.h>

class ModuleBase_IWorkshop;

/**
* \class PartSet_PreviewPlanes
* \ingroup Modules
* A class to show/hide sketch preview planes
*/
class PartSet_PreviewPlanes
{
public:
  /// Constructor
  PartSet_PreviewPlanes();

  ~PartSet_PreviewPlanes() {};

  /// Returns true if there is body visualized in the viewer
  /// \param theWorkshop the application workshop
  /// \return boolean value
  static bool hasVisualizedBodies(ModuleBase_IWorkshop* theWorkshop);

  /// Returns true if there is at least one Sketch visualized in the viewer
  /// \param theWorkshop the application workshop
  /// \return boolean value
  static bool hasVisualizedSketch(ModuleBase_IWorkshop* theWorkshop);

  /// Returns if the preview was displayed
  /// \return boolean value
  bool isPreviewDisplayed() const { return myPreviewDisplayed; }

  /// Returns true if the shape is one of the preview shapes
  /// \param theShape a shape to be checked
  /// \return boolean value
  bool isPreviewShape(std::shared_ptr<GeomAPI_Shape> theShape);

  /// Erase preview planes
  /// \param theWorkshop the application workshop
  void erasePreviewPlanes(ModuleBase_IWorkshop* theWorkshop);

  /// Show preview planes
  /// \param theWorkshop the application workshop
  void showPreviewPlanes(ModuleBase_IWorkshop* theWorkshop);

private:
     /// Create preview of planes for sketch plane selection
   /// \param theOrigin an origin of the plane
   /// \param theNorm a normal vector of the plane
   /// \param theRGB a color of plane presentation [r, g, b] array
  AISObjectPtr createPreviewPlane(std::shared_ptr<GeomAPI_Pnt> theOrigin,
                                  std::shared_ptr<GeomAPI_Dir> theNorm,
                                  const int theRGB[3]);

private:
  bool myPreviewDisplayed;

  AISObjectPtr myYZPlane;
  AISObjectPtr myXZPlane;
  AISObjectPtr myXYPlane;
};

#endif