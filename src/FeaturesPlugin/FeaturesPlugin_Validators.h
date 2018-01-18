// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef FeaturesPlugin_Validators_H_
#define FeaturesPlugin_Validators_H_

#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_FeatureValidator.h>

/// \class FeaturesPlugin_ValidatorPipePath
/// \ingroup Validators
/// \brief A validator for selection pipe path.
class FeaturesPlugin_ValidatorPipePath: public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

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
                       Events_InfoMessage& theError) const;
};

/// \class FeaturesPlugin_ValidatorBaseForGeneration
/// \ingroup Validators
/// \brief A validator for selection base for generation. Allows to select faces on sketch,
/// whole sketch(if it has at least one face), and following objects: vertex, edge, wire, face.
class FeaturesPlugin_ValidatorBaseForGeneration: public ModelAPI_AttributeValidator
{
public:
  //! \return true if attribute has selection type listed in the parameter arguments.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;

private:
  bool isValidAttribute(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class FeaturesPlugin_ValidatorBaseForGenerationSketchOrSketchObjects
/// \ingroup Validators
/// \brief Validator for the base objects for generation. Checks that sketch and it objects
///        are not selected at the same time.
class FeaturesPlugin_ValidatorBaseForGenerationSketchOrSketchObjects:
  public ModelAPI_FeatureValidator
{
 public:
  //! \return true if sketch and it objects not selected at the same time.
  //! \param theFeature the checked feature
  //! \param theArguments arguments of the feature (not used)
  //! \param theError error message
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/// \class FeaturesPlugin_ValidatorCompositeLauncher
/// \ingroup Validators
/// \brief A validator for selection at composite feature start
class FeaturesPlugin_ValidatorCompositeLauncher: public ModelAPI_AttributeValidator
{
public:
  //! \return true if attribute has selection type listed in the parameter arguments.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class FeaturesPlugin_ValidatorExtrusionDir
/// \ingroup Validators
/// \brief A validator for extrusion direction attribute. Allows it to be empty if base objects are
///        planar and do not contain vertices and edges.
class FeaturesPlugin_ValidatorExtrusionDir: public ModelAPI_FeatureValidator
{
public:
  //! \return true if attribute listed in the parameter arguments are planar.
  //! \param[in] theFeature the checked feature.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;

private:
  bool isShapesCanBeEmpty(const AttributePtr& theAttribute,
                          Events_InfoMessage& theError) const;
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
                       Events_InfoMessage& theError) const;
};

/// \class FeaturesPlugin_ValidatorFilletSelection
/// \ingroup Validators
/// \brief Validates selection for fillet operation.
class FeaturesPlugin_ValidatorFilletSelection: public ModelAPI_AttributeValidator
{
public:
  /// \return True if the attribute is valid. It checks whether the selection
  /// is acceptable for boolean operation.
  /// \param[in] theAttribute an attribute to check.
  /// \param[in] theArguments a filter parameters.
  /// \param[out] theError error message.
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
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
                       Events_InfoMessage& theError) const;
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
                       Events_InfoMessage& theError) const;
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
                       Events_InfoMessage& theError) const;
};

/// \class FeaturesPlugin_ValidatorUnionSelection
/// \ingroup Validators
/// \brief Validates selection for "Union" feature.
class FeaturesPlugin_ValidatorUnionSelection: public ModelAPI_AttributeValidator
{
public:
  /// \return True if the attribute is valid. It checks whether the selection
  /// is acceptable for operation.
  /// \param[in] theAttribute an attribute to check.
  /// \param[in] theArguments a filter parameters.
  /// \param[out] theError error message.
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/// \class FeaturesPlugin_ValidatorUnionArguments
/// \ingroup Validators
/// \brief Validator for the "Union" feature.
class FeaturesPlugin_ValidatorUnionArguments: public ModelAPI_FeatureValidator
{
 public:
  //! \return true if result is valid shape.
  //! \param theFeature the checked feature
  //! \param theArguments arguments of the feature (not used)
  //! \param theError error message
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/// \class FeaturesPlugin_ValidatorConcealedResult
/// \ingroup Validators
/// \brief Validator for the "Recover" feature.
class FeaturesPlugin_ValidatorConcealedResult: public ModelAPI_AttributeValidator
{
 public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

#endif
