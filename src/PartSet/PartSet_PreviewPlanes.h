// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_PreviewPlanes.h
// Created:     22 Mar 2016
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_PreviewPlanes_H
#define PartSet_PreviewPlanes_H

#include <GeomAPI_AISObject.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_AISObject.h>

class ModuleBase_IWorkshop;

/// the plane edge width
#define SKETCH_WIDTH        "4"
/// face of the square-face displayed for selection of general plane
#define PLANE_SIZE          "200"

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

  /// Returns if the preview was displayed
  /// \param theWorkshop the application workshop
  /// \return boolean value
  bool isPreviewDisplayed() const { return myPreviewDisplayed; }

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