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

#ifndef CollectionAPI_GroupSubstraction_H_
#define CollectionAPI_GroupSubstraction_H_

#include "CollectionAPI.h"

#include <CollectionPlugin_GroupSubstraction.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class CollectionAPI_GroupSubstraction
/// \ingroup CPPHighAPI
/// \brief Interface for Group Intersection feature.
class CollectionAPI_GroupSubstraction : public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  COLLECTIONAPI_EXPORT
  explicit CollectionAPI_GroupSubstraction(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  COLLECTIONAPI_EXPORT
  CollectionAPI_GroupSubstraction(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                  const std::list<ModelHighAPI_Selection>& theObjectsList,
                                  const std::list<ModelHighAPI_Selection>& theToolsList);

  /// Destructor.
  COLLECTIONAPI_EXPORT
  virtual ~CollectionAPI_GroupSubstraction();

  INTERFACE_2(CollectionPlugin_GroupSubstraction::ID(),
              objectsList, CollectionPlugin_GroupSubstraction::LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Objects list*/,
              toolsList, CollectionPlugin_GroupSubstraction::TOOLS_ID(),
              ModelAPI_AttributeSelectionList, /** Tools list*/)

  /// Set main objects.
  COLLECTIONAPI_EXPORT
  void setObjectsList(const std::list<ModelHighAPI_Selection>& theGroupList);

  /// Set tools.
  COLLECTIONAPI_EXPORT
  void setToolsList(const std::list<ModelHighAPI_Selection>& theGroupList);

  /// Dump wrapped feature
  COLLECTIONAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Group Addition object.
typedef std::shared_ptr<CollectionAPI_GroupSubstraction> GroupSubstractionPtr;

/// \ingroup CPPHighAPI
/// \brief Create Group Substraction feature.
COLLECTIONAPI_EXPORT
GroupSubstractionPtr addGroupSubstraction(const std::shared_ptr<ModelAPI_Document>& thePart,
                                          const std::list<ModelHighAPI_Selection>& theObjectsList,
                                          const std::list<ModelHighAPI_Selection>& theToolsList);

#endif // CollectionAPI_GroupSubstraction_H_
