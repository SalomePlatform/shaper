// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        CollectionPlugin_WidgetCreator.h
// Created:     15 Nov 2016
// Author:      Vitaly SMETANNIKOV


#ifndef CollectionPlugin_WidgetCreator_H
#define CollectionPlugin_WidgetCreator_H


#include "CollectionPlugin.h"
#include <ModuleBase_IWidgetCreator.h>

#include <string>
#include <set>

class QWidget;

/** 
* \ingroup GUI
* Interface to WidgetCreator which can create specific widgets by type
*/
class CollectionPlugin_WidgetCreator : public ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  CollectionPlugin_WidgetCreator();

  /// Virtual destructor
  ~CollectionPlugin_WidgetCreator() {}

  /// Returns a container of possible page types, which this creator can process
  /// \param theTypes a list of type names
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
  std::set<std::string> myPanelTypes; ///< types of panels
};

#endif