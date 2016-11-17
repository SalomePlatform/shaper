// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include <CollectionPlugin_Plugin.h>

#include <CollectionPlugin_Group.h>
#include <CollectionPlugin_Field.h>
#include <ModelAPI_Session.h>

#include <ModuleBase_WidgetCreatorFactory.h>

#include "CollectionPlugin_WidgetCreator.h"

#include <string>
#include <memory>

// the only created instance of this plugin
static CollectionPlugin_Plugin* MY_COLLECTION_INSTANCE = new CollectionPlugin_Plugin();

CollectionPlugin_Plugin::CollectionPlugin_Plugin()
{
  WidgetCreatorFactoryPtr aWidgetCreatorFactory = ModuleBase_WidgetCreatorFactory::get();
  aWidgetCreatorFactory->registerCreator(
   std::shared_ptr<CollectionPlugin_WidgetCreator>(new CollectionPlugin_WidgetCreator()));

  SessionPtr aMgr = ModelAPI_Session::get();
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr CollectionPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == CollectionPlugin_Group::ID()) {
    return FeaturePtr(new CollectionPlugin_Group);
  }else if (theFeatureID == CollectionPlugin_Field::ID()) {
    return FeaturePtr(new CollectionPlugin_Field);
  }


  // feature of such kind is not found
  return FeaturePtr();
}
