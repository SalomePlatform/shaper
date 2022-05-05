// Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_Measurement.h"

#include <FeaturesPlugin_Measurement.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelHighAPI_Services.h>
#include <ModelHighAPI_Tools.h>

double measureLength(const std::shared_ptr<ModelAPI_Document>& thePart,
                     const ModelHighAPI_Selection& theEdge)
{
  FeaturePtr aMeasure = thePart->addFeature(FeaturesPlugin_Measurement::ID());
  fillAttribute(FeaturesPlugin_Measurement::MEASURE_LENGTH(),
                aMeasure->string(FeaturesPlugin_Measurement::MEASURE_KIND()));
  fillAttribute(theEdge, aMeasure->selection(FeaturesPlugin_Measurement::EDGE_FOR_LENGTH_ID()));
  aMeasure->execute();

  // obtain result
  AttributeDoubleArrayPtr aResult = std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(
      aMeasure->attribute(FeaturesPlugin_Measurement::RESULT_VALUES_ID()));
  double aValue = aResult->size() ? aResult->value(0) : -1.0;

  // perform removing macro feature Measurement
  thePart->removeFeature(aMeasure);
  apply();

  return aValue;
}

double measureDistance(const std::shared_ptr<ModelAPI_Document>& thePart,
                       const ModelHighAPI_Selection& theFrom,
                       const ModelHighAPI_Selection& theTo)
{
  FeaturePtr aMeasure = thePart->addFeature(FeaturesPlugin_Measurement::ID());
  fillAttribute(FeaturesPlugin_Measurement::MEASURE_DISTANCE(),
                aMeasure->string(FeaturesPlugin_Measurement::MEASURE_KIND()));
  fillAttribute(theFrom,
                aMeasure->selection(FeaturesPlugin_Measurement::DISTANCE_FROM_OBJECT_ID()));
  fillAttribute(theTo, aMeasure->selection(FeaturesPlugin_Measurement::DISTANCE_TO_OBJECT_ID()));
  aMeasure->execute();

  // obtain result
  AttributeDoubleArrayPtr aResult = std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(
      aMeasure->attribute(FeaturesPlugin_Measurement::RESULT_VALUES_ID()));
  double aValue = aResult->size() ? aResult->value(0) : -1.0;

  // perform removing macro feature Measurement
  thePart->removeFeature(aMeasure);
  apply();

  return aValue;
}

double measureRadius(const std::shared_ptr<ModelAPI_Document>& thePart,
                     const ModelHighAPI_Selection& theObject)
{
  FeaturePtr aMeasure = thePart->addFeature(FeaturesPlugin_Measurement::ID());
  fillAttribute(FeaturesPlugin_Measurement::MEASURE_RADIUS(),
                aMeasure->string(FeaturesPlugin_Measurement::MEASURE_KIND()));
  fillAttribute(theObject, aMeasure->selection(FeaturesPlugin_Measurement::CIRCULAR_OBJECT_ID()));
  aMeasure->execute();

  // obtain result
  AttributeDoubleArrayPtr aResult = std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(
      aMeasure->attribute(FeaturesPlugin_Measurement::RESULT_VALUES_ID()));
  double aValue = aResult->size() ? aResult->value(0) : -1.0;

  // perform removing macro feature Measurement
  thePart->removeFeature(aMeasure);
  apply();

  return aValue;
}

std::list<double> measureAngle(const std::shared_ptr<ModelAPI_Document>& thePart,
                               const ModelHighAPI_Selection& theFrom,
                               const ModelHighAPI_Selection& theTo)
{
  FeaturePtr aMeasure = thePart->addFeature(FeaturesPlugin_Measurement::ID());
  fillAttribute(FeaturesPlugin_Measurement::MEASURE_ANGLE(),
                aMeasure->string(FeaturesPlugin_Measurement::MEASURE_KIND()));
  fillAttribute(theFrom, aMeasure->selection(FeaturesPlugin_Measurement::ANGLE_FROM_EDGE_ID()));
  fillAttribute(theTo, aMeasure->selection(FeaturesPlugin_Measurement::ANGLE_TO_EDGE_ID()));
  aMeasure->execute();

  // obtain result
  AttributeDoubleArrayPtr aResult = std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(
      aMeasure->attribute(FeaturesPlugin_Measurement::RESULT_VALUES_ID()));
  std::list<double> aValues;
  for (int i = 0, n = aResult->size(); i < n; ++i)
    aValues.push_back(aResult->value(i));

  // perform removing macro feature Measurement
  thePart->removeFeature(aMeasure);
  apply();

  return aValues;
}

double measureAngle(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& thePoint1,
                    const ModelHighAPI_Selection& thePoint2,
                    const ModelHighAPI_Selection& thePoint3)
{
  FeaturePtr aMeasure = thePart->addFeature(FeaturesPlugin_Measurement::ID());
  fillAttribute(FeaturesPlugin_Measurement::MEASURE_ANGLE_POINTS(),
                aMeasure->string(FeaturesPlugin_Measurement::MEASURE_KIND()));
  fillAttribute(thePoint1, aMeasure->selection(FeaturesPlugin_Measurement::ANGLE_POINT1_ID()));
  fillAttribute(thePoint2, aMeasure->selection(FeaturesPlugin_Measurement::ANGLE_POINT2_ID()));
  fillAttribute(thePoint3, aMeasure->selection(FeaturesPlugin_Measurement::ANGLE_POINT3_ID()));
  aMeasure->execute();

  // obtain result
  AttributeDoubleArrayPtr aResult = std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(
      aMeasure->attribute(FeaturesPlugin_Measurement::RESULT_VALUES_ID()));
  double aValue = aResult->size() ? aResult->value(0) : -1.0;

  // perform removing macro feature Measurement
  thePart->removeFeature(aMeasure);
  apply();

  return aValue;
}
