// Copyright (C) 2014-2026  CEA, EDF
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

#ifndef FeaturesPlugin_Thickness_H_
#define FeaturesPlugin_Thickness_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_Thickness
/// \ingroup Plugins
/// \brief Feature for thickness.
class FeaturesPlugin_Thickness: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  FeaturesPlugin_Thickness();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Thickness");
    return MY_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_ID()
  {
    static const std::string MY_CREATION_METHOD_ID("creation_method");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_THICK()
  {
    static const std::string MY_CREATION_METHOD_THICK("thickness");
    return MY_CREATION_METHOD_THICK;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_HOLLOWED()
  {
    static const std::string MY_CREATION_METHOD_HOLLOWED("hollowed_solid");
    return MY_CREATION_METHOD_HOLLOWED;
  }

  /// Attribute name of base shape.
  inline static const std::string& BASE_SHAPE_ID()
  {
    static const std::string MY_BASE_SHAPE_ID("base_shape");
    return MY_BASE_SHAPE_ID;
  }

  /// Attribute name of sub-faces to remove (for hollowed solid mode).
  inline static const std::string& FACES_ID()
  {
    static const std::string MY_FACES_ID("faces_to_remove");
    return MY_FACES_ID;
  }

  /// Attribute name of thickness value.
  inline static const std::string& THICKNESS_VALUE_ID()
  {
    static const std::string MY_THICKNESS_VALUE_ID("thickness_value");
    return MY_THICKNESS_VALUE_ID;
  }

  /// Attribute name of pipe/intersection joint bool flag.
  inline static const std::string& INSIDE_ID()
  {
    static const std::string MY_INSIDE_ID("is_inside");
    return MY_INSIDE_ID;
  }


  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Thickness::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Called on change of any argument-attribute of this object.
  /// \param[in] theID identifier of changed attribute.
  FEATURESPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);
};

#endif
