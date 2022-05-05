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

#ifndef FeaturesAPI_Recover_H_
#define FeaturesAPI_Recover_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Recover.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Reference;

/// \class FeaturesAPI_Recover
/// \ingroup CPPHighAPI
/// \brief Interface for Recover feature.
class FeaturesAPI_Recover: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Recover(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_Recover(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                      const ModelHighAPI_Reference& theBaseFeature,
                      const std::list<ModelHighAPI_Selection>& theRecoveredList,
                      const bool theRecoverCompound = false);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Recover();

  INTERFACE_2(FeaturesPlugin_Recover::ID(),
              baseFeature, FeaturesPlugin_Recover::BASE_FEATURE(),
              ModelAPI_AttributeReference, /** Concealed feature */,
              recoveredList, FeaturesPlugin_Recover::RECOVERED_ENTITIES(),
              ModelAPI_AttributeRefList, /** Recover list*/)

  /// Set base feature.
  FEATURESAPI_EXPORT
  void setBaseFeature(const ModelHighAPI_Reference& theBaseFeature);

  /// Set recovered list of the base feature
  FEATURESAPI_EXPORT
  void setRecoveredList(const std::list<ModelHighAPI_Selection>& theRecoverList);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Recover object.
typedef std::shared_ptr<FeaturesAPI_Recover> RecoverPtr;

/// \ingroup CPPHighAPI
/// \brief Create Recover feature.
FEATURESAPI_EXPORT
RecoverPtr addRecover(const std::shared_ptr<ModelAPI_Document>& thePart,
                      const ModelHighAPI_Reference& theBaseFeature,
                      const std::list<ModelHighAPI_Selection>& theRecoveredList,
                      const bool theRecoverCompound = false);

#endif // FeaturesAPI_Recover_H_
