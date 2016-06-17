// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Vertex.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "BuildAPI_Vertex.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Vertex::BuildAPI_Vertex(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Vertex::BuildAPI_Vertex(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Vertex::~BuildAPI_Vertex()
{

}

//==================================================================================================
void BuildAPI_Vertex::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
VertexPtr addVertex(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Vertex::ID());
  return VertexPtr(new BuildAPI_Vertex(aFeature, theBaseObjects));
}
