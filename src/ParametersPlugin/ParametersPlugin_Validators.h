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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

/*
 * Parameters_VariableValidator.h
 *
 *  Created on: Apr 9, 2015
 *      Author: sbh
 */

#ifndef PARAMETERSPLUGIN_VARIABLEVALIDATOR_H_
#define PARAMETERSPLUGIN_VARIABLEVALIDATOR_H_

#include <ParametersPlugin.h>
#include <ParametersPlugin_Validators.h>

#include <ModelAPI_AttributeValidator.h>

#include <memory>

/**
 * \class ParametersPlugin_VariableValidator
 * \ingroup Validators
 * \brief Validator for the variable name of parameter.
 */
class ParametersPlugin_VariableValidator : public ModelAPI_AttributeValidator
{
 public:
  PARAMETERSPLUGIN_EXPORT ParametersPlugin_VariableValidator();
  PARAMETERSPLUGIN_EXPORT virtual ~ParametersPlugin_VariableValidator();

  /**
   * \brief Returns true if attribute has a valid parameter name.
   * \param theAttribute the checked attribute
   * \param theArguments arguments of the attribute
   * \param theError the error string message if validation fails
   */
  PARAMETERSPLUGIN_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const;

 protected:
  /// Returns true if theString is unique parameter name in the document of theAttribute.
  PARAMETERSPLUGIN_EXPORT bool isUnique(const AttributePtr& theAttribute,
                                        const std::string& theString) const;
};

/**
 * \class ParametersPlugin_ExpressionValidator
 * \ingroup Validators
 * \brief Validator for the expression of parameter.
 */
class ParametersPlugin_ExpressionValidator: public ModelAPI_AttributeValidator
{
 public:
  PARAMETERSPLUGIN_EXPORT ParametersPlugin_ExpressionValidator();
  PARAMETERSPLUGIN_EXPORT virtual ~ParametersPlugin_ExpressionValidator();

  /**
   * \brief Returns true if attribute has a valid parameter expression.
   * \param theAttribute the checked attribute
   * \param theArguments arguments of the attribute
   * \param theError the error string message if validation fails
   */
  PARAMETERSPLUGIN_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const;
};


#endif /* PARAMETERSPLUGIN_VARIABLEVALIDATOR_H_ */
