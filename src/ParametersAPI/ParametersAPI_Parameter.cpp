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
    const std::string & theExpression,
    const std::string & theComment)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theName, name());
    fillAttribute(theExpression, expression());
    fillAttribute(theComment, comment());

    execute();
  }
}

ParametersAPI_Parameter::~ParametersAPI_Parameter()
{
}

//--------------------------------------------------------------------------------------
ParameterPtr addParameter(const std::shared_ptr<ModelAPI_Document> & thePart,
                          const std::string & theName,
                          const std::string & theExpression,
                          const std::string & theComment)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ParametersAPI_Parameter::ID());
  return ParameterPtr(new ParametersAPI_Parameter(aFeature, theName, theExpression, theComment));
}
