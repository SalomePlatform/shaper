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

#ifndef CollectionAPI_GroupAddition_H_
#define CollectionAPI_GroupAddition_H_

#include "CollectionAPI.h"

#include <CollectionPlugin_GroupAddition.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class CollectionAPI_GroupAddition
/// \ingroup CPPHighAPI
/// \brief Interface for Group Addition feature.
class CollectionAPI_GroupAddition : public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  COLLECTIONAPI_EXPORT
  explicit CollectionAPI_GroupAddition(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  COLLECTIONAPI_EXPORT
  CollectionAPI_GroupAddition(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const std::list<ModelHighAPI_Selection>& theGroupList);

  /// Destructor.
  COLLECTIONAPI_EXPORT
  virtual ~CollectionAPI_GroupAddition();

  INTERFACE_1(CollectionPlugin_GroupAddition::ID(),
              groupList, CollectionPlugin_GroupAddition::LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Group list*/)

  /// Set main objects.
  COLLECTIONAPI_EXPORT
  void setGroupList(const std::list<ModelHighAPI_Selection>& theGroupList);

  /// Dump wrapped feature
  COLLECTIONAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Group Addition object.
typedef std::shared_ptr<CollectionAPI_GroupAddition> GroupAdditionPtr;

/// \ingroup CPPHighAPI
/// \brief Create Group Addition feature.
COLLECTIONAPI_EXPORT
GroupAdditionPtr addGroupAddition(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theGroupsList);

#endif // CollectionAPI_GroupAddition_H_
