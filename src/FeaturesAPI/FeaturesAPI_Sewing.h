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

#ifndef FEATURESAPI_SEWING_H_
#define FEATURESAPI_SEWING_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Sewing.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Sewing
/// \ingroup CPPHighAPI
/// \brief Interface for Sewing feature.
class FeaturesAPI_Sewing: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Sewing(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Sewing(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const std::list<ModelHighAPI_Selection>& theMainObjects,
                              const ModelHighAPI_Double& theTolerance,
                              bool theIsAllowNonManifold = false,
                              bool theIsAlwaysCreateResult = true);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Sewing();

  INTERFACE_4(FeaturesPlugin_Sewing::ID(),
              mainObjects, FeaturesPlugin_Sewing::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              tolerance, FeaturesPlugin_Sewing::TOLERANCE_ID(),
              ModelAPI_AttributeDouble, /** Tolerance */,
              isAllowNonManifold, FeaturesPlugin_Sewing::ALLOW_NON_MANIFOLD_ID(),
              ModelAPI_AttributeBoolean, /** Allow Non-Manifold Shapes */,
              isAlwaysCreateResult, FeaturesPlugin_Sewing::ALWAYS_CREATE_RESULT_ID(),
              ModelAPI_AttributeBoolean, /** Always Create Result */)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Set the tolerance.
  FEATURESAPI_EXPORT
  void setTolerance(const ModelHighAPI_Double& theTolerance);

  /// Set flag whether to allow non-manifold results.
  FEATURESAPI_EXPORT
  void setIsAllowNonManifold(bool theFlag);

  /// Set flag whether to always create a result.
  FEATURESAPI_EXPORT
  void setIsAlwaysCreateResult(bool theFlag);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Sewing object.
typedef std::shared_ptr<FeaturesAPI_Sewing> SewingPtr;

/// \ingroup CPPHighAPI
/// \brief Create Sewing feature.
FEATURESAPI_EXPORT
SewingPtr addSewing(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theMainObjects,
                    const ModelHighAPI_Double& theTolerance,
                    const bool theIsAllowNonManifold,
                    const bool theIsAlwaysCreateResult);

#endif // FEATURESAPI_SEWING_H_
