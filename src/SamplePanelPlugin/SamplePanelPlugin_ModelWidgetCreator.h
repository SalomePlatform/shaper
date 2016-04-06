// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SamplePanelPlugin_ModelWidgetCreator.h
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef SamplePanelPlugin_ModelWidgetCreator_H
#define SamplePanelPlugin_ModelWidgetCreator_H

#include "SamplePanelPlugin.h"

#include <ModuleBase_IWidgetCreator.h>

#include <string>
#include <set>

class ModuleBase_IWorkshop;
class Config_WidgetAPI;
class QWidget;

/** 
* \ingroup GUI
* Interface to WidgetCreator which can create specific widgets by type
*/
class SamplePanelPlugin_ModelWidgetCreator : public ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  SamplePanelPlugin_ModelWidgetCreator();

  /// Virtual destructor
  ~SamplePanelPlugin_ModelWidgetCreator() {}

  /// Returns a container of possible widget types, which this creator can process
  /// \param a list of type names
  virtual void widgetTypes(std::set<std::string>& theTypes);

  /// Create widget by its type
  /// The default implementation is empty
  /// \param theType a type
  /// \param theParent a parent widget
  /// \param theData a low-level API for reading xml definitions of widgets
  /// \param theWorkshop a current workshop
  /// \return a created model widget or null
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                                     QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi,
                                                     ModuleBase_IWorkshop* /*theWorkshop*/);
private:
  std::set<std::string> myModelWidgetTypes; /// types of widgets
};

#endif
