// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef ModelAPI_Validator_H_
#define ModelAPI_Validator_H_

#include <ModelAPI.h>
#include <ModelAPI_Feature.h>
#include <memory>
#include <list>

class ModelAPI_Feature;
class Events_InfoMessage;

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
  virtual ~ModelAPI_Validator()
  {
  }
};

typedef std::shared_ptr<ModelAPI_Validator> ValidatorPtr;

/**\class ModelAPI_ValidatorsFactory
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
class MODELAPI_EXPORT ModelAPI_ValidatorsFactory
{
 public:
   virtual ~ModelAPI_ValidatorsFactory() {}

  /// Registers the instance of the validator by the ID
  virtual void registerValidator(const std::string& theID, ModelAPI_Validator* theValidator) = 0;

  /// Assigns validator to the feature
  virtual void assignValidator(const std::string& theID, const std::string& theFeatureID) = 0;

  /// Assigns validator to the feature with arguments of the validator
  virtual void assignValidator(const std::string& theID,
                                            const std::string& theFeatureID,
                                            const std::list<std::string>& theArguments) = 0;

  /// Assigns validator to the attribute of the feature
  virtual void assignValidator(const std::string& theID, const std::string& theFeatureID,
                               const std::string& theAttrID,
                               const std::list<std::string>& theArguments) = 0;

  /// Validators is a list of pairs <Validator, list of arguments>
  typedef std::list<std::pair<std::string, std::list<std::string> > > Validators;
  /// Provides a validator for the feature, returns NULL if no validator
  virtual void validators(const std::string& theFeatureID,
                          Validators& theResult) const = 0;
  /// Provides a validator for the attribute, returns NULL if no validator
  virtual void validators(const std::string& theFeatureID, const std::string& theAttrID,
                          Validators& theResult) const = 0;

  /// Returns registered validator by its Id
  virtual const ModelAPI_Validator* validator(const std::string& theID) const = 0;

  /// Returns true if feature and all its attributes are valid.
  virtual bool validate(const std::shared_ptr<ModelAPI_Feature>& theFeature) const = 0;

  /// Returns true if the attribute is valid.
  virtual bool validate(const std::shared_ptr<ModelAPI_Attribute>& theAttribute,
                        std::string& theValidator, Events_InfoMessage& theError) const = 0;

  /// register that this attribute in feature is not obligatory for the feature execution
  /// so, it is not needed for the standard validation mechanism
  virtual void registerNotObligatory(std::string theFeature, std::string theAttribute) = 0;

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute) = 0;

  /// register that this attribute conceals in the object browser
  /// all referenced features after execution
  virtual void registerConcealment(std::string theFeature, std::string theAttribute) = 0;

  /// Returns true that it was registered that attribute conceals the referenced result
  virtual bool isConcealed(std::string theFeature, std::string theAttribute) = 0;

  /// Register the attribute as a main argument of the feature
  virtual void registerMainArgument(std::string theFeature, std::string theAttribute) = 0;

  /// Returns true is the attribute is a main argument of the feature
  virtual bool isMainArgument(std::string theFeature, std::string theAttribute) = 0;

  /// Register the case-attribute: this attribute is checked only if its case is selected
  virtual void registerCase(std::string theFeature, std::string theAttribute,
    const std::list<std::pair<std::string, std::string> >& theCases) = 0;

 /// Returns true if the attribute must be checked (the case is selected)
  virtual bool isCase(FeaturePtr theFeature, std::string theAttribute) = 0;

  /// Register the selection attribute as geometrical selection
  virtual void registerGeometricalSelection(std::string theFeature, std::string theAttribute) = 0;

  /// Returns true if the attribute is a geometrical selection
  virtual bool isGeometricalSelection(std::string theFeature, std::string theAttribute) = 0;

 protected:
  /// Get instance from Session
  ModelAPI_ValidatorsFactory()
  {
  }
};

#endif
