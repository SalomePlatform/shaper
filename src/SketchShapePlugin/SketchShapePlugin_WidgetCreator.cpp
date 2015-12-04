// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "SketchShapePlugin_WidgetCreator.h"
#include "SketchShapePlugin_PageGroupBox.h"

SketchShapePlugin_WidgetCreator::SketchShapePlugin_WidgetCreator()
: ModuleBase_IWidgetCreator()
{
  myTypes.insert("sketchshape_groupbox");
}

const std::set<std::string>& SketchShapePlugin_WidgetCreator::widgetTypes()
{
  return myTypes;
}

ModuleBase_ModelWidget* SketchShapePlugin_WidgetCreator::createWidgetByType(
                                   const std::string& theType, QWidget* theParent)
{
  ModuleBase_ModelWidget* aWidget = 0;
  if (myTypes.find(theType) == myTypes.end())
    return aWidget;

  //if (theType == "sketchshape_groupbox") {
  //  aWidget = new SketchShapePlugin_PageGroupBox(theParent);
  //}

  return aWidget;
}
