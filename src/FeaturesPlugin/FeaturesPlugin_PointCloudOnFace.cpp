// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_PointCloudOnFace.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <Config_PropManager.h>
#include <GeomAlgoAPI_PointCloudOnFace.h>
#include <ModelAPI_ResultConstruction.h>

#include <iomanip>
#include <sstream>

//=================================================================================================
FeaturesPlugin_PointCloudOnFace::FeaturesPlugin_PointCloudOnFace()
{
}

//=================================================================================================
void FeaturesPlugin_PointCloudOnFace::initAttributes()
{
  // attribute for object selected
  data()->addAttribute(FACE_SELECTED_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(NUMBER_ID(), ModelAPI_AttributeInteger::typeId());
}

//=================================================================================================
void FeaturesPlugin_PointCloudOnFace::attributeChanged(const std::string& /*theID*/)
{
}

//=================================================================================================
void FeaturesPlugin_PointCloudOnFace::execute()
{

  AttributeSelectionPtr aSelectionFace = selection(FACE_SELECTED_ID());

  GeomShapePtr aShape;

  if (aSelectionFace && aSelectionFace->isInitialized()) {
    aShape = aSelectionFace->value();
    if (!aShape && aSelectionFace->context())
      aShape = aSelectionFace->context()->shape();
  }

  // Getting number of points
  int aNbPnts = integer(FeaturesPlugin_PointCloudOnFace::NUMBER_ID())->value();

  if (aShape && aNbPnts >= 2) {
    std::string aError;
    std::shared_ptr<GeomAPI_Shape> aPoints(new GeomAPI_Shape);
    if (!GeomAlgoAPI_PointCloudOnFace::PointCloud(aShape,
                                                  aNbPnts,
                                                  aPoints,
                                                  aError))
      setError("Error in points calculation :" +  aError);

    ResultConstructionPtr aConstr = document()->createConstruction(data());
    aConstr->setInfinite(true);
    aConstr->setShape(aPoints);
    setResult(aConstr);
  }
}
