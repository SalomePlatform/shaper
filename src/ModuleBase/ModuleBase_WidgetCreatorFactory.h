// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetCreatorFactory.cpp
// Created:     03 Dec 2015
// Author:      Natalia ERMOLAEVA

#ifndef MODULEBASE_WIDGETCREATORFACTORY_H_
#define MODULEBASE_WIDGETCREATORFACTORY_H_

#include <ModuleBase.h>

#include <memory>
#include <string>

#include <QMap>

#include <ModuleBase_IWidgetCreator.h>

class ModuleBase_ModelWidget;

class QWidget;

/**
* \ingroup GUI
* A class for creation of widgets instances in for property panel using XML deskription of 
* a feature
*/
class MODULEBASE_EXPORT ModuleBase_WidgetCreatorFactory
{
 public:
  // Returns an singular instance of the class if it exists or create it
  static std::shared_ptr<ModuleBase_WidgetCreatorFactory> get();

  /// Destructor
  virtual ~ModuleBase_WidgetCreatorFactory();

  /// The creator is added to the internal container to be used when the createWidgetByType is called
  /// \param theCreator a new widget creator
  void registerCreator(const WidgetCreatorPtr& theCreator);

  /// Create widget by its type
  /// \param theType a type
  /// \param theParent a parent widget
  ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                             QWidget* theParent = NULL);

private:
  /// Constructor is hidden
  ModuleBase_WidgetCreatorFactory();

  /// List of created model widgets
  QMap<std::string, WidgetCreatorPtr> myModelWidgets;
};

typedef std::shared_ptr<ModuleBase_WidgetCreatorFactory> WidgetCreatorFactoryPtr;

#endif /* MODULEBASE_WIDGETCREATORFACTORY_H_ */
