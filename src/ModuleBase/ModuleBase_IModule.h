// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ModuleBase_IModule_H
#define ModuleBase_IModule_H

#include "ModuleBase.h"
#include "ModuleBase_IWorkshop.h"

#include <ModelAPI_Feature.h>

#include <QString>
#include <QObject>

#include <string>
#include <map>

class QAction;
class QMouseEvent;
class QKeyEvent;
class QMenu;
class Config_WidgetAPI;
class ModuleBase_ModelWidget;
class ModuleBase_Operation;
class ModuleBase_IWorkshop;

/**
 * \ingroup GUI
 * Interface to a module
 */
class MODULEBASE_EXPORT ModuleBase_IModule : public QObject
{
  Q_OBJECT
 public:

   /// Constructor
   /// \param theParent instance of workshop intrface
   ModuleBase_IModule(ModuleBase_IWorkshop* theParent);

  virtual ~ModuleBase_IModule() {}

  /// Reads description of features from XML file 
  virtual void createFeatures();

  /// Called on creation of menu item in desktop
  virtual void actionCreated(QAction*);

  /// Launching of a edit operation on the feature 
  /// \param theFeature feature for editing
  virtual void editFeature(FeaturePtr theFeature);

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  virtual void launchOperation(const QString& theCmdId);

  /// Realizes some functionality by an operation start
  /// \param theOperation a started operation
  virtual void operationStarted(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation resume
  /// \param theOperation a resumed operation
  virtual void operationResumed(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation stop
  virtual void operationStopped(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation commit
  virtual void operationCommitted(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation abort
  virtual void operationAborted(ModuleBase_Operation* theOperation) {}

  /// Add menu atems for viewer into the given menu
  /// \param theMenu a popup menu to be shown in the viewer
  virtual void addViewerItems(QMenu* theMenu) const {}

  /// Add menu atems for object browser into the given menu
  /// \param theMenu a popup menu to be shown in the object browser
  virtual void addObjectBrowserItems(QMenu* theMenu) const {};

  /// Called when it is necessary to update a command state (enable or disable it)
  //virtual bool isFeatureEnabled(const QString& theCmdId) const = 0;

  /// Creates custom widgets for property panel
  /// \param theType a type of widget
  /// \param theParent the parent object
  /// \param theWidgetApi the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  /// \param theModelWidgets list of widget objects
  virtual QWidget* createWidgetByType(const std::string& theType, QWidget* theParent,
                                      Config_WidgetAPI* theWidgetApi, std::string theParentId,
                                      QList<ModuleBase_ModelWidget*>& theModelWidgets)
  {
    return 0;
  }

  /// Returns current workshop
  ModuleBase_IWorkshop* workshop() const { return myWorkshop; }

  /// Call back forlast tuning of property panel before operation performance
  /// It is called as on clearing of property panel as on filling with new widgets
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation) {}

  /// Returns whether the object can be displayed at the bounds of the active operation.
  /// Display only current operation results
  /// \param theObject a model object
  virtual bool canDisplayObject(const ObjectPtr& theObject) const;

public slots:
  /// Called on call of command corresponded to a feature
  void onFeatureTriggered();

protected slots:
  /// Called on selection changed event
  virtual void onSelectionChanged() {}

 protected:
  /// Sends the operation for launching
  /// \param theOperation the operation
  void sendOperation(ModuleBase_Operation* theOperation);

  /// Creates a new operation
  /// \param theCmdId the operation name
  virtual ModuleBase_Operation* createOperation(const std::string& theCmdId);

  /// Register validators for this module
  virtual void registerValidators() {}

  /// Register selection filters for this module
  virtual void registerFilters() {}

  /// Register properties of this module
  virtual void registerProperties() {}

  /// Returns new instance of operation object (used in createOperation for customization)
  virtual ModuleBase_Operation* getNewOperation(const std::string& theFeatureId);

protected:

  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;

  /// Map of features in XML
  std::map<std::string, std::string> myFeaturesInFiles;
};



//! This function must return a new module instance.
extern "C" {
typedef ModuleBase_IModule* (*CREATE_FUNC)(ModuleBase_IWorkshop*);
}

#define CREATE_MODULE "createModule"

#endif //ModuleBase_IModule
