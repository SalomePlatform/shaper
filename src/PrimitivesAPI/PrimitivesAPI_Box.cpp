// Copyright (C) 2014-2016 CEA/DEN, EDF R&D -->

// File:        PrimitivesAPI_Box.cpp
// Created:     28 June 2016
// Author:      Clarisse Genrault

#include "PrimitivesAPI_Box.h"

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