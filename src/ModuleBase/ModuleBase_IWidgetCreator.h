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
  /// \returns types
  virtual const std::set<std::string>& pageTypes() = 0;

  /// Returns a container of possible widget types, which this creator can process
  /// \returns types
  virtual const std::set<std::string>& widgetTypes() = 0;

  /// Create page by its type
  /// \param theType a type
  /// \param theParent a parent widget
  virtual ModuleBase_PageBase* createPageByType(const std::string& theType,
                                                QWidget* theParent,
                                                Config_WidgetAPI* theWidgetApi,
                                                std::string theParentId) = 0;

  /// Create widget by its type
   /// \param theType a type
   /// \param theParent a parent widget
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                                     QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi,
                                                     std::string theParentId,
                                                     ModuleBase_IWorkshop* theWorkshop) = 0;
};

typedef std::shared_ptr<ModuleBase_IWidgetCreator> WidgetCreatorPtr;


#endif