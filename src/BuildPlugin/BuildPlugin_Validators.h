// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#ifndef BuildPlugin_Validators_H_
#define BuildPlugin_Validators_H_

#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_FeatureValidator.h>

/// \class BuildPlugin_ValidatorBaseForBuild
/// \ingroup Validators
/// \brief A validator for selection base shapes for build features.
/// Allows to select shapes on sketch and
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
                        Events_InfoMessage& theError) const;
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
                       Events_InfoMessage& theError) const;
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
                       Events_InfoMessage& theError) const;
};

/// \class BuildPlugin_ValidatorBaseForSolids
/// \ingroup Validators
/// \brief A validator for selection base shapes for solid. Allows to select faces closed enough
/// to create a solid.
class BuildPlugin_ValidatorBaseForSolids: public ModelAPI_FeatureValidator
{
public:
  //! Returns true if attributes is ok.
  //! \param theFeature the checked feature.
  //! \param theArguments arguments of the feature.
  //! \param theError error message.
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/// \class BuildPlugin_ValidatorSubShapesSelection
/// \ingroup Validators
/// \brief A validator for selection sub-shapes for SubShape feature.
class BuildPlugin_ValidatorSubShapesSelection: public ModelAPI_AttributeValidator
{
public:
  //! Returns true if attribute is ok.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class BuildPlugin_ValidatorFillingSelection
/// \ingroup Validators
/// \brief A validator for selection of Filling feature.
class BuildPlugin_ValidatorFillingSelection: public ModelAPI_AttributeValidator
{
public:
  //! Returns true if attribute is ok.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class BuildPlugin_ValidatorBaseForVertex
/// \ingroup Validators
/// \brief A validator for selection of Vertex feature.
class BuildPlugin_ValidatorBaseForVertex: public ModelAPI_AttributeValidator
{
public:
  //! Returns true if attribute is ok.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

 /// \class BuildPlugin_ValidatorExpression
 /// \ingroup Validators
 /// \brief Validator for the expression of parameter.
class BuildPlugin_ValidatorExpressionInterpolation: public ModelAPI_AttributeValidator
{
public:
   //! Returns true if attribute has a valid parameter expression.
   //! \param theAttribute the checked attribute
   //! \param theArguments arguments of the attribute
   //! \param theError the error string message if validation fails
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

#endif
