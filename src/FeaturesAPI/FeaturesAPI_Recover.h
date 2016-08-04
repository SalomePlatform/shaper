// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Recover.h
// Created:     04 August 2016
// Author:      Mikhail Ponikarov

#ifndef FeaturesAPI_Recover_H_
#define FeaturesAPI_Recover_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Recover.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

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
                      const bool thePersistent = false);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Recover();

  INTERFACE_3(FeaturesPlugin_Recover::ID(),
              baseFeature, FeaturesPlugin_Recover::BASE_FEATURE(), ModelAPI_AttributeReference, /** Concealed feature */,
              recoveredList, FeaturesPlugin_Recover::RECOVERED_ENTITIES(), ModelAPI_AttributeRefList, /** Recover list*/,
              isPersistent, FeaturesPlugin_Recover::PERSISTENT(), ModelAPI_AttributeBoolean, /** Is persistent flag */)

  /// Set base feature.
  FEATURESAPI_EXPORT
  void setBaseFeature(const ModelHighAPI_Reference& theBaseFeature);

  /// Set recovered list of the base feature
  FEATURESAPI_EXPORT
  void setRecoveredList(const std::list<ModelHighAPI_Selection>& theRecoverList);

  /// Set auxiliary
  FEATURESAPI_EXPORT
  void setIsPersistent(bool thePersistent);

};

/// Pointer on Recover object.
typedef std::shared_ptr<FeaturesAPI_Recover> RecoverPtr;

/// \ingroup CPPHighAPI
/// \brief Create Recover feature.
FEATURESAPI_EXPORT
RecoverPtr addRecover(const std::shared_ptr<ModelAPI_Document>& thePart,
                      const ModelHighAPI_Reference& theBaseFeature,
                      const std::list<ModelHighAPI_Selection>& theRecoveredList,
                      const bool thePersistent = false);

#endif // FeaturesAPI_Recover_H_
