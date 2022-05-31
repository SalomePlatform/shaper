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

#ifndef FeaturesPlugin_FusionFaces_H_
#define FeaturesPlugin_FusionFaces_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_FusionFaces
/// \ingroup Plugins
/// \brief Feature for fusion of connected faces.
class FeaturesPlugin_FusionFaces : public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  FeaturesPlugin_FusionFaces();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("FusionFaces");
    return MY_ID;
  }

  /// Attribute name of base shape.
  inline static const std::string& BASE_SHAPE_ID()
  {
    static const std::string MY_BASE_SHAPE_ID("base_shape");
    return MY_BASE_SHAPE_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_FusionFaces::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Executes the faces fusion and stores the modififed shape.
  FEATURESPLUGIN_EXPORT virtual void execute();
};

#endif
