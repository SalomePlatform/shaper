// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "SketchShapePlugin_WidgetCreator.h"
#include "SketchShapePlugin_PageGroupBox.h"

SketchShapePlugin_WidgetCreator::SketchShapePlugin_WidgetCreator()
: ModuleBase_IWidgetCreator()
{
  myPages.insert("sketchshape_groupbox");
}

const std::set<std::string>& SketchShapePlugin_WidgetCreator::pageTypes()
{
  return myPages;
}

const std::set<std::string>& SketchShapePlugin_WidgetCreator::widgetTypes()
{
  return myTypes;
}

ModuleBase_PageBase* SketchShapePlugin_WidgetCreator::createPageByType(
                                   const std::string& theType, QWidget* theParent)
{
  ModuleBase_PageBase* aPage = 0;
  if (myPages.find(theType) == myPages.end())
    return aPage;

  if (theType == "sketchshape_groupbox") {
    aPage = new SketchShapePlugin_PageGroupBox(theParent);
  }

  return aPage;
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
