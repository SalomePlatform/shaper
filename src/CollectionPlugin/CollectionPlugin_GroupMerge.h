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

#ifndef COLLECTIONPLUGIN_GROUPMERGE_H_
#define COLLECTIONPLUGIN_GROUPMERGE_H_

#include "CollectionPlugin.h"
#include "CollectionPlugin_GroupOperation.h"
#include "ModelAPI_ResultGroup.h"

/**\class CollectionPlugin_GroupMerge
 * \ingroup Plugins
 * \brief Merge several groups of same or different shape type,
 * \the validator being used for the shape type.
 */
class CollectionPlugin_GroupMerge: public CollectionPlugin_GroupOperation
{
public:
  /// attribute name of selected entities list
  inline static const std::string& LIST_ID()
  {
    static const std::string MY_GROUP_LIST_ID("group_list");
    return MY_GROUP_LIST_ID;
  }


  /// Creates a new group result if needed
  COLLECTIONPLUGIN_EXPORT void execute(ResultGroupPtr & aGroup);

  /// Request for initialization of data model of the feature: adding all attributes
  COLLECTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  CollectionPlugin_GroupMerge() = default;

};

#endif
