// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ParametersPlugin_WidgetCreator.h
// Created:     11 Apr 2016
// Author:      Vitaly SMETANNIKOV

#ifndef ParametersPlugin_WidgetCreator_H
#define ParametersPlugin_WidgetCreator_H

#include "ParametersPlugin.h"

#include <ModuleBase_IWidgetCreator.h>

#include <string>
#include <set>

/** 
* \ingroup GUI
* Interface to WidgetCreator which can create specific widgets by type
*/
class ParametersPlugin_WidgetCreator : public ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  ParametersPlugin_WidgetCreator();

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
};

#endif
