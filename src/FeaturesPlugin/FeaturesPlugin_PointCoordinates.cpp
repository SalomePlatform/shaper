// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_PointCoordinates.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDoubleArray.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>

#include <Config_PropManager.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Vertex.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <iomanip>
#include <sstream>

FeaturesPlugin_PointCoordinates::FeaturesPlugin_PointCoordinates()
{
}

void FeaturesPlugin_PointCoordinates::initAttributes()
{
  // attribute for point selected
  data()->addAttribute(POINT_SELECTED_ID(), ModelAPI_AttributeSelection::typeId());

  // attribute for x, y and z coordinates
  data()->addAttribute(X_COORD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(Y_COORD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(Z_COORD_ID(), ModelAPI_AttributeString::typeId());

  // attributes for result message and values
  data()->addAttribute(RESULT_VALUES_ID(), ModelAPI_AttributeDoubleArray::typeId());
  data()->realArray(RESULT_VALUES_ID())->setSize(3);
}

void FeaturesPlugin_PointCoordinates::execute()
{
}

void FeaturesPlugin_PointCoordinates::attributeChanged(const std::string& theID)
{
  if (theID == POINT_SELECTED_ID()) {
    AttributeSelectionPtr aSelection = selection(POINT_SELECTED_ID());
    GeomShapePtr aShape;
    GeomPointPtr aPoint;
    if (aSelection && aSelection->isInitialized()) {
      aShape = aSelection->value();
      if (!aShape && aSelection->context())
        aShape = aSelection->context()->shape();
    }

    AttributeDoubleArrayPtr aValues =
      std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(attribute(RESULT_VALUES_ID()));
    std::stringstream streamx;
    std::stringstream streamy;
    std::stringstream streamz;
    if (aShape) {
      aPoint = GeomAlgoAPI_PointBuilder::point(aShape);
      streamx << std::setprecision(14) << aPoint->x();
      aValues->setValue(0, aPoint->x());
      streamy << std::setprecision(14) << aPoint->y();
      aValues->setValue(1, aPoint->y());
      streamz << std::setprecision(14) << aPoint->z();
      aValues->setValue(2, aPoint->z());
    }

    string(X_COORD_ID() )->setValue( "X = " +  streamx.str() );
    string(Y_COORD_ID() )->setValue( "Y = " +  streamy.str() );
    string(Z_COORD_ID() )->setValue( "Z = " +  streamz.str() );
  }

}
