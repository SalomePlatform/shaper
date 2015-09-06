 // Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ModuleBase_IModule_H
#define ModuleBase_IModule_H

#include "ModuleBase.h"
#include "ModuleBase_IWorkshop.h"

#include <ModelAPI_Feature.h>

#include <QString>
#include <QObject>
#include <QMap>

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
class ModuleBase_IDocumentDataModel;

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

  // Add default selection filters of the module to the current viewer
  virtual void activateSelectionFilters() {};

  // Remove default selection filters of the module from the current viewer
  virtual void deactivateSelectionFilters() {};

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
  virtual void onOperationStarted(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation resume
  /// By default it emits operationResumed signal
  /// \param theOperation a resumed operation
  virtual void onOperationResumed(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation stop
  virtual void onOperationStopped(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation commit
  virtual void onOperationCommitted(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation abort
  virtual void onOperationAborted(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation start
  /// \param theOperation a started operation
  virtual ModuleBase_Operation* currentOperation() const = 0;

  /// Add menu atems for viewer into the given menu
  /// \param theMenu a popup menu to be shown in the viewer
  /// \param theStdActions a map of standard actions
  /// \return true if items are added and there is no necessity to provide standard menu
  virtual bool addViewerMenu(QMenu* theMenu, const QMap<QString, QAction*>& theStdActions) const { return false; }

  /// Add menu atems for object browser into the given menu
  /// \param theMenu a popup menu to be shown in the object browser
  virtual void addObjectBrowserMenu(QMenu* theMenu) const {};

  /// Called when it is necessary to update a command state (enable or disable it)
  //virtual bool isFeatureEnabled(const QString& theCmdId) const = 0;

  /// Creates custom widgets for property panel
  /// \param theType a type of widget
  /// \param theParent the parent object
  /// \param theWidgetApi the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  /// \param theModelWidgets list of widget objects
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType, QWidget* theParent,
                                      Config_WidgetAPI* theWidgetApi, std::string theParentId)
  {
    return 0;
  }

  /// Returns current workshop
  ModuleBase_IWorkshop* workshop() const { return myWorkshop; }

  /// Call back forlast tuning of property panel before operation performance
  /// It is called as on clearing of property panel as on filling with new widgets
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation) {}

  //! Returns True if there are available Undos and there is not an active operation
  virtual bool canUndo() const;

  //! Returns True if there are available Redos and there is not an active operation
  virtual bool canRedo() const;

  /// Returnas true if the action can be applyed to the object
  /// \param theObject a checked object
  /// \param theActionId an identifier of action, to be found in the menu manager like "DELETE_CMD"
  /// \return the a booean result
  virtual bool canApplyAction(const ObjectPtr& theObject, const QString& theActionId) const = 0;

  /// Returns True if the current operation can be committed. By default it is true.
  /// \return a boolean value
  virtual bool canCommitOperation() const;

  /// Returns whether the object can be erased. The default realization returns true.
  /// \param theObject a model object
  virtual bool canEraseObject(const ObjectPtr& theObject) const;

  /// Returns whether the object can be displayed. The default realization returns true.
  /// \param theObject a model object
  virtual bool canDisplayObject(const ObjectPtr& theObject) const;

  /// Returns true if selection for the object can be activate.
  /// By default a result or feature of the current operation can not be activated
  /// \param theObject a model object
  virtual bool canActivateSelection(const ObjectPtr& theObject) const;

  /// Reacts to the delete action in module
  /// \returns true if the action is processed
  virtual bool deleteObjects() { return false; };

  /// Returns data model object for representation of data tree in Object browser
  virtual ModuleBase_IDocumentDataModel* dataModel() const = 0;

  /// Performs functionality on closing document
  virtual void closeDocument() = 0;

  /// Clears specific presentations in the viewer
  virtual void clearViewer() = 0;

  /// Returns a list of modes, where the AIS objects should be activated
  /// \param theModes a list of modes
  virtual void activeSelectionModes(QIntList& theModes) {}

  /** Update the object presentable properties such as color, lines width and other
  * If the object is result with the color attribute value set, it is used,
  * otherwise the customize is applyed to the object's feature if it is a custom prs
  * \param theObject an object instance
  * \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  * \returns true if the object is modified
  */
  virtual bool customizeObject(ObjectPtr theObject, const bool theUpdateViewer);

  /// This method is called on object browser creation for customisation of module specific features
  /// \param theObjectBrowser a pinter on Object Browser widget
  virtual void customizeObjectBrowser(QWidget* theObjectBrowser) {}

  /// Creates a new operation
  /// \param theCmdId the operation name
  virtual ModuleBase_Operation* createOperation(const std::string& theCmdId);

  /// Sends the operation for launching
  /// \param theOperation the operation
  virtual void sendOperation(ModuleBase_Operation* theOperation);

  //! Returns data object by AIS
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const = 0;

  /// Update state of pop-up menu items in object browser
  /// \param theStdActions - a map of standard actions
  virtual void updateObjectBrowserMenu(const QMap<QString, QAction*>& theStdActions) {}

  /// Update state of pop-up menu items in viewer
  /// \param theStdActions - a map of standard actions
  virtual void updateViewerMenu(const QMap<QString, QAction*>& theStdActions) {}

signals:
  void operationLaunched();

  void operationResumed(ModuleBase_Operation* theOp);

public slots:
  /// Called on call of command corresponded to a feature
  virtual void onFeatureTriggered();

  /// Slolt called on object display
  /// \param theObject a data object
  /// \param theAIS a presentation object
  virtual void onObjectDisplayed(ObjectPtr theObject, AISObjectPtr theAIS) {}

  /// Slot called on before object erase
  /// \param theObject a data object
  /// \param theAIS a presentation object
  virtual void onBeforeObjectErase(ObjectPtr theObject, AISObjectPtr theAIS) {}

protected slots:
  /// Called on selection changed event
  virtual void onSelectionChanged() {}

 protected:
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
