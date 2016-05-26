// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Validators.h
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Validators_H_
#define FeaturesPlugin_Validators_H_

#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_FeatureValidator.h>

/// \class FeaturesPlugin_ValidatorPipeLocations
/// \ingroup Validators
/// \brief Validator for the pipe locations.
class FeaturesPlugin_ValidatorPipeLocations: public ModelAPI_FeatureValidator
{
 public:
  //! \return true if number of selected locations the same as number of selected bases, or empty.
  //! \param theFeature the checked feature
  //! \param theArguments arguments of the feature (not used)
  //! \param theError error message
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

/// \class FeaturesPlugin_ValidatorBaseForGeneration
/// \ingroup Validators
/// \brief A validator for selection base for generation. Allows to select faces on sketch,
/// whole sketch(if it has at least one face), and following objects: vertex, edge, wire, face.
class FeaturesPlugin_ValidatorBaseForGeneration: public ModelAPI_AttributeValidator
{
public:
  //! Returns true if attribute has selection type listed in the parameter arguments.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        std::string& theError) const;

private:
  bool isValidAttribute(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        std::string& theError) const;
};

/// \class FeaturesPlugin_ValidatorCompositeLauncher
/// \ingroup Validators
/// \brief A validator for selection at composite feature start
class FeaturesPlugin_ValidatorCompositeLauncher: public ModelAPI_AttributeValidator
{
public:
  //! Returns true if attribute has selection type listed in the parameter arguments.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        std::string& theError) const;
};

/// \class FeaturesPlugin_ValidatorCanBeEmpty
/// \ingroup Validators
/// \brief A validator for extrusion direction attribute. Allows it to be empty if base objects are
///        planar and do not contain vertices and edges.
class FeaturesPlugin_ValidatorCanBeEmpty: public ModelAPI_FeatureValidator
{
public:
  //! Returns true if attribute listed in the parameter arguments are planar.
  //! \param[in] theFeature the checked feature.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);

private:
  bool isShapesCanBeEmpty(const AttributePtr& theAttribute,
                          std::string& theError) const;
};

/// \class FeaturesPlugin_ValidatorBooleanSelection
/// \ingroup Validators
/// \brief Validates selection for boolean operation.
class FeaturesPlugin_ValidatorBooleanSelection: public ModelAPI_AttributeValidator
{
public:
  /// \return True if the attribute is valid. It checks whether the selection
  /// is acceptable for boolean operation.
  /// \param[in] theAttribute an attribute to check.
  /// \param[in] theArguments a filter parameters.
  /// \param[out] theError error message.
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

/// \class FeaturesPlugin_ValidatorPartitionSelection
/// \ingroup Validators
/// \brief Validates selection for partition.
class FeaturesPlugin_ValidatorPartitionSelection: public ModelAPI_AttributeValidator
{
public:
  /// \return True if the attribute is valid. It checks whether the selection
  /// is acceptable for operation.
  /// \param[in] theAttribute an attribute to check.
  /// \param[in] theArguments a filter parameters.
  /// \param[out] theError error message.
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

/// \class FeaturesPlugin_ValidatorRemoveSubShapesSelection
/// \ingroup Validators
/// \brief Validates selection for "Remove Sub-Shapes" feature.
class FeaturesPlugin_ValidatorRemoveSubShapesSelection: public ModelAPI_AttributeValidator
{
public:
  /// \return True if the attribute is valid. It checks whether the selection
  /// is acceptable for operation.
  /// \param[in] theAttribute an attribute to check.
  /// \param[in] theArguments a filter parameters.
  /// \param[out] theError error message.
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

/// \class FeaturesPlugin_ValidatorRemoveSubShapesResult
/// \ingroup Validators
/// \brief Validator for the Remove Sub-Shapes feature.
class FeaturesPlugin_ValidatorRemoveSubShapesResult: public ModelAPI_FeatureValidator
{
 public:
  //! \return true if result is valid shape.
  //! \param theFeature the checked feature
  //! \param theArguments arguments of the feature (not used)
  //! \param theError error message
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
