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

#include "FeaturesAPI_Scale.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Scale::FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Scale::FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theMainObjects,
                                     const ModelHighAPI_Selection& theCenterPoint,
                                     const ModelHighAPI_Double& theScaleFactor)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    fillAttribute(theCenterPoint, centerPoint());
    setScaleFactor(theScaleFactor);
  }
}

//==================================================================================================
FeaturesAPI_Scale::FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theMainObjects,
                                     const ModelHighAPI_Selection& theCenterPoint,
                                     const ModelHighAPI_Double& theScaleFactorX,
                                     const ModelHighAPI_Double& theScaleFactorY,
                                     const ModelHighAPI_Double& theScaleFactorZ)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theMainObjects, mainObjects());
    fillAttribute(theCenterPoint, centerPoint());
    setDimensions(theScaleFactorX, theScaleFactorY, theScaleFactorZ);
  }
}

//==================================================================================================
FeaturesAPI_Scale::~FeaturesAPI_Scale()
{
}

//==================================================================================================
void FeaturesAPI_Scale::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mainObjects());
  execute();
}

//==================================================================================================
void FeaturesAPI_Scale::setCenterPoint(const ModelHighAPI_Selection& theCenterPoint)
{
  fillAttribute(theCenterPoint, centerPoint());
  execute();
}

//==================================================================================================
void FeaturesAPI_Scale::setScaleFactor(const ModelHighAPI_Double& theScaleFactor)
{
  fillAttribute(FeaturesPlugin_Scale::CREATION_METHOD_BY_FACTOR(), creationMethod());
  fillAttribute(theScaleFactor, scaleFactor());
  execute();
}

//==================================================================================================
void FeaturesAPI_Scale::setDimensions(const ModelHighAPI_Double& theScaleFactorX,
                                      const ModelHighAPI_Double& theScaleFactorY,
                                      const ModelHighAPI_Double& theScaleFactorZ)
{
  fillAttribute(FeaturesPlugin_Scale::CREATION_METHOD_BY_DIMENSIONS(), creationMethod());
  fillAttribute(theScaleFactorX, scaleFactorX());
  fillAttribute(theScaleFactorY, scaleFactorY());
  fillAttribute(theScaleFactorZ, scaleFactorZ());
  execute();
}

//==================================================================================================
void FeaturesAPI_Scale::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Scale::OBJECTS_LIST_ID());
  theDumper << aBase << " = model.addScale(" << aDocName << ", " << anAttrObjects;

  AttributeSelectionPtr anAttrPoint =
    aBase->selection(FeaturesPlugin_Scale::CENTER_POINT_ID());
  theDumper << " , " << anAttrPoint;

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Scale::CREATION_METHOD())->value();

  if (aCreationMethod == FeaturesPlugin_Scale::CREATION_METHOD_BY_FACTOR()) {
    AttributeDoublePtr anAttrScaleFactor =
      aBase->real(FeaturesPlugin_Scale::SCALE_FACTOR_ID());
    theDumper << ", " << anAttrScaleFactor;
  } else if (aCreationMethod == FeaturesPlugin_Scale::CREATION_METHOD_BY_DIMENSIONS()) {
    AttributeDoublePtr anAttrScaleFactorX =
      aBase->real(FeaturesPlugin_Scale::SCALE_FACTOR_X_ID());
    AttributeDoublePtr anAttrScaleFactorY =
      aBase->real(FeaturesPlugin_Scale::SCALE_FACTOR_Y_ID());
    AttributeDoublePtr anAttrScaleFactorZ =
      aBase->real(FeaturesPlugin_Scale::SCALE_FACTOR_Z_ID());
    theDumper << ", " << anAttrScaleFactorX << " , " << anAttrScaleFactorY;
    theDumper << ", " << anAttrScaleFactorZ;
  }

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
ScalePtr addScale(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                  const ModelHighAPI_Selection& theCenterPoint,
                  const ModelHighAPI_Double& theScaleFactorX,
                  const ModelHighAPI_Double& theScaleFactorY,
                  const ModelHighAPI_Double& theScaleFactorZ,
                  const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Scale::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");
  ScalePtr aScale;
  if (theScaleFactorY.value() == 0 && theScaleFactorZ.value() == 0)
    aScale.reset(new FeaturesAPI_Scale(aFeature, theMainObjects, theCenterPoint, theScaleFactorX));
  else
    aScale.reset(new FeaturesAPI_Scale(aFeature, theMainObjects, theCenterPoint,
                 theScaleFactorX, theScaleFactorY, theScaleFactorZ));
  return aScale;
}