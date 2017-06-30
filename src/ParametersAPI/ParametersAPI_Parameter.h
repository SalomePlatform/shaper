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

#ifndef SRC_PARAMETERSAPI_PARAMETERSAPI_PARAMETER_H_
#define SRC_PARAMETERSAPI_PARAMETERSAPI_PARAMETER_H_

//--------------------------------------------------------------------------------------
#include "ParametersAPI.h"

#include <ParametersPlugin_Parameter.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelHighAPI_Double;
//--------------------------------------------------------------------------------------
/**\class ParametersAPI_Parameter
 * \ingroup CPPHighAPI
 * \brief Interface for Parameter feature
 */
class ParametersAPI_Parameter : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  PARAMETERSAPI_EXPORT
  explicit ParametersAPI_Parameter(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  PARAMETERSAPI_EXPORT
  explicit ParametersAPI_Parameter(const  std::shared_ptr<ModelAPI_Feature> & theFeature,
                                   const std::string & theName,
                                   const std::string & theExpression,
                                   const std::string & theComment = std::string());
  /// Destructor
  PARAMETERSAPI_EXPORT
  virtual ~ParametersAPI_Parameter();

  INTERFACE_3(ParametersPlugin_Parameter::ID(),
              name, ParametersPlugin_Parameter::VARIABLE_ID(),
              ModelAPI_AttributeString, /** Name */,
              expression, ParametersPlugin_Parameter::EXPRESSION_ID(),
              ModelAPI_AttributeString, /** Expression */,
              comment, ParametersPlugin_Parameter::COMMENT_ID(),
              ModelAPI_AttributeString, /** Comment */
  )

  /// Just sets the numeric value to the parameter (it resets the previous expression)
  PARAMETERSAPI_EXPORT void setValue(const double theValue);
  /// Returns the current value of the parameter: the expression result.
  PARAMETERSAPI_EXPORT double value();

  /// Dump wrapped feature
  PARAMETERSAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

};

//! Pointer on Parameter object
typedef std::shared_ptr<ParametersAPI_Parameter> ParameterPtr;

/**\ingroup CPPHighAPI
 * \brief Create Parameter feature
 */
PARAMETERSAPI_EXPORT
ParameterPtr addParameter(const std::shared_ptr<ModelAPI_Document> & thePart,
                          const std::string & theName,
                          const std::string & theExpression,
                          const std::string & theComment = std::string());

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_PARAMETERSAPI_PARAMETERSAPI_PARAMETER_H_ */
