// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_SketchDrawer_H_
#define SketchPlugin_SketchDrawer_H_

#include "SketchPlugin.h"
#include <ModelAPI_Feature.h>

/// \class SketchPlugin_SketchDrawer
/// \ingroup Plugins
/// \brief Feature for creation of a sketch on selected base object and plane. It tries
///  to make sketch elements equal to edges and vertices of the objects that belong to
///  the selected plane.
class SketchPlugin_SketchDrawer: public ModelAPI_Feature
{
 public:
  /// A constructor
  SketchPlugin_SketchDrawer();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_FEATURE_ID("SketchDrawer");
    return MY_FEATURE_ID;
  }

  /// Attribute name of the base shape selected.
  inline static const std::string& BASE_ID()
  {
    static const std::string ID("base_shape");
    return ID;
  }
  /// Attribute name of the plane selected.
  inline static const std::string& PLANE_ID()
  {
    static const std::string ID("plane");
    return ID;
  }
  /// Attribute name of the add-dimensions flag.
  inline static const std::string& ADD_DIMENSIONS_ID()
  {
    static const std::string ID("add_dimensions");
    return ID;
  }

  /// \return the kind of a feature.
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_SketchDrawer::ID();
    return MY_KIND;
  }

  /// Creates a new sketch.
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Reimplemented from ModelAPI_Feature::isMacro(). Means that feature is removed on apply.
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;}

  /// No preview is generated until it is applied.
  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;}
};

#endif
