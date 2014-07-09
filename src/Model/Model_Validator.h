// File:        Model_Validator.hxx
// Created:     2 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Validator_HeaderFile
#define Model_Validator_HeaderFile

#include <Model.h>
#include <ModelAPI_Validator.h>
#include <map>

/**\class Model_ValidatorsFactory
 * \ingroup DataModel
 * \breif Manages the registered validators
 *
 * Allows to get a validator by the feature identifier and 
 * the attribute identifier (if attribute is validated).
 * All accessible validators mustbe registered by the ID string first.
 * The instance of this factory can be get in the PluginManager.
 * Keeps the validator objects alive and just returns one of it by request.
 * All the needed information is provided to the validator as an argument,
 * this allows to work with them independently from the feature specific object.
 */
class Model_ValidatorsFactory: public ModelAPI_ValidatorsFactory
{
  std::map<std::string, ModelAPI_Validator*> myIDs; ///< map from ID to registered validator
  std::map<std::string, ModelAPI_Validator*> myFeatures; ///< validators by feature ID
  std::map<std::string, std::map<std::string, std::pair<ModelAPI_Validator*, 
    std::list<std::string> > > > myAttrs; ///< validators and arguments by feature and attribute IDs
public:
  /// Registers the instance of the validator by the ID
  MODEL_EXPORT virtual void registerValidator(
    const std::string& theID, ModelAPI_Validator* theValidator);

  /// Assigns validator to the feature
  MODEL_EXPORT virtual void assignValidator(
    const std::string& theID, const std::string& theFeatureID);

  /// Assigns validator to the attribute of the feature
  MODEL_EXPORT virtual void assignValidator(const std::string& theID, 
    const std::string& theFeatureID, const std::string& theAttrID,
    const std::list<std::string>& theArguments);

  /// Provides a validator for the feature, returns NULL if no validator
  MODEL_EXPORT virtual const ModelAPI_Validator* validator(const std::string& theFeatureID) const;

  /// Returns the result of "validate" method for attribute of validator.
  /// If validator is not exists, returns true: everything is valid by default.
  //MODEL_EXPORT virtual bool validate(
  //  const boost::shared_ptr<ModelAPI_Feature>& theFeature, const std::string& theAttrID) const;

protected:
  /// Get instance from PluginManager
  Model_ValidatorsFactory();

  friend class Model_PluginManager;
};

#endif
