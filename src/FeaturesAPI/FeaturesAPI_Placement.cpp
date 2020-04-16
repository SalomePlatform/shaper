// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_Placement.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Placement::FeaturesAPI_Placement(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Placement::FeaturesAPI_Placement(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<ModelHighAPI_Selection>& theObjects,
                                             const ModelHighAPI_Selection& theStartShape,
                                             const ModelHighAPI_Selection& theEndShape,
                                             const bool theReverseDirection,
                                             const bool theCentering)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theObjects, myobjects);
    fillAttribute(theStartShape, mystartShape);
    fillAttribute(theEndShape, myendShape);
    fillAttribute(theReverseDirection, myreverseDirection);
    setCentering(theCentering);
  }
}

//==================================================================================================
FeaturesAPI_Placement::~FeaturesAPI_Placement()
{

}

//==================================================================================================
void FeaturesAPI_Placement::setObjects(const std::list<ModelHighAPI_Selection>& theObjects)
{
  fillAttribute(theObjects, myobjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Placement::setStartShape(const ModelHighAPI_Selection& theStartShape)
{
  fillAttribute(theStartShape, mystartShape);

  execute();
}

//==================================================================================================
void FeaturesAPI_Placement::setEndShape(const ModelHighAPI_Selection& theEndShape)
{
  fillAttribute(theEndShape, myendShape);

  execute();
}

//==================================================================================================
void FeaturesAPI_Placement::setReverseDirection(const bool theReverseDirection)
{
  fillAttribute(theReverseDirection, myreverseDirection);

  execute();
}

//==================================================================================================
void FeaturesAPI_Placement::setCentering(const bool theCentering)
{
  fillAttribute(theCentering, mycentering);

  execute();
}

//==================================================================================================
void FeaturesAPI_Placement::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Placement::OBJECTS_LIST_ID());
  AttributeSelectionPtr anAttrStartShape =
    aBase->selection(FeaturesPlugin_Placement::START_SHAPE_ID());
  AttributeSelectionPtr anAttrEndShape = aBase->selection(FeaturesPlugin_Placement::END_SHAPE_ID());
  AttributeBooleanPtr anAttrReverse = aBase->boolean(FeaturesPlugin_Placement::REVERSE_ID());
  AttributeBooleanPtr anAttrCentering = aBase->boolean(FeaturesPlugin_Placement::CENTERING_ID());

  theDumper << aBase << " = model.addPlacement(" << aDocName << ", "
            << anAttrObjects << ", " << anAttrStartShape << ", " << anAttrEndShape;

  if (anAttrReverse->value())
    theDumper << ", reverse = " << anAttrReverse;
  if (anAttrCentering->value())
    theDumper << ", centering = " << anAttrCentering;
  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
PlacementPtr addPlacement(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theObjects,
                          const ModelHighAPI_Selection& theStartShape,
                          const ModelHighAPI_Selection& theEndShape,
                          const bool theReverseDirection,
                          const bool theCentering,
                          const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Placement::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");
  return PlacementPtr(new FeaturesAPI_Placement(aFeature,
                                                theObjects,
                                                theStartShape,
                                                theEndShape,
                                                theReverseDirection,
                                                theCentering));
}
