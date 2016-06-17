// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Wire.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "BuildAPI_Wire.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Wire::BuildAPI_Wire(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Wire::BuildAPI_Wire(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Wire::~BuildAPI_Wire()
{

}

//==================================================================================================
void BuildAPI_Wire::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void BuildAPI_Wire::addContour()
{
  feature()->customAction(BuildPlugin_Wire::ADD_CONTOUR_ACTION_ID());
}

//==================================================================================================
WirePtr addWire(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Wire::ID());
  return WirePtr(new BuildAPI_Wire(aFeature, theBaseObjects));
}
