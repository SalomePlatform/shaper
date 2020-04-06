// Copyright (C) 2020  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_SketchCopy_H_
#define SketchPlugin_SketchCopy_H_

#include "SketchPlugin.h"
#include <ModelAPI_Feature.h>

/// \class SketchPlugin_SketchCopy
/// \ingroup Plugins
/// \brief Feature for copy of a sketch and all its sub-features to the same plane.
class SketchPlugin_SketchCopy : public ModelAPI_Feature
{
public:
  /// A constructor
  SketchPlugin_SketchCopy();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_FEATURE_ID("SketchCopy");
    return MY_FEATURE_ID;
  }

  /// Attribute name of the base sketch selected.
  inline static const std::string& BASE_ID()
  {
    static const std::string ID("base_sketch");
    return ID;
  }

  /// \return the kind of a feature.
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_SketchCopy::ID();
    return MY_KIND;
  }

  /// Creates a new sketch.
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Means that feature is removed on apply.
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;}

  /// No preview is generated until it is applied.
  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;}
};

#endif
