// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef FeaturesAPI_Partition_H_
#define FeaturesAPI_Partition_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Partition.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Partition
/// \ingroup CPPHighAPI
/// \brief Interface for Partition feature.
class FeaturesAPI_Partition: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Partition(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Partition(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Partition();

  INTERFACE_1(FeaturesPlugin_Partition::ID(),
              baseObjects, FeaturesPlugin_Partition::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Partition object.
typedef std::shared_ptr<FeaturesAPI_Partition> PartitionPtr;

/// \ingroup CPPHighAPI
/// \brief Create Partition feature.
FEATURESAPI_EXPORT
PartitionPtr addPartition(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects);

#endif // FeaturesAPI_Partition_H_
