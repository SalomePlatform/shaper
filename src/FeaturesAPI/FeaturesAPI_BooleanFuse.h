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

#ifndef FeaturesAPI_BooleanFuse_H_
#define FeaturesAPI_BooleanFuse_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_BooleanFuse.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Integer;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_BooleanFuse
/// \ingroup CPPHighAPI
/// \brief Interface for Boolean Fuse feature.
class FeaturesAPI_BooleanFuse: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_BooleanFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_BooleanFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                          const std::list<ModelHighAPI_Selection>& theMainObjects,
                          const bool theRemoveEdges = false);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_BooleanFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                          const std::list<ModelHighAPI_Selection>& theMainObjects,
                          const std::list<ModelHighAPI_Selection>& theToolObjects,
                          const bool theRemoveEdges = false);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_BooleanFuse();

  INTERFACE_4(FeaturesPlugin_BooleanFuse::ID(),
              creationMethod, FeaturesPlugin_BooleanFuse::CREATION_METHOD(),
              ModelAPI_AttributeString, /** Creation method */,
              mainObjects, FeaturesPlugin_BooleanFuse::OBJECT_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              toolObjects, FeaturesPlugin_BooleanFuse::TOOL_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Tool objects*/,
              removeEdges, FeaturesPlugin_BooleanFuse::REMOVE_INTERSECTION_EDGES_ID(),
              ModelAPI_AttributeBoolean, /** Remove edges */)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Set tool objects.
  FEATURESAPI_EXPORT
  void setToolObjects(const std::list<ModelHighAPI_Selection>& theToolObjects);

  /// Set remove edges.
  FEATURESAPI_EXPORT
  void setRemoveEdges(const bool theRemoveEdges);

  /// Set mode.
  FEATURESAPI_EXPORT
  void setAdvancedMode(const bool theMode);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Boolean object.
typedef std::shared_ptr<FeaturesAPI_BooleanFuse> BooleanFusePtr;

#define DUMMY_TOOLS std::pair<std::list<ModelHighAPI_Selection>, bool>\
                    (std::list<ModelHighAPI_Selection>(), false)

/// \ingroup CPPHighAPI
/// \brief Create Boolean Fuse feature.
FEATURESAPI_EXPORT BooleanFusePtr addFuse(
    const std::shared_ptr<ModelAPI_Document>& part,
    const std::list<ModelHighAPI_Selection>& objects,
    const std::pair<std::list<ModelHighAPI_Selection>, bool>& tools = DUMMY_TOOLS,
    const bool removeEdges = false,
    const bool keepSubResults = false);

#endif // FeaturesAPI_BooleanFuse_H_
