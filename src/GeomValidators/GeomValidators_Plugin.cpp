// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include <GeomValidators_Plugin.h>

#include <GeomValidators_BodyShapes.h>
#include <GeomValidators_BooleanArguments.h>
#include <GeomValidators_BooleanSelection.h>
#include <GeomValidators_ConstructionComposite.h>
#include <GeomValidators_Different.h>
#include <GeomValidators_DifferentShapes.h>
#include <GeomValidators_Face.h>
#include <GeomValidators_Finite.h>
#include <GeomValidators_PartitionArguments.h>
#include <GeomValidators_ShapeType.h>
#include <GeomValidators_ZeroOffset.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

// the only created instance of this plugin
static GeomValidators_Plugin* MY_GEOMVALIDATORS_INSTANCE = new GeomValidators_Plugin();

GeomValidators_Plugin::GeomValidators_Plugin()
{
  // register validators
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  aFactory->registerValidator("GeomValidators_BodyShapes", new GeomValidators_BodyShapes);
  aFactory->registerValidator("GeomValidators_BooleanArguments", new GeomValidators_BooleanArguments);
  aFactory->registerValidator("GeomValidators_ConstructionComposite", new GeomValidators_ConstructionComposite);
  aFactory->registerValidator("GeomValidators_Different", new GeomValidators_Different);
  aFactory->registerValidator("GeomValidators_DifferentShapes", new GeomValidators_DifferentShapes);
  aFactory->registerValidator("GeomValidators_Face", new GeomValidators_Face);
  aFactory->registerValidator("GeomValidators_Finite", new GeomValidators_Finite);
  aFactory->registerValidator("GeomValidators_PartitionArguments", new GeomValidators_PartitionArguments);
  aFactory->registerValidator("GeomValidators_ShapeType", new GeomValidators_ShapeType);
  aFactory->registerValidator("GeomValidators_ZeroOffset", new GeomValidators_ZeroOffset);
  aFactory->registerValidator("GeomValidators_BooleanSelection", new GeomValidators_BooleanSelection);

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr GeomValidators_Plugin::createFeature(std::string theFeatureID)
{
  // feature of such kind is not found
  return FeaturePtr();
}
