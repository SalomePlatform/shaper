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

#include "FeaturesAPI_Translation.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Translation::FeaturesAPI_Translation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Translation::FeaturesAPI_Translation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const ModelHighAPI_Selection& theAxisObject,
  const ModelHighAPI_Double& theDistance)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    setAxisAndDistance(theAxisObject, theDistance);
  }
}

//==================================================================================================
FeaturesAPI_Translation::FeaturesAPI_Translation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const ModelHighAPI_Double& theDx,
  const ModelHighAPI_Double& theDy,
  const ModelHighAPI_Double& theDz)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    setDimensions(theDx, theDy, theDz);
  }
}

//==================================================================================================
FeaturesAPI_Translation::FeaturesAPI_Translation(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const ModelHighAPI_Selection& theStartPoint,
  const ModelHighAPI_Selection& theEndPoint)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    setPoints(theStartPoint, theEndPoint);
  }
}

//==================================================================================================
FeaturesAPI_Translation::~FeaturesAPI_Translation()
{

}

//==================================================================================================
void FeaturesAPI_Translation::setMainObjects(
  const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Translation::setAxisAndDistance(const ModelHighAPI_Selection& theAxisObject,
                                                 const ModelHighAPI_Double& theDistance)
{
  fillAttribute(FeaturesPlugin_Translation::CREATION_METHOD_BY_DISTANCE(), mycreationMethod);
  fillAttribute(theAxisObject, myaxisObject);
  fillAttribute(theDistance, mydistance);

  execute();
}

//==================================================================================================
void FeaturesAPI_Translation::setDimensions(const ModelHighAPI_Double& theDx,
                                            const ModelHighAPI_Double& theDy,
                                            const ModelHighAPI_Double& theDz)
{
  fillAttribute(FeaturesPlugin_Translation::CREATION_METHOD_BY_DIMENSIONS(), mycreationMethod);
  fillAttribute(theDx, mydx);
  fillAttribute(theDy, mydy);
  fillAttribute(theDz, mydz);

  execute();
}

//==================================================================================================
void FeaturesAPI_Translation::setPoints(const ModelHighAPI_Selection& theStartPoint,
                                        const ModelHighAPI_Selection& theEndPoint)
{
  fillAttribute(FeaturesPlugin_Translation::CREATION_METHOD_BY_TWO_POINTS(), mycreationMethod);
  fillAttribute(theStartPoint, mystartPoint);
  fillAttribute(theEndPoint, myendPoint);

  execute();
}

//==================================================================================================
void FeaturesAPI_Translation::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Translation::OBJECTS_LIST_ID());
  theDumper << aBase << " = model.addTranslation(" << aDocName << ", " << anAttrObjects;

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Translation::CREATION_METHOD())->value();

  if (aCreationMethod == FeaturesPlugin_Translation::CREATION_METHOD_BY_DISTANCE()) {
    AttributeSelectionPtr anAttrAxis =
      aBase->selection(FeaturesPlugin_Translation::AXIS_OBJECT_ID());
    AttributeDoublePtr anAttrDistance =
      aBase->real(FeaturesPlugin_Translation::DISTANCE_ID());
    theDumper << ", " << anAttrAxis << ", " << anAttrDistance;
  } else if (aCreationMethod == FeaturesPlugin_Translation::CREATION_METHOD_BY_DIMENSIONS()) {
    AttributeDoublePtr anAttrDx = aBase->real(FeaturesPlugin_Translation::DX_ID());
    AttributeDoublePtr anAttrDy = aBase->real(FeaturesPlugin_Translation::DY_ID());
    AttributeDoublePtr anAttrDz = aBase->real(FeaturesPlugin_Translation::DZ_ID());
    theDumper << ", " << anAttrDx << ", " << anAttrDy << ", " << anAttrDz;
  } else if (aCreationMethod == FeaturesPlugin_Translation::CREATION_METHOD_BY_TWO_POINTS()) {
    AttributeSelectionPtr anAttrStartPoint =
      aBase->selection(FeaturesPlugin_Translation::START_POINT_ID());
    AttributeSelectionPtr anAttrEndPoint =
      aBase->selection(FeaturesPlugin_Translation::END_POINT_ID());
    theDumper << ", " << anAttrStartPoint << ", " << anAttrEndPoint;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
TranslationPtr addTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const std::list<ModelHighAPI_Selection>& theMainObjects,
                              const ModelHighAPI_Selection& theAxisObject,
                              const ModelHighAPI_Double& theDistance)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Translation::ID());
  return TranslationPtr(new FeaturesAPI_Translation(aFeature, theMainObjects,
                                                    theAxisObject, theDistance));
}

//==================================================================================================
TranslationPtr addTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const std::list<ModelHighAPI_Selection>& theMainObjects,
                              const ModelHighAPI_Double& theDx,
                              const ModelHighAPI_Double& theDy,
                              const ModelHighAPI_Double& theDz)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Translation::ID());
  return TranslationPtr(new FeaturesAPI_Translation(aFeature, theMainObjects, theDx, theDy, theDz));
}

//==================================================================================================
TranslationPtr addTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const std::list<ModelHighAPI_Selection>& theMainObjects,
                              const ModelHighAPI_Selection& theStartPoint,
                              const ModelHighAPI_Selection& theEndPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Translation::ID());
  return TranslationPtr(new FeaturesAPI_Translation(aFeature, theMainObjects,
                                                    theStartPoint, theEndPoint));
}
