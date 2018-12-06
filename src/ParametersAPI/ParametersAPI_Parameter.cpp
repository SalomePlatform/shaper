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

#include "ParametersAPI_Parameter.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Events.h>
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

void ParametersAPI_Parameter::setValue(const double theValue)
{
  // convert value to the expression string
  std::ostringstream aValueStr;
  aValueStr<<theValue;
  fillAttribute(aValueStr.str(), expression());
  execute();
}

double ParametersAPI_Parameter::value() {
  ResultParameterPtr aRes =
    std::dynamic_pointer_cast<ModelAPI_ResultParameter>(feature()->firstResult());
  // it may raise an exception if result is invalid
  return aRes->data()->real(ModelAPI_ResultParameter::VALUE())->value();
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
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ParametersAPI_Parameter::ID());
  return ParameterPtr(new ParametersAPI_Parameter(aFeature, theName, theExpression, theComment));
}

//--------------------------------------------------------------------------------------
void removeParameter(const std::shared_ptr<ModelAPI_Document> & thePart,
                     const ParameterPtr & theParameter)
{
  FeaturePtr aParam = theParameter->feature();
  if (aParam) {
    ModelAPI_ReplaceParameterMessage::send(aParam, 0);
    thePart->removeFeature(aParam);
  }
}
