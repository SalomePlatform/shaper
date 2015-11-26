// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_Plugin.hxx
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#ifndef SKETCHSHAPEPLUGIN_PLUGIN_H_
#define SKETCHSHAPEPLUGIN_PLUGIN_H_

#include <SketchShapePlugin.h>
#include <ModelAPI_Plugin.h>
//#include <ModuleBase_IWidgetCreator.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Events.h>

class ModuleBase_ModelWidget;
class QWidget;

/**\class SketchShapePlugin_Plugin
 * \ingroup Plugins
 * \brief Interface common for any plugin: allows to use plugin by the plugins manager.
 */
//, public ModuleBase_IWidgetCreator
class SketchShapePlugin_Plugin : public ModelAPI_Plugin
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

  /// Create widget by its type
  /// \param theType a type
  /// \param theParent a parent widget
  //virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
  //                                                   QWidget* theParent = NULL);
 public:
   SketchShapePlugin_Plugin();
};

#endif
