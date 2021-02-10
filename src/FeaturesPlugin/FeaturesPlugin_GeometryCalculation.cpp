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

#include "FeaturesPlugin_GeometryCalculation.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDoubleArray.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>

#include <Config_PropManager.h>

#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_ShapeTools.h>


#include <iomanip>
#include <sstream>

//=================================================================================================
FeaturesPlugin_GeometryCalculation::FeaturesPlugin_GeometryCalculation()
{
}

//=================================================================================================
void FeaturesPlugin_GeometryCalculation::initAttributes()
{
  // attribute for point selected
  data()->addAttribute(OBJECT_SELECTED_ID(), ModelAPI_AttributeSelection::typeId());

  // attributes for result message and values
  data()->addAttribute(LENGTH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(AREA_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(VOLUME_ID(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(RESULT_VALUES_ID(), ModelAPI_AttributeDoubleArray::typeId());
  data()->realArray(RESULT_VALUES_ID())->setSize(3);

}

//=================================================================================================
void FeaturesPlugin_GeometryCalculation::execute()
{
}

//=================================================================================================
void FeaturesPlugin_GeometryCalculation::attributeChanged(const std::string& theID)
{
  if (theID == OBJECT_SELECTED_ID()) {

    AttributeSelectionPtr aSelection = selection(OBJECT_SELECTED_ID());
    AttributeDoubleArrayPtr aValues =
      std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(attribute(RESULT_VALUES_ID()));
    std::stringstream streamL;
    std::stringstream streamA;
    std::stringstream streamV;
    GeomShapePtr aShape;

    if (aSelection && aSelection->isInitialized()) {
      aShape = aSelection->value();
      if (!aShape && aSelection->context())
        aShape = aSelection->context()->shape();
    }

    if (aShape) {
      double aTolerance = 0.0001;
      double aLength;
      double aSurfArea;
      double aVolume;

      aLength = GeomAlgoAPI_ShapeTools::length(aShape);
      aSurfArea = GeomAlgoAPI_ShapeTools::area(aShape);
      aVolume = GeomAlgoAPI_ShapeTools::volume(aShape);

      streamL << std::setprecision(14) << aLength;
      aValues->setValue(0, aLength);
      streamA << std::setprecision(14) << aSurfArea;
      aValues->setValue(1, aSurfArea);
      streamV << std::setprecision(14) << aVolume;
      aValues->setValue(2, aVolume);
    }

    string(LENGTH_ID())->setValue(streamL.str());
    string(AREA_ID())->setValue(streamA.str());
    string(VOLUME_ID())->setValue(streamV.str());
  }
}
