// Copyright (C) 2014-2016 CEA/DEN, EDF R&D -->

// File:        PrimitivesAPI_Box.cpp
// Created:     28 June 2016
// Author:      Clarisse Genrault

#include "PrimitivesAPI_Box.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
PrimitivesAPI_Box::PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
PrimitivesAPI_Box::PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Double & theDx,
                                     const ModelHighAPI_Double & theDy,
                                     const ModelHighAPI_Double & theDz)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setDimensions(theDx, theDy, theDz);
}

//==================================================================================================
PrimitivesAPI_Box::PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Selection& theFirstPoint,
                                     const ModelHighAPI_Selection& theSecondPoint)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setPoints(theFirstPoint, theSecondPoint);
}

//==================================================================================================
PrimitivesAPI_Box::~PrimitivesAPI_Box()
{

}

//==================================================================================================
void PrimitivesAPI_Box::setDimensions(const ModelHighAPI_Double& theDx,
                                      const ModelHighAPI_Double& theDy,
                                      const ModelHighAPI_Double& theDz)
{
  fillAttribute(PrimitivesPlugin_Box::CREATION_METHOD_BY_DIMENSIONS(), creationMethod());
  fillAttribute(theDx, dx());
  fillAttribute(theDy, dy());
  fillAttribute(theDz, dz());

  execute();
}

//==================================================================================================
void PrimitivesAPI_Box::setPoints(const ModelHighAPI_Selection& theFirstPoint,
                                  const ModelHighAPI_Selection& theSecondPoint)
{
  fillAttribute(PrimitivesPlugin_Box::CREATION_METHOD_BY_TWO_POINTS(), creationMethod());
  fillAttribute(theFirstPoint, firstPoint());
  fillAttribute(theSecondPoint, secondPoint());

  execute();
}

//==================================================================================================
void PrimitivesAPI_Box::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addBox(" << aDocName;

  std::string aCreationMethod = aBase->string(PrimitivesPlugin_Box::CREATION_METHOD())->value();
  
  if(aCreationMethod == PrimitivesPlugin_Box::CREATION_METHOD_BY_DIMENSIONS()) {
    AttributeDoublePtr anAttrDx = aBase->real(PrimitivesPlugin_Box::DX_ID());
    AttributeDoublePtr anAttrDy = aBase->real(PrimitivesPlugin_Box::DY_ID());
    AttributeDoublePtr anAttrDz = aBase->real(PrimitivesPlugin_Box::DZ_ID());
    
    theDumper << ", " << anAttrDx << ", " << anAttrDy << ", " << anAttrDz;
  } else if (aCreationMethod == PrimitivesPlugin_Box::CREATION_METHOD_BY_TWO_POINTS()) {
    AttributeSelectionPtr anAttrFirstPnt =
      aBase->selection(PrimitivesPlugin_Box::POINT_FIRST_ID());
    AttributeSelectionPtr anAttrSecondPnt =
      aBase->selection(PrimitivesPlugin_Box::POINT_SECOND_ID());

    theDumper << ", " << anAttrFirstPnt << ", " << anAttrSecondPnt;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
BoxPtr addBox(const std::shared_ptr<ModelAPI_Document>& thePart,
              const ModelHighAPI_Double& theDx,
              const ModelHighAPI_Double& theDy,
              const ModelHighAPI_Double& theDz)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Box::ID());
  return BoxPtr(new PrimitivesAPI_Box(aFeature, theDx, theDy, theDz));
}

//==================================================================================================
BoxPtr addBox(const std::shared_ptr<ModelAPI_Document>& thePart,
              const ModelHighAPI_Selection& theFirstPoint,
              const ModelHighAPI_Selection& theSecondPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Box::ID());
  return BoxPtr(new PrimitivesAPI_Box(aFeature, theFirstPoint, theSecondPoint));
}