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
                                   const ModelHighAPI_Double & theExpression);
  /// Destructor
  PARAMETERSAPI_EXPORT
  virtual ~ParametersAPI_Parameter();

  INTERFACE_2(ParametersPlugin_Parameter::ID(),
              name, ParametersPlugin_Parameter::VARIABLE_ID(), ModelAPI_AttributeString, /** Name */,
              expression, ParametersPlugin_Parameter::EXPRESSION_ID(), ModelAPI_AttributeDouble, /** Expression */
  )

};

//! Pointer on Parameter object
typedef std::shared_ptr<ParametersAPI_Parameter> ParameterPtr;

/**\ingroup CPPHighAPI
 * \brief Create Parameter feature
 */
PARAMETERSAPI_EXPORT
ParameterPtr addParameter(const std::shared_ptr<ModelAPI_Document> & thePart,
                          const std::string & theName,
                          const ModelHighAPI_Double & theExpression);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_PARAMETERSAPI_PARAMETERSAPI_PARAMETER_H_ */
