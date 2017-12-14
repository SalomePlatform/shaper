// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

 // Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ModuleBase_IModule_H
#define ModuleBase_IModule_H

#include "ModuleBase.h"
#include "ModuleBase_IWorkshop.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_ICustomPrs.h>

#include <SelectMgr_ListOfFilter.hxx>

#include <QString>
#include <QObject>
#include <QMap>
#include <QList>

#include <string>
#include <vector>
#include <map>

class QAction;
class QMouseEvent;
class QKeyEvent;
class QMenu;
class Config_WidgetAPI;
class ModuleBase_ModelWidget;
class ModuleBase_Operation;
class ModuleBase_ViewerPrs;

class ModuleBase_IWorkshop;
class ModelAPI_Result;
class Events_Message;

class AIS_InteractiveObject;

/**
 * \ingroup GUI
 * Interface to a module
 */
class MODULEBASE_EXPORT ModuleBase_IModule : public QObject
{
  Q_OBJECT
 public:
  /// enumeration to know which objects should be customized
  enum ModuleBase_CustomizeFlag {
    CustomizeArguments = 0, /// references of other objects referenced to the current feature
    CustomizeResults, /// results of the current feature
    CustomizeHighlightedObjects /// highlighted objects of the active widget
  };

   /// Constructor
   /// \param theParent instance of workshop interface
   ModuleBase_IModule(ModuleBase_IWorkshop* theParent);

  virtual ~ModuleBase_IModule() {}

  /// Stores the current selection
  virtual void storeSelection() {}

  /// Restores the current selection
  virtual void restoreSelection() {}

  /// Reads description of features from XML file
  virtual void createFeatures();

  /// Called on creation of menu item in desktop
  virtual void actionCreated(QAction*);

  /// Launching of a edit operation on the feature
  /// \param theFeature feature for editing
  virtual void editFeature(FeaturePtr theFeature);

  /// Returns true if the operation can be committed. Result in default implementation is true.
  /// \return boolean value
  virtual bool canCommitOperation() const { return true; }

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  /// \param isStartAfterCommitOnly operation is launched if there is no active operation or
  ///        it is committed
  virtual void launchOperation(const QString& theCmdId,
                               const bool& isStartAfterCommitOnly);

  /// Executes feature as a modal dialog box
  /// \param theCmdId the operation name
  virtual void launchModal(const QString& theCmdId);

  /// Realizes some functionality by an operation start
  /// \param theOperation a started operation
  virtual void operationStarted(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation resume
  /// By default it emits operationResumed signal
  /// \param theOperation a resumed operation
  virtual void operationResumed(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation stop
  virtual void operationStopped(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation commit
  virtual void operationCommitted(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation abort
  virtual void operationAborted(ModuleBase_Operation* theOperation) {}

  /// Realizes some functionality by an operation start
  virtual ModuleBase_Operation* currentOperation() const = 0;

  /// Add menu items for viewer into the actions map
  /// \param theStdActions a map of standard actions
  /// \param theParent a parent widget
  /// \param theMenuActions map of action/menu for the desirable index in the viewer menu
  /// \return true if items are added and there is no necessity to provide standard menu
  virtual bool addViewerMenu(const QMap<QString, QAction*>& theStdActions,
                             QWidget* theParent,
                             QMap<int, QAction*>& theMenuActions) const
  { return false; }

  /// Add menu items for object browser into the given menu
  /// \param theMenu a popup menu to be shown in the object browser
  virtual void addObjectBrowserMenu(QMenu* theMenu) const {};

  /// Creates custom widgets for property panel
  /// \param theType a type of widget
  /// \param theParent the parent object
  /// \param theWidgetApi the widget configuration.
  ///                     The attribute of the model widget is obtained from
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                                     QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi)
  {
    return 0;
  }

  /// Returns the active widget, by default it is the property panel active widget
  virtual ModuleBase_ModelWidget* activeWidget() const = 0;

  /// Returns current workshop
  ModuleBase_IWorkshop* workshop() const { return myWorkshop; }

  /// Call back forlast tuning of property panel before operation performance
  /// It is called as on clearing of property panel as on filling with new widgets
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation) {}

  /// Have an opportunity to create widgets for the current operation
  /// instead of standard creation in workshop
  /// \param theOperation a started operation
  /// \param theWidgets a list of created widgets
  /// \return boolean result, false by default
  virtual bool createWidgets(ModuleBase_Operation* theOperation,
                             QList<ModuleBase_ModelWidget*>& theWidgets) const { return false; }

  //! Returns True if there are available Undos and there is not an active operation
  virtual bool canUndo() const;

  //! Returns True if there are available Redos and there is not an active operation
  virtual bool canRedo() const;

  /// Returns true if the action can be applyed to the object
  /// \param theObject a checked object
  /// \param theActionId an identifier of action, to be found in the menu manager like "DELETE_CMD"
  /// \return the a boolean result
  virtual bool canApplyAction(const ObjectPtr& theObject, const QString& theActionId) const = 0;

  /// Returns True if the current operation can be committed. By default it is true.
  /// \return a boolean value
  //virtual bool canCommitOperation() const;

  /// Returns whether the object can be erased. The default realization returns true.
  /// \param theObject a model object
  virtual bool canEraseObject(const ObjectPtr& theObject) const;

  /// Returns whether the object can be displayed. The default realization returns true.
  /// \param theObject a model object
  virtual bool canDisplayObject(const ObjectPtr& theObject) const;

  /// Returns whether the started operation may use preselection of the previous one
  /// Cases are: previous operation is null, edit operation, previuos and started operations
  /// kinds are the same
  /// \param thePreviousOperationKind a kind of previous operation
  /// \param theStartedOperationKind a kind of a started operation
  virtual bool canUsePreselection(const QString& thePreviousOperationKind,
                                  const QString& theStartedOperationKind);

  /// Make some functionality after the objects are hidden in viewer
  /// \param theObjects a list of hidden objects
  //virtual void processHiddenObject(const std::list<ObjectPtr>& theObjects) {};

  /// Returns true if selection for the object can be activate.
  /// By default a result or feature of the current operation can not be activated
  /// \param theObject a model object
  virtual bool canActivateSelection(const ObjectPtr& theObject) const;

  /// Reacts to the delete action in module
  /// \returns true if the action is processed
  virtual bool deleteObjects() { return false; };

  /// Performs functionality on closing document
  virtual void closeDocument() = 0;

  /// Clears specific presentations in the viewer
  virtual void clearViewer() = 0;

  /// Returns a list of modes, where the AIS objects should be activated
  /// \param theModes a list of modes
  virtual void activeSelectionModes(QIntList& theModes) {}

  /// Appends specific selection modes for the module to the list of types
  /// \param theModesType combination of available selection filters
  /// \param theModes a selection modes to be extended
  virtual void moduleSelectionModes(int theModesType, QIntList& theModes) = 0;

  /// Appends into container of filters module filters corresponded to the modes type
  /// \param theFilterTypes container of available selection filters
  /// \param theSelectionFilters [out] container to be extend by elements
  virtual void moduleSelectionFilters(const QIntList& theFilterTypes,
                                      SelectMgr_ListOfFilter& theSelectionFilters) = 0;

  /// Returns types of registered module selection filters
  /// \param theSelectionFilters [out] container of type value
  virtual QIntList selectionFilters() { return QIntList(); }

  /// Returns selection filter
  /// \param theType selection filter type
  /// \param theFilter instance of filter
  virtual Handle(SelectMgr_Filter) selectionFilter(const int theType) = 0;

  /// Return true if the custom presentation is activated
  /// \param theFlag a flag of level of customization, which means that only part of sub-elements
  /// \return boolean value
  virtual bool isCustomPrsActivated(const ModuleBase_CustomizeFlag& theFlag) const
  { return false; };

  /// Activate custom presentation for the object. Default realization is empty.
  /// \param theFeature a feature instance
  /// \param theFlag a flag of level of customization, which means that only part of sub-elements
  /// \param theUpdateViewer the parameter whether the viewer should be update immediately
  virtual void activateCustomPrs(const FeaturePtr& theFeature,
                                 const ModuleBase_CustomizeFlag& theFlag,
                                 const bool theUpdateViewer) {}

  /// Deactivate custom presentation for the object. Default realization is empty.
  /// \param theFlag a flag of level of customization, which means that only part of sub-elements
  /// \param theUpdateViewer the parameter whether the viewer should be update immediately
  virtual void deactivateCustomPrs(const ModuleBase_CustomizeFlag& theFlag,
                                   const bool theUpdateViewer) {}

  /// Modifies the given presentation in the custom way.
  virtual bool customisePresentation(std::shared_ptr<ModelAPI_Result> theResult,
                                     AISObjectPtr thePrs,
                                     GeomCustomPrsPtr theCustomPrs) { return false; };

  /// Modifies the given presentation in the custom way after usual customize is performed.
  virtual bool afterCustomisePresentation(std::shared_ptr<ModelAPI_Result> theResult,
                                     AISObjectPtr thePrs,
                                     GeomCustomPrsPtr theCustomPrs) { return false; };

  /// Update the object presentable properties such as color, lines width and other
  /// If the object is result with the color attribute value set, it is used,
  /// otherwise the customize is applyed to the object's feature if it is a custom prs
  /// \param theObject an object instance
  /// \param theFlag a flag of level of customization, which means that only part of sub-elements
  /// should be updated(e.g. only highlighted elements)
  /// \param theUpdateViewer the parameter whether the viewer should be update immediately
  /// \returns true if the object is modified
  virtual bool customizeObject(ObjectPtr theObject, const ModuleBase_CustomizeFlag& theFlag,
                               const bool theUpdateViewer);

  /// This method is called on object browser creation for customization of module specific features
  /// \param theObjectBrowser a pinter on Object Browser widget
  virtual void customizeObjectBrowser(QWidget* theObjectBrowser) {}

  /// Creates a new operation
  /// \param theCmdId the operation name
  virtual ModuleBase_Operation* createOperation(const std::string& theCmdId);

  /// Create specific for the module presentation
  /// \param theResult an object for presentation
  /// \return created presentation or NULL(default value)
  virtual Handle(AIS_InteractiveObject) createPresentation(
                           const std::shared_ptr<ModelAPI_Result>& theResult);

  //! Returns data object by AIS
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const = 0;

  //! Returns true if the presentation can be shown in shading mode
  //! \param theAIS presentation to be checked
  //! \return boolean value
  virtual bool canBeShaded(Handle(AIS_InteractiveObject) theAIS) const;

  /// Update state of pop-up menu items in object browser
  /// \param theStdActions - a map of standard actions
  virtual void updateObjectBrowserMenu(const QMap<QString, QAction*>& theStdActions) {}

  /// Update state of pop-up menu items in viewer
  /// \param theStdActions - a map of standard actions
  virtual void updateViewerMenu(const QMap<QString, QAction*>& theStdActions) {}

  /// Returns true if the action should be always enabled
  /// \param theActionId an action index: Accept or Accept All
  /// \return boolean value
  virtual bool isActionEnableStateFixed(const int theActionId) const { return false; }

  //! Returns the feature error if the current state of the feature in the module is not correct
  //! If the feature is correct, it returns an empty value
  //! \return string value
  virtual QString getFeatureError(const FeaturePtr& theFeature);

  /// Returns list of granted operation indices
  virtual void grantedOperationIds(ModuleBase_Operation* theOperation, QStringList& theIds) const;

  /// Connects or disconnects to the value changed signal of the property panel widgets
  /// \param theWidget a property contol widget
  /// \param isToConnect a boolean value whether connect or disconnect
  virtual void connectToPropertyPanel(ModuleBase_ModelWidget* theWidget, const bool isToConnect) {};

  /// Validates the operation to change the "Apply" button state.
  /// \param thePreviousState the previous state of the widget
  virtual void widgetStateChanged(int thePreviousState) {};

  /// Returns true if the event is processed.
  /// \param thePreviousAttributeID an index of the previous active attribute
  virtual bool processEnter(const std::string& thePreviousAttributeID) { return false; };

  /// Performs some GUI actions before an operation transaction is stopped
  /// Default realization is empty
  virtual void beforeOperationStopped(ModuleBase_Operation* theOperation) {};

  /// Finds a shape by attribute if it is possible
  /// \param theAttribute an attribute
  /// \return a geom shape
  virtual GeomShapePtr findShape(const AttributePtr& theAttribute) = 0;

  /// Finds an attribute by geom shape if it is possible
  /// \param theObject an object of the attribute
  /// \param theGeomShape a geom shape
  /// \return theAttribute
  virtual AttributePtr findAttribute(const ObjectPtr& theObject,
                                     const GeomShapePtr& theGeomShape) = 0;

  /// Returns reentrant message if it was accepted
  virtual std::shared_ptr<Events_Message> reentrantMessage() = 0;

  /// Put current selection into reentrant message
  /// \param theMessage a message of reentrant operation
  virtual void setReentrantPreSelection(const std::shared_ptr<Events_Message>& theMessage) = 0;

  /// Returns XML information by the feature index
  /// \param theFeatureId a feature id
  /// \param theXmlCfg XML configuration
  /// \param theDescription feature description
  void getXMLRepresentation(const std::string& theFeatureId, std::string& theXmlCfg,
                            std::string& theDescription);

signals:
  /// Segnal emitted when an operation is resumed
  /// \param theOp a resumed operation
  void resumed(ModuleBase_Operation* theOp);

public slots:
  /// Called on call of command corresponded to a feature
  virtual void onFeatureTriggered();

  /// Slot called on object display
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
