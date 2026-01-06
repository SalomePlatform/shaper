// Copyright (C) 2017-2026  CEA, EDF
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

#include "FeaturesAPI_Thickness.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================

FeaturesAPI_Thickness::FeaturesAPI_Thickness(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

FeaturesAPI_Thickness::FeaturesAPI_Thickness(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theBaseObject,
                                             const ModelHighAPI_Double& theThickness,
                                             const bool isInside)
  : FeaturesAPI_Thickness(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Thickness::CREATION_METHOD_THICK(), mycreationMethod);
    fillAttribute(theBaseObject, mybaseObject);
    fillAttribute(theThickness, mythicknessValue);
    fillAttribute(isInside, myisInside);

    execute();
  }
}

FeaturesAPI_Thickness::FeaturesAPI_Thickness(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theBaseObject,
                                             const ModelHighAPI_Double& theThickness,
                                             const std::list<ModelHighAPI_Selection>& theFaces,
                                             const bool isInside)
  : FeaturesAPI_Thickness(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Thickness::CREATION_METHOD_HOLLOWED(), mycreationMethod);
    fillAttribute(theBaseObject, mybaseObject);
    fillAttribute(theThickness, mythicknessValue);
    fillAttribute(theFaces, myfaces);
    fillAttribute(isInside, myisInside);

    execute();
  }
}

FeaturesAPI_Thickness::~FeaturesAPI_Thickness()
{
}

void FeaturesAPI_Thickness::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrShape =
    aBase->selection(FeaturesPlugin_Thickness::BASE_SHAPE_ID());

  AttributeDoublePtr anAttrThickness =
    aBase->real(FeaturesPlugin_Thickness::THICKNESS_VALUE_ID());

  AttributeBooleanPtr anAttrIsInside =
    aBase->boolean(FeaturesPlugin_Thickness::INSIDE_ID());

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Thickness::CREATION_METHOD_ID())->value();

  if (aCreationMethod == FeaturesPlugin_Thickness::CREATION_METHOD_THICK()) {
    theDumper << aBase << " = model.addThickness(" << aDocName;
    theDumper << ", " << anAttrShape << ", " << anAttrThickness;
    theDumper << ", " << anAttrIsInside << ")" << std::endl;
  }
  else if (aCreationMethod == FeaturesPlugin_Thickness::CREATION_METHOD_HOLLOWED()) {
    AttributeSelectionListPtr anAttrFaces =
      aBase->selectionList(FeaturesPlugin_Thickness::FACES_ID());

    theDumper << aBase << " = model.addHollowedSolid(" << aDocName;
    theDumper << ", " << anAttrShape << ", " << anAttrThickness;
    theDumper << ", " << anAttrFaces;
    theDumper << ", " << anAttrIsInside << ")" << std::endl;
  }
}

//==================================================================================================

ThicknessPtr addThickness(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Selection& theBaseObject,
                          const ModelHighAPI_Double& theThickness,
                          const bool isInside)
{
  FeaturePtr aFeature = thePart->addFeature(FeaturesAPI_Thickness::ID());

  ThicknessPtr aThickness (new FeaturesAPI_Thickness(aFeature, theBaseObject, theThickness, isInside));
  return aThickness;
}

ThicknessPtr addHollowedSolid(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Selection& theBaseObject,
                              const ModelHighAPI_Double& theThickness,
                              const std::list<ModelHighAPI_Selection>& theFaces,
                              const bool isInside)
{
  FeaturePtr aFeature = thePart->addFeature(FeaturesAPI_Thickness::ID());

  ThicknessPtr aThickness (new FeaturesAPI_Thickness(aFeature, theBaseObject, theThickness, theFaces, isInside));
  return aThickness;
}
