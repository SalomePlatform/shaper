// File:        ModelAPI_Validator.hxx
// Created:     2 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Validator_H_
#define ModelAPI_Validator_H_

#include <ModelAPI.h>
#include <boost/shared_ptr.hpp>
#include <list>

class ModelAPI_Feature;

/**\class ModelAPI_Validator
 * \ingroup DataModel
 * \brief Allows to validate the attribute value of a feature or the whole feature.
 *
 * This object is assigned by the name
 * in the XML file to the specific attribute or to the whole feature.
 * If validator returns "false", it is signalized in user interface
 * and feature is not executed.
 * Validators must be registered in the validators factory to be
 * correctly identified by the XML string-ID.
 */
class MODELAPI_EXPORT ModelAPI_Validator
{
public:
  // Make virtual destructor in order to make the class polymorphic
  virtual ~ModelAPI_Validator() {};

  /// Returns true if feature and/or attributes are valid
  /// \param theFeature the validated feature
  /// \param theAttr the validated attribute ID, empty string of feature is validated
  /// \param theArguments list of string, feature attribute names: dependent attributes
  //virtual bool validate(const boost::shared_ptr<ModelAPI_Feature>& theFeature,
  //  const std::string theAttr, std::list<std::string> theArguments) const = 0;
};

typedef boost::shared_ptr<ModelAPI_Validator> ValidatorPtr;

/**\class ModelAPI_ValidatorsFactory
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
class MODELAPI_EXPORT ModelAPI_ValidatorsFactory
{
public:
  /// Registers the instance of the validator by the ID
  virtual void registerValidator(const std::string& theID, ModelAPI_Validator* theValidator) = 0;

  /// Assigns validator to the feature
  virtual void assignValidator(const std::string& theID, const std::string& theFeatureID) = 0;

  /// Assigns validator to the attribute of the feature
  virtual void assignValidator(const std::string& theID, 
    const std::string& theFeatureID, const std::string& theAttrID,
    const std::list<std::string>& theArguments) = 0;

  /// Provides a validator for the feature, returns NULL if no validator
  virtual void validators(const std::string& theFeatureID, 
    std::list<ModelAPI_Validator*>& theResult) const = 0;
  /// Provides a validator for the attribute, returns NULL if no validator
  virtual void validators(
    const std::string& theFeatureID, const std::string& theAttrID,
    std::list<ModelAPI_Validator*>& theValidators, 
    std::list<std::list<std::string> >& theArguments) const = 0;

  /// Returns registered validator by its Id
  virtual const ModelAPI_Validator* validator(const std::string& theID) const = 0;

  /// Returns the result of "validate" method for attribute of validator.
  /// If validator is not exists, returns true: everything is valid by default.
  //virtual bool validate(
  //  const boost::shared_ptr<ModelAPI_Feature>& theFeature, const std::string& theAttrID) const = 0;

protected:
  /// Get instance from PluginManager
  ModelAPI_ValidatorsFactory() {}
};

#endif
