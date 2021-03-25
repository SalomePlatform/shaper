// Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_PointCoordinates.h"

#include <FeaturesPlugin_PointCoordinates.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelHighAPI_Services.h>
#include <ModelHighAPI_Tools.h>

std::list<double> getPointCoordinates(const std::shared_ptr<ModelAPI_Document>& thePart,
                                      const ModelHighAPI_Selection& thePoint)
{
  FeaturePtr aPointCoordFeat = thePart->addFeature(FeaturesPlugin_PointCoordinates::ID());

  fillAttribute(thePoint, aPointCoordFeat
                      ->selection(FeaturesPlugin_PointCoordinates::POINT_SELECTED_ID()));
  std::list<double> res;

  // obtain result
  AttributeDoubleArrayPtr aResult = std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(
      aPointCoordFeat->attribute(FeaturesPlugin_PointCoordinates::RESULT_VALUES_ID()));

  for ( int i : {0, 1, 2})
    res.push_back( aResult->value(i));

  return res;
}
