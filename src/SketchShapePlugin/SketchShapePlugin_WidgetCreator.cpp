// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "SketchShapePlugin_WidgetCreator.h"

#include "SketchShapePlugin_PageGroupBox.h"

#include "SketchShapePlugin_WidgetMultiSelector.h"
#include "SketchShapePlugin_WidgetCheckGroupBox.h"

SketchShapePlugin_WidgetCreator::SketchShapePlugin_WidgetCreator()
: ModuleBase_IWidgetCreator()
{
  myPages.insert("sketchshape_groupbox");
  myPages.insert("sketchshape_check_groupbox");

  myWidgets.insert("sketchshape_multi_selector");
}

const std::set<std::string>& SketchShapePlugin_WidgetCreator::pageTypes()
{
  return myPages;
}

const std::set<std::string>& SketchShapePlugin_WidgetCreator::widgetTypes()
{
  return myWidgets;
}

ModuleBase_PageBase* SketchShapePlugin_WidgetCreator::createPageByType(
                                   const std::string& theType, QWidget* theParent,
                                   Config_WidgetAPI* theWidgetApi, std::string theParentId)
{
  ModuleBase_PageBase* aPage = 0;
  if (myPages.find(theType) == myPages.end())
    return aPage;

  if (theType == "sketchshape_groupbox") {
    aPage = new SketchShapePlugin_PageGroupBox(theParent);
  }
  else if (theType == "sketchshape_check_groupbox") {
    aPage = new SketchShapePlugin_WidgetCheckGroupBox(theParent, theWidgetApi, theParentId);
  }

  return aPage;
}

ModuleBase_ModelWidget* SketchShapePlugin_WidgetCreator::createWidgetByType(
                                   const std::string& theType, QWidget* theParent,
                                   Config_WidgetAPI* theWidgetApi, std::string theParentId,
                                   ModuleBase_IWorkshop* theWorkshop)
{
  ModuleBase_ModelWidget* aWidget = 0;
  if (myWidgets.find(theType) == myWidgets.end())
    return aWidget;

  if (theType == "sketchshape_multi_selector") {
    aWidget = new SketchShapePlugin_WidgetMultiSelector(theParent, theWorkshop, theWidgetApi,
                                                        theParentId);
  }

  return aWidget;
}
