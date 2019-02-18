// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <SamplePanelPlugin_Plugin.h>
#include <SamplePanelPlugin_Feature.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_WidgetCreatorFactory.h>
#include <SamplePanelPlugin_WidgetCreator.h>
#include <SamplePanelPlugin_ModelWidgetCreator.h>

// the only created instance of this plugin
static SamplePanelPlugin_Plugin* MY_SAMPLE_PANEL_PLUGIN_INSTANCE = new SamplePanelPlugin_Plugin();

SamplePanelPlugin_Plugin::SamplePanelPlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();

  WidgetCreatorFactoryPtr aWidgetCreatorFactory = ModuleBase_WidgetCreatorFactory::get();
  aWidgetCreatorFactory->registerCreator(
          std::shared_ptr<SamplePanelPlugin_WidgetCreator>(new SamplePanelPlugin_WidgetCreator()));
  aWidgetCreatorFactory->registerCreator(
   std::shared_ptr<SamplePanelPlugin_ModelWidgetCreator>(
   new SamplePanelPlugin_ModelWidgetCreator()));

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr SamplePanelPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == SamplePanelPlugin_Feature::ID()) {
    return FeaturePtr(new SamplePanelPlugin_Feature);
  }
  // feature of such kind is not found
  return FeaturePtr();
}

