// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ModuleBase_IWidgetCreator_H
#define ModuleBase_IWidgetCreator_H

#include "ModuleBase.h"

#include <set>
#include <string>
#include <memory>

class ModuleBase_ModelWidget;
class ModuleBase_PageBase;
class ModuleBase_IWorkshop;

class Config_WidgetAPI;

class QWidget;

/** 
* \ingroup GUI
* Interface to WidgetCreator which can create specific widgets by type
*/
class MODULEBASE_EXPORT ModuleBase_IWidgetCreator
{
public:
  /// Default constructor
  ModuleBase_IWidgetCreator();

  /// Virtual destructor
  ~ModuleBase_IWidgetCreator();

  /// Returns a container of possible page types, which this creator can process
  /// \returns list of type names
  virtual const std::set<std::string>& panelTypes();

  /// Returns a container of possible page types, which this creator can process
  /// \returns list of type names
  virtual const std::set<std::string>& pageTypes();

  /// Returns a container of possible widget types, which this creator can process
  /// \returns list of type names
  virtual const std::set<std::string>& widgetTypes();

  /// Create panel control by its type.
  /// \param theType a panel type
  /// \param theParent a parent widget
  /// \return created widget or null
  virtual QWidget* createPanelByType(const std::string& theType,
                                     QWidget* theParent) {};

  /// Create page by its type
  /// \param theType a type
  /// \param theParent a parent widget
  /// \param theData a low-level API for reading xml definitions of widgets
  virtual ModuleBase_PageBase* createPageByType(const std::string& theType,
                                                QWidget* theParent,
                                                Config_WidgetAPI* theWidgetApi) {};

  /// Create widget by its type
  /// \param theType a type
  /// \param theParent a parent widget
  /// \param theData a low-level API for reading xml definitions of widgets
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                                     QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi,
                                                     ModuleBase_IWorkshop* theWorkshop) {};
};

typedef std::shared_ptr<ModuleBase_IWidgetCreator> WidgetCreatorPtr;


#endif