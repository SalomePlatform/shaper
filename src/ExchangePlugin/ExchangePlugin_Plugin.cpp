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

#include <ExchangePlugin_Plugin.h>
#include <ExchangePlugin_Dump.h>
#include <ExchangePlugin_ImportFeature.h>
#include <ExchangePlugin_ExportFeature.h>
#include <ExchangePlugin_Validators.h>

#include <Config_PropManager.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <memory>

// the only created instance of this plugin
static ExchangePlugin_Plugin* MY_EXCHANGE_INSTANCE = new ExchangePlugin_Plugin();

ExchangePlugin_Plugin::ExchangePlugin_Plugin()
{
  // register this plugin
  SessionPtr aSession = ModelAPI_Session::get();
  aSession->registerPlugin(this);
  ModelAPI_ValidatorsFactory* aFactory = aSession->validators();
  aFactory->registerValidator("ExchangePlugin_ImportFormat",
                              new ExchangePlugin_ImportFormatValidator);
  aFactory->registerValidator("ExchangePlugin_ExportFormat",
                              new ExchangePlugin_ExportFormatValidator);
}

FeaturePtr ExchangePlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == ExchangePlugin_ImportFeature::ID()) {
    return FeaturePtr(new ExchangePlugin_ImportFeature);
  } else
  if (theFeatureID == ExchangePlugin_ExportFeature::ID()) {
    return FeaturePtr(new ExchangePlugin_ExportFeature);
  } else
  if (theFeatureID == ExchangePlugin_Dump::ID()) {
    return FeaturePtr(new ExchangePlugin_Dump);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
