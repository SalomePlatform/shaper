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

#include <GeomValidators_Plugin.h>

#include <GeomValidators_BodyShapes.h>
#include <GeomValidators_BooleanArguments.h>
#include <GeomValidators_ConstructionComposite.h>
#include <GeomValidators_Different.h>
#include <GeomValidators_DifferentShapes.h>
#include <GeomValidators_Face.h>
#include <GeomValidators_Finite.h>
#include <GeomValidators_PartitionArguments.h>
#include <GeomValidators_ShapeType.h>
#include <GeomValidators_ZeroOffset.h>
#include <GeomValidators_IntersectionSelection.h>
#include <GeomValidators_FeatureKind.h>
#include <GeomValidators_MinObjectsSelected.h>

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
  aFactory->registerValidator("GeomValidators_BooleanArguments",
                              new GeomValidators_BooleanArguments);
  aFactory->registerValidator("GeomValidators_ConstructionComposite",
                              new GeomValidators_ConstructionComposite);
  aFactory->registerValidator("GeomValidators_Different", new GeomValidators_Different);
  aFactory->registerValidator("GeomValidators_DifferentShapes",
                              new GeomValidators_DifferentShapes);
  aFactory->registerValidator("GeomValidators_Face", new GeomValidators_Face);
  aFactory->registerValidator("GeomValidators_Finite", new GeomValidators_Finite);
  aFactory->registerValidator("GeomValidators_PartitionArguments",
                              new GeomValidators_PartitionArguments);
  aFactory->registerValidator("GeomValidators_ShapeType", new GeomValidators_ShapeType);
  aFactory->registerValidator("GeomValidators_ZeroOffset", new GeomValidators_ZeroOffset);
  aFactory->registerValidator("GeomValidators_IntersectionSelection",
                              new GeomValidators_IntersectionSelection);
  aFactory->registerValidator("GeomValidators_FeatureKind", new GeomValidators_FeatureKind);
  aFactory->registerValidator("GeomValidators_MinObjectsSelected",
                              new GeomValidators_MinObjectsSelected);

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr GeomValidators_Plugin::createFeature(std::string theFeatureID)
{
  // feature of such kind is not found
  return FeaturePtr();
}
