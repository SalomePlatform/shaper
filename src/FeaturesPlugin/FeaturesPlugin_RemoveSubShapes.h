// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_RemoveSubShapes_H_
#define FeaturesPlugin_RemoveSubShapes_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_RemoveSubShapes
/// \ingroup Plugins
/// \brief Feature for removing sub-shapes from collections(wire, shell, compsolid, compound).
class FeaturesPlugin_RemoveSubShapes: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  FeaturesPlugin_RemoveSubShapes();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Remove_SubShapes");
    return MY_ID;
  }

  /// Attribute name of base shape.
  inline static const std::string& BASE_SHAPE_ID()
  {
    static const std::string MY_BASE_SHAPE_ID("base_shape");
    return MY_BASE_SHAPE_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("creation_method");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_KEEP_SUBSHAPES()
  {
    static const std::string MY_CREATION_METHOD_ID("by_keep_subshapes");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_REMOVE_SUBSHAPES()
  {
    static const std::string MY_CREATION_METHOD_ID("by_remove_subshapes");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name of sub-shapes to keep.
  inline static const std::string& SUBSHAPES_TO_KEEP_ID()
  {
    static const std::string MY_SUBSHAPES_ID("subshapes_to_keep");
    return MY_SUBSHAPES_ID;
  }

  /// Attribute name of sub-shapes to remove.
  inline static const std::string& SUBSHAPES_TO_REMOVE_ID()
  {
    static const std::string MY_SUBSHAPES_ID("subshapes_to_remove");
    return MY_SUBSHAPES_ID;
  }


  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_RemoveSubShapes::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object.
  /// \param[in] theID identifier of changed attribute.
  FEATURESPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

private:
  bool myChangedInCode;
};

#endif
