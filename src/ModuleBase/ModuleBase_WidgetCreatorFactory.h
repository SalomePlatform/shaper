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
class ModuleBase_PageBase;
class ModuleBase_IWorkshop;

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

  /// Returns true if there is a creator, which can make a panel by the type
  /// \param theType a type
  /// \return a boolean value
  bool hasPanelWidget(const std::string& theType);

  /// Create panel by its type
  /// \param theType a type
  /// \param theParent a parent widget
  /// \return a created panel or null
  QWidget* createPanel(const std::string& theType, QWidget* theParent);

  /// Returns true if there is a creator, which can make a page by the type
  /// \param theType a type
  /// \return a boolean value
  bool hasPageWidget(const std::string& theType);

  /// Create page by its type
  /// \param theType a type
  /// \param theParent a parent widget
  /// \return a created page or null
  ModuleBase_PageBase* createPageByType(const std::string& theType,
                                        QWidget* theParent,
                                        Config_WidgetAPI* theWidgetApi);

  /// Create widget by its type
  /// \param theType a type
  /// \param theParent a parent widget
  /// \return a created widget or null
  ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                             QWidget* theParent,
                                             Config_WidgetAPI* theWidgetApi,
                                             ModuleBase_IWorkshop* theWorkshop);

private:
  /// Constructor is hidden
  ModuleBase_WidgetCreatorFactory();

  /// Map of widget panel in XML to creator
  QMap<std::string, WidgetCreatorPtr> myPanelToCreator;

  /// Map of widget page in XML to creator
  QMap<std::string, WidgetCreatorPtr> myPageToCreator;

  /// Map of widget type in XML to creator
  QMap<std::string, WidgetCreatorPtr> myCreators;
};

typedef std::shared_ptr<ModuleBase_WidgetCreatorFactory> WidgetCreatorFactoryPtr;

#endif /* MODULEBASE_WIDGETCREATORFACTORY_H_ */
