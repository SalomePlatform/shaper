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

#ifndef FeaturesPlugin_Offset_H_
#define FeaturesPlugin_Offset_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_Offset
/// \ingroup Plugins
/// \brief Feature for offset.
class FeaturesPlugin_Offset: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  FeaturesPlugin_Offset();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Offset3d");
    return MY_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_ID()
  {
    static const std::string MY_CREATION_METHOD_ID("creation_method");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_EQUAL()
  {
    static const std::string MY_CREATION_METHOD_EQUAL("offset_equal");
    return MY_CREATION_METHOD_EQUAL;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_PARTIAL()
  {
    static const std::string MY_CREATION_METHOD_PARTIAL("offset_partial");
    return MY_CREATION_METHOD_PARTIAL;
  }

  /// Attribute name of base shape.
  inline static const std::string& BASE_SHAPE_ID()
  {
    static const std::string MY_BASE_SHAPE_ID("base_shape");
    return MY_BASE_SHAPE_ID;
  }

  /// Attribute name of offset value.
  inline static const std::string& OFFSET_VALUE_ID()
  {
    static const std::string MY_OFFSET_VALUE_ID("offset_value");
    return MY_OFFSET_VALUE_ID;
  }

  /// Attribute name of pipe/intersection joint bool flag.
  inline static const std::string& PIPE_JOINT_ID()
  {
    static const std::string MY_PIPE_JOINT_ID("pipe_joint");
    return MY_PIPE_JOINT_ID;
  }

  /// Attribute name of sub-faces to offset (for partial offset).
  inline static const std::string& FACES_ID()
  {
    static const std::string MY_FACES_ID("faces_to_offset");
    return MY_FACES_ID;
  }


  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Offset::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();
};

#endif
