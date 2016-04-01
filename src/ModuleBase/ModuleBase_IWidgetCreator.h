// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ModuleBase_IWidgetCreator_H
#define ModuleBase_IWidgetCreator_H

#include "ModuleBase.h"

#include "ModelAPI_Feature.h"

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
  /// The default implementation is empty
  /// \param theTypes a list of type names
  virtual void panelTypes(std::set<std::string>& theTypes) {}

  /// Returns a container of possible page types, which this creator can process
  /// The default implementation is empty
  /// \param a list of type names
  virtual void pageTypes(std::set<std::string>& theTypes) {}

  /// Returns a container of possible widget types, which this creator can process
  /// \param a list of type names
  virtual void widgetTypes(std::set<std::string>& theTypes) {}

  /// Create panel control by its type.
  /// The default implementation is empty
  /// \param theType a panel type
  /// \param theParent a parent widget
  /// \param theFeature a feature modified in the panel
  /// \return created widget or null
  virtual QWidget* createPanelByType(const std::string& theType,
                                     QWidget* theParent,
                                     const FeaturePtr& theFeature);

  /// Create page by its type
  /// The default implementation is empty
  /// \param theType a type
  /// \param theParent a parent widget
  /// \param theData a low-level API for reading xml definitions of widgets
  virtual ModuleBase_PageBase* createPageByType(const std::string& theType,
                                                QWidget* theParent,
                                                Config_WidgetAPI* theWidgetApi);

  /// Create widget by its type
  /// The default implementation is empty
  /// \param theType a type
  /// \param theParent a parent widget
  /// \param theData a low-level API for reading xml definitions of widgets
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                                     QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi,
                                                     ModuleBase_IWorkshop* theWorkshop);
};

typedef std::shared_ptr<ModuleBase_IWidgetCreator> WidgetCreatorPtr;


#endif