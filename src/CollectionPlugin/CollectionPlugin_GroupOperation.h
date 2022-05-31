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

#ifndef COLLECTIONPLUGIN_GROUPOPERATION_H_
#define COLLECTIONPLUGIN_GROUPOPERATION_H_

#include "CollectionPlugin.h"
#include "CollectionPlugin_Group.h"

/**\class CollectionPlugin_GroupOperation
 * \ingroup Plugins
 * \brief Base class for operations on groups.
 */
class CollectionPlugin_GroupOperation : public CollectionPlugin_Group
{
public:
  /// Result of groups is created on the fly and don't stored to the document
  COLLECTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return true;}

protected:
  /// Updates the type of selection list based on types of selected groups
  void updateGroupType(const std::string& theAttributeName);
};

#endif
