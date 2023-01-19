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

#ifndef FeaturesAPI_Union_H_
#define FeaturesAPI_Union_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Union.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Double.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Union
/// \ingroup CPPHighAPI
/// \brief Interface for Union feature.
class FeaturesAPI_Union: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Union(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Union(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                             const ModelHighAPI_Double& theFuzzy = ModelHighAPI_Double(-1));

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Union();

  INTERFACE_3(FeaturesPlugin_Union::ID(),
              baseObjects, FeaturesPlugin_Union::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */,
              useFuzzy, FeaturesPlugin_Union::USE_FUZZY_ID(),
              ModelAPI_AttributeBoolean, /** Use Fuzzy parameter*/,
              fuzzyParam, FeaturesPlugin_Union::FUZZY_PARAM_ID(),
              ModelAPI_AttributeDouble, /** Fuzzy parameter */)

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

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

/// Pointer on Union object.
typedef std::shared_ptr<FeaturesAPI_Union> UnionPtr;

/// \ingroup CPPHighAPI
/// \brief Create Union feature.
FEATURESAPI_EXPORT
UnionPtr addUnion(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                  const ModelHighAPI_Double& fuzzyParam = ModelHighAPI_Double(-1),
                  const bool keepSubResults = false);

#endif // FeaturesAPI_Union_H_
