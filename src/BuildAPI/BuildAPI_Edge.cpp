// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Edge.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "BuildAPI_Edge.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Edge::BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Edge::BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Edge::~BuildAPI_Edge()
{

}

//==================================================================================================
void BuildAPI_Edge::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
EdgePtr addEdge(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Edge::ID());
  return EdgePtr(new BuildAPI_Edge(aFeature, theBaseObjects));
}
