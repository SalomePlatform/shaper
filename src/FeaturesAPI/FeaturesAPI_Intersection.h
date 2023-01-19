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

#ifndef FeaturesAPI_Intersection_H_
#define FeaturesAPI_Intersection_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Intersection.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Double.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Intersection
/// \ingroup CPPHighAPI
/// \brief Interface for Intersection feature.
class FeaturesAPI_Intersection: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Intersection(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Intersection(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theObjects,
                                    const ModelHighAPI_Double& aFuzzy = ModelHighAPI_Double(-1));

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Intersection();

  INTERFACE_3(FeaturesPlugin_Intersection::ID(),
              objects, FeaturesPlugin_Intersection::OBJECT_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Objects */,
              useFuzzy, FeaturesPlugin_Intersection::USE_FUZZY_ID(),
              ModelAPI_AttributeBoolean, /** Use Fuzzy parameter*/,
              fuzzyParam, FeaturesPlugin_Intersection::FUZZY_PARAM_ID(),
              ModelAPI_AttributeDouble, /** Fuzzy parameter */)

  /// Modify objects attribute of the feature.
  FEATURESAPI_EXPORT
  void setObjects(const std::list<ModelHighAPI_Selection>& theObjects);

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

/// Pointer on Intersection object.
typedef std::shared_ptr<FeaturesAPI_Intersection> IntersectionPtr;

/// \ingroup CPPHighAPI
/// \brief Create Intersection feature.
FEATURESAPI_EXPORT
IntersectionPtr addIntersection(const std::shared_ptr<ModelAPI_Document>& part,
                                const std::list<ModelHighAPI_Selection>& objects,
                                const ModelHighAPI_Double& fuzzyParam = ModelHighAPI_Double(-1),
                                const bool keepSubResults = false);

#endif // FeaturesAPI_Intersection_H_
