// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->
// Name   : ParametersAPI_Parameter.h
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

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
