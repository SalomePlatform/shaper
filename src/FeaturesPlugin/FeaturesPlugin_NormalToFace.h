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

#ifndef FeaturesPlugin_NormalToFace_H_
#define FeaturesPlugin_NormalToFace_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>

#include <GeomAPI_IPresentable.h>
#include <GeomAPI_IScreenParams.h>

/// \class FeaturesPlugin_NormalToFace
/// \ingroup Plugins
/// \brief Persistence feature to construct normal to face
class FeaturesPlugin_NormalToFace : public ModelAPI_Feature
{
public:
  /// Normal to face kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Normal");
    return MY_ID;
  }

  /// Attribute name for face selected.
  inline static const std::string& FACE_SELECTED_ID()
  {
    static const std::string MY_FACE_ID("face");
    return MY_FACE_ID;
  }

  /// Attribute name for vertex selected.
  inline static const std::string& VERTEX_SELECTED_ID()
  {
    static const std::string MY_VERTEX_SELECTED_ID("vertex");
    return MY_VERTEX_SELECTED_ID;
  }

  /// Attribute name of vertex option.
  inline static const std::string& VERTEX_OPTION_ID()
  {
    static const std::string MY_VERTEX_OPTION_ID("vertex_option");
    return MY_VERTEX_OPTION_ID;
  }

  /// \return the kind of a feature.
  virtual const std::string& getKind()
  {
    return ID();
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  FEATURESPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  FeaturesPlugin_NormalToFace();

};

#endif
