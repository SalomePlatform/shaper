// File:        Model_Validator.hxx
// Created:     2 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Validator_H_
#define Model_Validator_H_

#include <Model.h>
#include <ModelAPI_Validator.h>
#include <map>
#include <set>

/**\class Model_ValidatorsFactory
 * \ingroup DataModel
 * \breif Manages the registered validators
 *
 * Allows to get a validator by the feature identifier and 
 * the attribute identifier (if attribute is validated).
 * All accessible validators mustbe registered by the ID string first.
 * The instance of this factory can be get in the Session.
 * Keeps the validator objects alive and just returns one of it by request.
 * All the needed information is provided to the validator as an argument,
 * this allows to work with them independently from the feature specific object.
 */
class Model_ValidatorsFactory : public ModelAPI_ValidatorsFactory
{
 private:
  std::map<std::string, ModelAPI_Validator*> myIDs;  ///< map from ID to registered validator
  /// validators IDs to list of arguments
  typedef std::map<std::string, std::list<std::string> > AttrValidators;
  /// validators IDs by feature ID
  std::map<std::string, AttrValidators> myFeatures;
  /// validators IDs and arguments by feature and attribute IDs
  std::map<std::string, std::map<std::string, AttrValidators> > myAttrs;
  /// Stores the registered attributes that leads to the concealment of referenced objects in 
  /// data tree. Map from feature kind to set of attribute IDs.
  std::map<std::string, std::set<std::string> > myConcealed;

 public:
  /// Registers the instance of the validator by the ID
  MODEL_EXPORT virtual void registerValidator(const std::string& theID,
                                              ModelAPI_Validator* theValidator);

  /// Assigns validator to the feature
  MODEL_EXPORT virtual void assignValidator(const std::string& theID,
                                            const std::string& theFeatureID);

  /// Assigns validator to the feature with arguments of the validator
  MODEL_EXPORT virtual void assignValidator(const std::string& theID,
                                            const std::string& theFeatureID,
                                            const std::list<std::string>& theArguments);

  /// Assigns validator to the attribute of the feature
  MODEL_EXPORT virtual void assignValidator(const std::string& theID,
                                            const std::string& theFeatureID,
                                            const std::string& theAttrID,
                                            const std::list<std::string>& theArguments);

  /// Provides a validator for the feature, returns NULL if no validator
  MODEL_EXPORT virtual void validators(const std::string& theFeatureID,
                                       std::list<ModelAPI_Validator*>& theResult,
                                       std::list<std::list<std::string> >& theArguments) const;
  /// Provides a validator for the attribute, returns NULL if no validator
  MODEL_EXPORT virtual void validators(const std::string& theFeatureID,
                                       const std::string& theAttrID,
                                       std::list<ModelAPI_Validator*>& theValidators,
                                       std::list<std::list<std::string> >& theArguments) const;

  /// Returns registered validator by its Id
  MODEL_EXPORT virtual const ModelAPI_Validator* validator(const std::string& theID) const;

  /// Returns true if feature and all its attributes are valid.
  MODEL_EXPORT virtual bool validate(const std::shared_ptr<ModelAPI_Feature>& theFeature) const;

  /// register that this attribute in feature is not obligatory for the feature execution
  /// so, it is not needed for the standard validation mechanism
  virtual void registerNotObligatory(std::string theFeature, std::string theAttribute);

  /// register that this attribute conceals in the object browser
  /// all referenced features after execution
  virtual void registerConcealment(std::string theFeature, std::string theAttribute);

  /// Returns true that it was registered that attribute conceals the referenced result
  virtual bool isConcealed(std::string theFeature, std::string theAttribute);

protected:
  void addDefaultValidators(std::list<ModelAPI_Validator*>& theValidators) const;
  /// Get instance from Session
  Model_ValidatorsFactory();

  friend class Model_Session;
};

#endif
