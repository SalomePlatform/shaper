// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Shell.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "BuildAPI_Shell.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Shell::BuildAPI_Shell(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Shell::BuildAPI_Shell(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                               const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Shell::~BuildAPI_Shell()
{

}

//==================================================================================================
void BuildAPI_Shell::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

// TODO(spo): make add* as static functions of the class
//==================================================================================================
ShellPtr addShell(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Shell::ID());
  return ShellPtr(new BuildAPI_Shell(aFeature, theBaseObjects));
}
