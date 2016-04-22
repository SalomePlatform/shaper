// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildPlugin_Validators.h
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#ifndef BuildPlugin_Validators_H_
#define BuildPlugin_Validators_H_

#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_FeatureValidator.h>

/// \class BuildPlugin_ValidatorBaseForBuild
/// \ingroup Validators
/// \brief A validator for selection base shapes for build features. Allows to select shapes on sketch and
/// whole objects with allowed type.
class BuildPlugin_ValidatorBaseForBuild: public ModelAPI_AttributeValidator
{
public:
  //! Returns true if attribute is ok.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        std::string& theError) const;
};

/// \class BuildPlugin_ValidatorBaseForWire
/// \ingroup Validators
/// \brief A validator for selection base shapes for wire. Allows to select edges on sketch and
/// wires objects that are connected to already selected shapes.
class BuildPlugin_ValidatorBaseForWire: public ModelAPI_FeatureValidator
{
public:
  //! Returns true if attributes is ok.
  //! \param theFeature the checked feature.
  //! \param theArguments arguments of the feature.
  //! \param theError error message.
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;

  /// \return true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

/// \class BuildPlugin_ValidatorBaseForFace
/// \ingroup Validators
/// \brief A validator for selection base shapes for face. Allows to select sketch edges, edges and
/// wires objects that lie in the same plane and don't have intersections.
class BuildPlugin_ValidatorBaseForFace: public ModelAPI_FeatureValidator
{
public:
  //! Returns true if attributes is ok.
  //! \param theFeature the checked feature.
  //! \param theArguments arguments of the feature.
  //! \param theError error message.
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;

  /// \return true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
