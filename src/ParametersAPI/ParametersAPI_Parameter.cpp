// Name   : ParametersAPI_Parameter.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ParametersAPI_Parameter.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
ParametersAPI_Parameter::ParametersAPI_Parameter(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

ParametersAPI_Parameter::ParametersAPI_Parameter(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theName,
    const ModelHighAPI_Double & theExpression)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theName, name());
    fillAttribute(theExpression, expression());

    execute();
  }
}

ParametersAPI_Parameter::~ParametersAPI_Parameter()
{
}

//--------------------------------------------------------------------------------------
ParameterPtr addParameter(const std::shared_ptr<ModelAPI_Document> & theParameter,
                          const std::string & theName,
                          const ModelHighAPI_Double & theExpression)
{
  // TODO(spo): check that theParameter is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = theParameter->addFeature(ParametersAPI_Parameter::ID());
  return ParameterPtr(new ParametersAPI_Parameter(aFeature));
}
