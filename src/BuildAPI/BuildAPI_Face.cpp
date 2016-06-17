// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Face.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "BuildAPI_Face.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Face::BuildAPI_Face(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Face::BuildAPI_Face(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Face::~BuildAPI_Face()
{

}

//==================================================================================================
void BuildAPI_Face::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
FacePtr addFace(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Face::ID());
  return FacePtr(new BuildAPI_Face(aFeature, theBaseObjects));
}
