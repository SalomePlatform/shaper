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

#ifndef FeaturesAPI_Partition_H_
#define FeaturesAPI_Partition_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Partition.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Double.h>

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
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                 const ModelHighAPI_Double& theFuzzy = ModelHighAPI_Double(-1));

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Partition();

  INTERFACE_3(FeaturesPlugin_Partition::ID(),
              baseObjects, FeaturesPlugin_Partition::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */,
              useFuzzy, FeaturesPlugin_Partition::USE_FUZZY_ID(),
              ModelAPI_AttributeBoolean, /** Use Fuzzy parameter*/,
              fuzzyParam, FeaturesPlugin_Partition::FUZZY_PARAM_ID(),
              ModelAPI_AttributeDouble, /** Fuzzy parameter */)

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Set use fuzzy parameter.
  FEATURESAPI_EXPORT
  void setUseFuzzy(bool theUseFuzzy);

  /// Modify fuzzy parameter attribute of the feature.
  FEATURESAPI_EXPORT
  void setFuzzy(const ModelHighAPI_Double& theFuzzy);

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
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Double& fuzzyParam = ModelHighAPI_Double(-1),
                          const bool keepSubResults = false);

#endif // FeaturesAPI_Partition_H_
