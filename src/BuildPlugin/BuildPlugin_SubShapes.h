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

#ifndef BuildPlugin_SubShapes_H_
#define BuildPlugin_SubShapes_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

/// \class BuildPlugin_SubShapes
/// \ingroup Plugins
/// \brief Feature for adding or removing sub-shapes from shape.
class BuildPlugin_SubShapes: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  BuildPlugin_SubShapes();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("SubShapes");
    return MY_ID;
  }

  /// Attribute name of base shape.
  inline static const std::string& BASE_SHAPE_ID()
  {
    static const std::string MY_BASE_SHAPE_ID("base_shape");
    return MY_BASE_SHAPE_ID;
  }

  /// Attribute name of sub-shapes.
  inline static const std::string& SUBSHAPES_ID()
  {
    static const std::string MY_SUBSHAPES_ID("subshapes");
    return MY_SUBSHAPES_ID;
  }

  /// \return the kind of a feature.
  BUILDPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = BuildPlugin_SubShapes::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  BUILDPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object.
  /// \param[in] theID identifier of changed attribute.
  BUILDPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed.
  BUILDPLUGIN_EXPORT virtual void execute();
};

#endif
