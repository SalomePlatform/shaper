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

#ifndef COLLECTIONPLUGIN_GROUPSHAPE_H_
#define COLLECTIONPLUGIN_GROUPSHAPE_H_

#include "CollectionPlugin.h"
#include "CollectionPlugin_GroupOperation.h"
#include "CollectionPlugin_GroupMerge.h"

/**\class CollectionPlugin_GroupShape
 * \ingroup Plugins
 * \brief Merge several groups of different shape type into a single shape.
 */
class CollectionPlugin_GroupShape: public CollectionPlugin_GroupMerge
{
public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_GROUP_ID("GroupShape");
    return MY_GROUP_ID;
  }

  /// Returns the kind of a feature
  COLLECTIONPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = CollectionPlugin_GroupShape::ID();
    return MY_KIND;
  }

  /// Creates a new group result if needed
  COLLECTIONPLUGIN_EXPORT void execute();

  /// Use plugin manager for features creation
  CollectionPlugin_GroupShape() = default;

};

#endif
