// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include "FiltersPlugin_BelongsTo.h"
#include "FiltersPlugin_ContinuousFaces.h"
#include "FiltersPlugin_EdgeSize.h"
#include "FiltersPlugin_ExternalFaces.h"
#include "FiltersPlugin_FaceSize.h"
#include "FiltersPlugin_FeatureEdges.h"
#include "FiltersPlugin_HorizontalFace.h"
#include "FiltersPlugin_OnGeometry.h"
#include "FiltersPlugin_OnLine.h"
#include "FiltersPlugin_OnPlane.h"
#include "FiltersPlugin_OnPlaneSide.h"
#include "FiltersPlugin_OppositeToEdge.h"
#include "FiltersPlugin_Plugin.h"
#include "FiltersPlugin_RelativeToSolid.h"
#include "FiltersPlugin_Selection.h"
#include "FiltersPlugin_Validators.h"
#include "FiltersPlugin_VerticalFace.h"
#include "FiltersPlugin_VolumeSize.h"

#include <Config_ModuleReader.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_FiltersFactory.h>

// the only created instance of this plugin
static FiltersPlugin_Plugin* MY_VIEWFILTERS_INSTANCE = new FiltersPlugin_Plugin();

FiltersPlugin_Plugin::FiltersPlugin_Plugin()
{
  // register filters
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_FiltersFactory* aFactory = aMgr->filters();
  aFactory->registerFilter("HorizontalFaces", new FiltersPlugin_HorizontalFace);
  aFactory->registerFilter("VerticalFaces", new FiltersPlugin_VerticalFace);
  aFactory->registerFilter("BelongsTo", new FiltersPlugin_BelongsTo);
  aFactory->registerFilter("OnPlane", new FiltersPlugin_OnPlane);
  aFactory->registerFilter("OnLine", new FiltersPlugin_OnLine);
  aFactory->registerFilter("OnGeometry", new FiltersPlugin_OnGeometry);
  aFactory->registerFilter("OnPlaneSide", new FiltersPlugin_OnPlaneSide);
  aFactory->registerFilter("OppositeToEdge", new FiltersPlugin_OppositeToEdge);
  aFactory->registerFilter("RelativeToSolid", new FiltersPlugin_RelativeToSolid);
  aFactory->registerFilter("ExternalFaces", new FiltersPlugin_ExternalFaces);
  aFactory->registerFilter("EdgeSize", new FiltersPlugin_EdgeSize);
  aFactory->registerFilter("FaceSize", new FiltersPlugin_FaceSize);
  aFactory->registerFilter("VolumeSize", new FiltersPlugin_VolumeSize);
  aFactory->registerFilter("FeatureEdges", new FiltersPlugin_FeatureEdges);
  aFactory->registerFilter("ContinuousFaces", new FiltersPlugin_ContinuousFaces);

  Config_ModuleReader::loadScript("FiltersPlugin_TopoConnectedFaces");

  // register validators
  ModelAPI_ValidatorsFactory* aValidators = aMgr->validators();
  aValidators->registerValidator("FiltersPlugin_ShapeType",
                                 new FiltersPlugin_ShapeTypeValidator);

  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr FiltersPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == FiltersPlugin_Selection::ID())
    return FeaturePtr(new FiltersPlugin_Selection);
  // feature of such kind is not found
  return FeaturePtr();
}
