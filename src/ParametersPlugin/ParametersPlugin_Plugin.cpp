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

#include <pyconfig.h>

#include <ParametersPlugin_Plugin.h>
#include <ParametersPlugin_Parameter.h>
#include <ParametersPlugin_ParametersMgr.h>
#include <ParametersPlugin_Validators.h>
#include <ParametersPlugin_WidgetCreator.h>

#include <ModuleBase_WidgetCreatorFactory.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <memory>

// the only created instance of this plugin
static ParametersPlugin_Plugin* MY_PARAMETERSPLUGIN_INSTANCE = new ParametersPlugin_Plugin();

ParametersPlugin_Plugin::ParametersPlugin_Plugin()
{
  // register this plugin
  WidgetCreatorFactoryPtr aWidgetCreatorFactory = ModuleBase_WidgetCreatorFactory::get();
  aWidgetCreatorFactory->registerCreator(
   std::shared_ptr<ParametersPlugin_WidgetCreator>(new ParametersPlugin_WidgetCreator()));

  SessionPtr aSession = ModelAPI_Session::get();
  aSession->registerPlugin(this);

  ModelAPI_ValidatorsFactory* aFactory = aSession->validators();
  aFactory->registerValidator("Parameters_VariableValidator",
                              new ParametersPlugin_VariableValidator);
  aFactory->registerValidator("Parameters_ExpressionValidator",
                              new ParametersPlugin_ExpressionValidator);

  myEvalListener =
    std::shared_ptr<ParametersPlugin_EvalListener>(new ParametersPlugin_EvalListener());
}

FeaturePtr ParametersPlugin_Plugin::createFeature(std::string theFeatureID)
{
  // TODO: register some features
  if (theFeatureID == ParametersPlugin_Parameter::ID()) {
    return FeaturePtr(new ParametersPlugin_Parameter);
  }
  if (theFeatureID == ParametersPlugin_ParametersMgr::ID()) {
    return FeaturePtr(new ParametersPlugin_ParametersMgr);
  }
  return FeaturePtr();
}

