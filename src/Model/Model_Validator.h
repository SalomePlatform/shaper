// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
 * \brief Manages the registered validators
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
  /// Stores the registered attributes must be checked only if the particular case is activated
  /// Map from feature kind to map of attribute IDs to pair 
  // (switchId (ID of the attribute) and case Ids (possible values of the switch attribute))
  std::map<std::string, std::map<std::string,
    std::pair<std::string, std::set<std::string> > > > myCases;

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
                                       Validators& theResult) const;
  /// Provides a validator for the attribute, returns NULL if no validator
  MODEL_EXPORT virtual void validators(const std::string& theFeatureID, const std::string& theAttrID,
                                       Validators& theResult) const;

  /// Returns registered validator by its Id
  MODEL_EXPORT virtual const ModelAPI_Validator* validator(const std::string& theID) const;

  /// Returns true if feature and all its attributes are valid.
  MODEL_EXPORT virtual bool validate(const std::shared_ptr<ModelAPI_Feature>& theFeature) const;

  /// Returns true if the attribute is valid.
  MODEL_EXPORT virtual bool validate(const std::shared_ptr<ModelAPI_Attribute>& theAttribute,
                                     std::string& theValidator, std::string& theError) const;

  /// register that this attribute in feature is not obligatory for the feature execution
  /// so, it is not needed for the standard validation mechanism
  virtual void registerNotObligatory(std::string theFeature, std::string theAttribute);

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);

  /// register that this attribute conceals in the object browser
  /// all referenced features after execution
  virtual void registerConcealment(std::string theFeature, std::string theAttribute);

  /// Returns true that it was registered that attribute conceals the referenced result
  virtual bool isConcealed(std::string theFeature, std::string theAttribute);

  /// register the case-attribute (\a myCases set definition)
  virtual void registerCase(std::string theFeature, std::string theAttribute,
    std::string theSwitchId, std::string theCaseId);

  /// Returns true if the attribute must be checked (the case is selected)
  virtual bool isCase(FeaturePtr theFeature, std::string theAttribute);


protected:
  /// Adds the defualt validators that are usefull for all features.
  void addDefaultValidators(Validators& theValidators) const;
  /// Adds the defualt validators that are usefull for all attributes.
  void addDefaultAttributeValidators(Validators& theValidators) const;
  /// Get instance from Session
  Model_ValidatorsFactory();

  friend class Model_Session;
};

#endif
