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
class Config_WidgetAPI;
class ModuleBase_ModelWidget;
class ModuleBase_Operation;
class ModuleBase_IWorkshop;

/**
 * Interface to a module
 */
class MODULEBASE_EXPORT ModuleBase_IModule : public QObject
{
  Q_OBJECT
 public:

   ModuleBase_IModule(ModuleBase_IWorkshop* theParent);

  virtual ~ModuleBase_IModule() {}

  /// Reads description of features from XML file 
  virtual void createFeatures();

  /// Called on creation of menu item in desktop
  virtual void actionCreated(QAction*);

  /// Called when user selects feature for editing
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

  /// Called when it is necessary to update a command state (enable or disable it)
  //virtual bool isFeatureEnabled(const QString& theCmdId) const = 0;

  /// Creates custom widgets for property panel
  virtual QWidget* createWidgetByType(const std::string& theType, QWidget* theParent,
                                      Config_WidgetAPI* theWidgetApi, std::string theParentId,
                                      QList<ModuleBase_ModelWidget*>& theModelWidgets)
  {
    return 0;
  }

  ModuleBase_IWorkshop* workshop() const { return myWorkshop; }

  /// Call back forlast tuning of property panel before operation performance
  /// It is called as on clearing of property panel as on filling with new widgets
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation) {}

public slots:
  void onFeatureTriggered();

protected slots:
  /// Called on selection changed event
  virtual void onSelectionChanged() {}

  /// SLOT, that is called by mouse press in the viewer.
  /// The mouse released point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  //virtual void onMousePressed(QMouseEvent* theEvent) {}

  /// SLOT, that is called by mouse release in the viewer.
  /// The mouse released point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  //virtual void onMouseReleased(QMouseEvent* theEvent) {}
  
  /// SLOT, that is called by mouse move in the viewer.
  /// The mouse moved point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  //virtual void onMouseMoved(QMouseEvent* theEvent) {}

  /// SLOT, that is called by the mouse double click in the viewer.
  /// \param theEvent the mouse event
  //virtual void onMouseDoubleClick(QMouseEvent* theEvent) {}

  /// SLOT, that is called by the key in the viewer is clicked.
  /// \param theEvent the mouse event
  //virtual void onKeyRelease(QKeyEvent* theEvent) {}

 protected:
  /// Sends the operation for launching
  /// \param theOperation the operation
  void sendOperation(ModuleBase_Operation* theOperation);

  /// Creates a new operation
  /// \param theCmdId the operation name
  /// \param theFeatureKind a kind of feature to get the feature xml description
  virtual ModuleBase_Operation* createOperation(const std::string& theFeatureId);

  /// Register validators for this module
  virtual void registerValidators() {}

  /// Register selection filters for this module
  virtual void registerFilters() {}

  /// Returns new instance of operation object (used in createOperation for customization)
  virtual ModuleBase_Operation* getNewOperation(const std::string& theFeatureId);

protected:

  ModuleBase_IWorkshop* myWorkshop;
  std::map<std::string, std::string> myFeaturesInFiles;
};



//! This function must return a new module instance.
extern "C" {
typedef ModuleBase_IModule* (*CREATE_FUNC)(ModuleBase_IWorkshop*);
}

#define CREATE_MODULE "createModule"

#endif //ModuleBase_IModule
