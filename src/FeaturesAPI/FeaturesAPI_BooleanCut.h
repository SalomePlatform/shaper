// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_BooleanCut_H_
#define FeaturesAPI_BooleanCut_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_BooleanCut.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Double.h>

class ModelHighAPI_Integer;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_BooleanCut
/// \ingroup CPPHighAPI
/// \brief Interface for Boolean Cut feature.
class FeaturesAPI_BooleanCut: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_BooleanCut(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_BooleanCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::list<ModelHighAPI_Selection>& theMainObjects,
                         const std::list<ModelHighAPI_Selection>& theToolObjects,
                         const ModelHighAPI_Double& theFuzzy = ModelHighAPI_Double(-1));

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_BooleanCut();

  INTERFACE_4(FeaturesPlugin_BooleanCut::ID(),
              mainObjects, FeaturesPlugin_BooleanCut::OBJECT_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              toolObjects, FeaturesPlugin_BooleanCut::TOOL_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Tool objects*/,
              useFuzzy, FeaturesPlugin_BooleanCut::USE_FUZZY_ID(),
              ModelAPI_AttributeBoolean, /** Use Fuzzy parameter*/,
              fuzzyParam, FeaturesPlugin_BooleanCut::FUZZY_PARAM_ID(),
              ModelAPI_AttributeDouble, /** Fuzzy parameter */)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Set tool objects.
  FEATURESAPI_EXPORT
  void setToolObjects(const std::list<ModelHighAPI_Selection>& theToolObjects);

  /// Set use fuzzy parameter.
  FEATURESAPI_EXPORT
  void setUseFuzzy(bool theUseFuzzy);

  /// Set fuzzy parameter.
  FEATURESAPI_EXPORT
  void setFuzzyValue(const ModelHighAPI_Double& theFuzzy);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Boolean object.
typedef std::shared_ptr<FeaturesAPI_BooleanCut> BooleanCutPtr;

/// \ingroup CPPHighAPI
/// \brief Create Boolean Cut feature.
FEATURESAPI_EXPORT
BooleanCutPtr addCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                     const std::list<ModelHighAPI_Selection>& theMainObjects,
                     const std::list<ModelHighAPI_Selection>& theToolObjects,
                     const ModelHighAPI_Double& fuzzyParam = ModelHighAPI_Double(-1),
                     const bool keepSubResults = false);

#endif // FeaturesAPI_BooleanCut_H_
