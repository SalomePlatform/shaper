// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->
// Name   : ParametersAPI_Parameter.cpp
// Purpose:
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ParametersAPI_Parameter.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
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
    if (!theComment.empty())
      fillAttribute(theComment, comment());

    execute();
  }
}

ParametersAPI_Parameter::~ParametersAPI_Parameter()
{
}

void ParametersAPI_Parameter::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());
  const std::string& aParamName = theDumper.name(aBase, false, true);

  AttributeStringPtr anExpr   = aBase->string(ParametersPlugin_Parameter::EXPRESSION_ID());
  AttributeStringPtr aComment = aBase->string(ParametersPlugin_Parameter::COMMENT_ID());

  theDumper << "model.addParameter(" << aDocName << ", \"" << aParamName << "\", " << anExpr;
  if (aComment->isInitialized() && !aComment->value().empty())
    theDumper << ", " << aComment;
  theDumper << ")" << std::endl;
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
