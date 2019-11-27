// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_BooleanSmash_H_
#define FeaturesAPI_BooleanSmash_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_BooleanSmash.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Integer;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_BooleanSmash
/// \ingroup CPPHighAPI
/// \brief Interface for Boolean Smash feature.
class FeaturesAPI_BooleanSmash: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_BooleanSmash(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_BooleanSmash(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                           const std::list<ModelHighAPI_Selection>& theMainObjects,
                           const std::list<ModelHighAPI_Selection>& theToolObjects,
                           const int theVersion = 0);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_BooleanSmash();

  INTERFACE_2(FeaturesPlugin_BooleanSmash::ID(),
              mainObjects, FeaturesPlugin_BooleanSmash::OBJECT_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              toolObjects, FeaturesPlugin_BooleanSmash::TOOL_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Tool objects*/)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Set tool objects.
  FEATURESAPI_EXPORT
  void setToolObjects(const std::list<ModelHighAPI_Selection>& theToolObjects);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Boolean object.
typedef std::shared_ptr<FeaturesAPI_BooleanSmash> BooleanSmashPtr;

/// \ingroup CPPHighAPI
/// \brief Create Boolean Smash feature.
FEATURESAPI_EXPORT
BooleanSmashPtr addSmash(const std::shared_ptr<ModelAPI_Document>& thePart,
                         const std::list<ModelHighAPI_Selection>& theMainObjects,
                         const std::list<ModelHighAPI_Selection>& theToolObjects,
                         const bool keepSubResults = false);

#endif // FeaturesAPI_BooleanSmash_H_
