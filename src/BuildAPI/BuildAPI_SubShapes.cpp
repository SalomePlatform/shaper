// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_SubShapes.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "BuildAPI_SubShapes.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_SubShapes::BuildAPI_SubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_SubShapes::BuildAPI_SubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theBaseShape,
                                       const std::list<ModelHighAPI_Selection>& theSubShapes)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseShape, mybaseShape);
    setSubShapes(theSubShapes);
  }
}

//==================================================================================================
BuildAPI_SubShapes::~BuildAPI_SubShapes()
{

}

//==================================================================================================
void BuildAPI_SubShapes::setBaseShape(const ModelHighAPI_Selection& theBaseShape)
{
  fillAttribute(theBaseShape, mybaseShape);

  execute();
}

//==================================================================================================
void BuildAPI_SubShapes::setSubShapes(const std::list<ModelHighAPI_Selection>& theSubShapes)
{
  fillAttribute(theSubShapes, mysubShapes);

  execute();
}

// TODO(spo): make add* as static functions of the class
//==================================================================================================
SubShapesPtr addSubShapes(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Selection& theBaseShape,
                          const std::list<ModelHighAPI_Selection>& theSubShapes)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_SubShapes::ID());
  return SubShapesPtr(new BuildAPI_SubShapes(aFeature, theBaseShape, theSubShapes));
}
