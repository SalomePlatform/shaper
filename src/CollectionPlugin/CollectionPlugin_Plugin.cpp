// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include <CollectionPlugin_Plugin.h>

#include <CollectionPlugin_Group.h>
#include <ModelAPI_Session.h>

#include <string>
#include <memory>

// the only created instance of this plugin
static CollectionPlugin_Plugin* MY_COLLECTION_INSTANCE = new CollectionPlugin_Plugin();

CollectionPlugin_Plugin::CollectionPlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr CollectionPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == CollectionPlugin_Group::ID()) {
    return FeaturePtr(new CollectionPlugin_Group);
  }

  // feature of such kind is not found
  return FeaturePtr();
}
