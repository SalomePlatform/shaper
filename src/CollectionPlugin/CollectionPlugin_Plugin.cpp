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

#include <CollectionPlugin_Plugin.h>

#include <CollectionPlugin_Group.h>
#include <CollectionPlugin_Field.h>
#include <CollectionPlugin_Validators.h>
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

  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("CollectionPlugin_FieldValidator",
    new CollectionPlugin_FieldValidator);

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr CollectionPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == CollectionPlugin_Group::ID()) {
    return FeaturePtr(new CollectionPlugin_Group);
  } else if (theFeatureID == CollectionPlugin_Field::ID()) {
    return FeaturePtr(new CollectionPlugin_Field);
  }

  // feature of such kind is not found
  return FeaturePtr();
}
