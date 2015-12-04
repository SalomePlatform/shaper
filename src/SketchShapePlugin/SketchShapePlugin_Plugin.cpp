// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_Plugin.cpp
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#include <SketchShapePlugin_Plugin.h>
#include <SketchShapePlugin_Validators.h>
#include <SketchShapePlugin_Feature.h>
/*
#include <SketchShapePlugin_PageGroupBox.h>

#include <Events_Loop.h>
*/
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_WidgetCreatorFactory.h>
#include <SketchShapePlugin_WidgetCreator.h>
/*#include <ModelAPI_Data.h>

#include <ModuleBase_ModelWidget.h>
#include <qwidget.h>

#include <memory>*/

// the only created instance of this plugin
static SketchShapePlugin_Plugin* MY_SKETCH_SHAPE_INSTANCE = new SketchShapePlugin_Plugin();

SketchShapePlugin_Plugin::SketchShapePlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("SketchShapePlugin_FeatureValidator",
                              new SketchShapePlugin_FeatureValidator);

  WidgetCreatorFactoryPtr aWidgetCreatorFactory = ModuleBase_WidgetCreatorFactory::get();
  aWidgetCreatorFactory->registerCreator(
          std::shared_ptr<SketchShapePlugin_WidgetCreator>(new SketchShapePlugin_WidgetCreator()));

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr SketchShapePlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == SketchShapePlugin_Feature::ID()) {
    return FeaturePtr(new SketchShapePlugin_Feature);
  }
  // feature of such kind is not found
  return FeaturePtr();
}

/*ModuleBase_ModelWidget* SketchShapePlugin_Plugin::createWidgetByType(const std::string& theType,
                                                                     QWidget* theParent)
{
  ModuleBase_ModelWidget* aWidget = 0;
  if (theType == "sketchshape_groupbox")
    aWidget = new SketchShapePlugin_PageGroupBox(theParent);
  return aWidget;
}*/

