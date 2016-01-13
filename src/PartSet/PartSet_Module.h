// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"
#include "PartSet_Tools.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_CompositeFeature.h>

#include <Events_Listener.h>

//#include <StdSelect_FaceFilter.hxx>
#include <TopoDS_Shape.hxx>
#include <SelectMgr_ListOfFilter.hxx>

#include <QMap>
#include <QMenu>
#include <QObject>
#include <QModelIndex>

#include <string>

#include <memory>

class ModuleBase_Operation;
class ModuleBase_IViewWindow;
class PartSet_MenuMgr;
class PartSet_CustomPrs;
class PartSet_SketcherMgr;
class PartSet_SketcherReetntrantMgr;

class QAction;

/**
* \ingroup Modules
* Implementation of Partset module
*/
class PARTSET_EXPORT PartSet_Module : public ModuleBase_IModule, public Events_Listener
{
Q_OBJECT

/// Enumeration to specify the restart operation properties.
enum RestartingMode {
  RM_None, /// the operation should not be restarted
  RM_Forbided, /// the operation should not be restarted after there is no active widget
  RM_LastFeatureUsed, /// the operation is restarted and use the previous feature for own initialization
  RM_EmptyFeatureUsed /// the operation is restarted and does not use the previous feature
};

public:

  /// Constructor
  /// \param theWshop a pointer to a workshop
  PartSet_Module(ModuleBase_IWorkshop* theWshop);
  virtual ~PartSet_Module();

  // Add default selection filters of the module to the current viewer
  virtual void activateSelectionFilters();
  // Remove default selection filters of the module from the current viewer
  virtual void deactivateSelectionFilters();

  /// Creates custom widgets for property panel
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType, QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi, std::string theParentId);

  /// Returns the active widget, by default it is the property panel active widget
  /// If the internal edit operation is started, this is the first widget of the operation
  virtual ModuleBase_ModelWidget* activeWidget() const;

  /// Call back forlast tuning of property panel before operation performance
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation);

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  virtual void launchOperation(const QString& theCmdId);

  /// Realizes some functionality by an operation start
  /// Displays all sketcher sub-Objects, hides sketcher result, appends selection filters
  /// Activate the operation presentation
  /// \param theOperation a started operation
  virtual void operationStarted(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation resume
  /// Activate the operation presentation
  /// \param theOperation a resumed operation
  virtual void operationResumed(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation commit
  /// Restarts sketcher operation automatically of it is necessary
  /// \param theOperation a committed operation
  virtual void operationCommitted(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation abort
  /// Hides all sketcher sub-Objects, displays sketcher result and removes selection filters
  /// \param theOperation an aborted operation
  virtual void operationAborted(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation stop
  /// Hides all sketcher sub-Objects, displays sketcher result and removes selection filters
  /// \param theOperation a stopped operation
  virtual void operationStopped(ModuleBase_Operation* theOperation);

  /// Returns current operation
  virtual ModuleBase_Operation* currentOperation() const;

  /// Returns True if there are available Undos and the sketch manager allows undo
  /// \return the boolean result
  virtual bool canUndo() const;

  //! Returns True if there are available Redos and the sketch manager allows redo
  /// \return the boolean result
  virtual bool canRedo() const;

  /// Returnas true if the action can be applyed to the object
  /// \param theObject a checked object
  /// \param theActionId an identifier of action, to be found in the menu manager like "DELETE_CMD"
  /// \return the a booean result
  virtual bool canApplyAction(const ObjectPtr& theObject, const QString& theActionId) const;

  /// Returns True if the current operation can be committed. Asks the sketch manager.
  /// \return a boolean value
  //virtual bool canCommitOperation() const;

  /// Returns whether the object can be erased at the bounds of the active operation.
  /// The sub-objects of the current operation can not be erased
  /// \param theObject a model object
  virtual bool canEraseObject(const ObjectPtr& theObject) const;

  /// Returns whether the object can be displayed at the bounds of the active operation.
  /// Display only current operation results for usual operation and ask the sketcher manager
  /// if it is a sketch operation
  /// \param theObject a model object
  virtual bool canDisplayObject(const ObjectPtr& theObject) const;

  /// Returns true if selection for the object can be activate.
  /// For sketch operation allow the selection activation if the operation is edit, for other
  /// operation uses the default result
  /// \param theObject a model object
  virtual bool canActivateSelection(const ObjectPtr& theObject) const;

  /// Add menu atems for object browser into the given menu
  /// \param theMenu a popup menu to be shown in the object browser
  virtual void addObjectBrowserMenu(QMenu* theMenu) const;

  /// Add menu atems for viewer into the given menu
  /// \param theMenu a popup menu to be shown in the viewer
  /// \param theStdActions a map of standard actions
  /// \return true if items are added and there is no necessity to provide standard menu
  virtual bool addViewerMenu(QMenu* theMenu, const QMap<QString, QAction*>& theStdActions) const;

  /// Returns a list of modes, where the AIS objects should be activated
  /// \param theModes a list of modes
  virtual void activeSelectionModes(QIntList& theModes);

  /// Returns whether the mouse enter the viewer's window
  /// \return true if items are added and there is no necessity to provide standard menu
  bool isMouseOverWindow();

  /// Returns sketch manager object
  PartSet_SketcherMgr* sketchMgr() const { return mySketchMgr; }

  /// Returns sketch reentrant manager
  PartSet_SketcherReetntrantMgr* sketchReentranceMgr() { return mySketchReentrantMgr; }

  /// Performs functionality on closing document
  virtual void closeDocument();

  /// Clears specific presentations in the viewer
  virtual void clearViewer();

  /// Event Listener method
  /// \param theMessage an event message
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Set the object with the object results are customized
  /// \param theFeature a feature
  void setCustomized(const FeaturePtr& theFeature);

  /** Update the object presentable properties such as color, lines width and other
  * If the object is result with the color attribute value set, it is used,
  * otherwise the customize is applyed to the object's feature if it is a custom prs
  * \param theObject an object instance
  * \param theFlag a flag of level of customization, which means that only part of sub-elements
  * should be updated(e.g. only highlighted elements)
  * \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  * \returns true if the object is modified
  */
  virtual bool customizeObject(ObjectPtr theObject, const ModuleBase_CustomizeFlag& theFlag,
                               const bool theUpdateViewer);

  /// This method is called on object browser creation for customisation of module specific features
  /// \param theObjectBrowser a pinter on Object Browser widget
  virtual void customizeObjectBrowser(QWidget* theObjectBrowser);

  /// Returns the viewer Z layer
  int getVisualLayerId() const { return myVisualLayerId; }

  //! Returns data object by AIS
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const;

  /// Update state of pop-up menu items in viewer
  /// \param theStdActions - a map of standard actions
  virtual void updateViewerMenu(const QMap<QString, QAction*>& theStdActions); 

  //! Returns the feature error if the current state of the feature in the module is not correct
  //! If the feature is correct, it returns an empty value
  //! \return string value
  virtual QString getFeatureError(const FeaturePtr& theFeature);

  /// Returns list of granted operation indices
  virtual void grantedOperationIds(ModuleBase_Operation* theOperation, QStringList& theIds) const;

  /// Validates the current operation and send the state change to sketch manager
  /// \param thePreviousState the previous widget value state
  virtual void widgetStateChanged(int thePreviousState);

  /// Returns true if the event is processed. It gives the reentrance manager to process the enter.
  /// \param thePreviousAttributeID an index of the previous active attribute
  virtual bool processEnter(const std::string& thePreviousAttributeID);

  /// Performs some GUI actions after an operation transaction is opened
  /// Default realization is empty
  virtual void beforeOperationStarted(ModuleBase_Operation* theOperation);

  /// Performs some GUI actions before an operation transaction is stopped
  /// Default realization is empty
  virtual void beforeOperationStopped(ModuleBase_Operation* theOperation);

public slots:
  /// Redefines the parent method in order to customize the next case:
  /// If the sketch nested operation is active and the presentation is not visualized in the viewer,
  /// the operation should be always aborted.
  virtual void onFeatureTriggered();

  /// Slolt called on object display
  /// \param theObject a data object
  /// \param theAIS a presentation object
  virtual void onObjectDisplayed(ObjectPtr theObject, AISObjectPtr theAIS);

  /// Slot called on before object erase
  /// \param theObject a data object
  /// \param theAIS a presentation object
  virtual void onBeforeObjectErase(ObjectPtr theObject, AISObjectPtr theAIS);

  /// Called on transformation in current viewer
  /// \param theTrsfType type of tranformation
  void onViewTransformed(int theTrsfType = 2);

protected slots:
  /// Called when previous operation is finished
  virtual void onSelectionChanged();

  /// SLOT, that is called by key release in the viewer.
  /// \param theWnd a view window
  /// \param theEvent the key event
  void onKeyRelease(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent);

  /// SLOT, that is called by the current operation filling with the preselection.
  /// It commits the operation of it is can be committed
  void onOperationActivatedByPreselection();

  /// A slot called on view window creation
  void onViewCreated(ModuleBase_IViewWindow*);

  /// A slot to change property panel title on change of boolean operation type
  /// \param theOperation the operation type
  void onBooleanOperationChange(int theOperation);

protected:
  /// Register validators for this module
  virtual void registerValidators();

  /// Register selection filters for this module
  virtual void registerFilters();

  /// Register properties of this module
  virtual void registerProperties();

  /// Connects or disconnects to the value changed signal of the property panel widgets
  /// \param theWidget a property contol widget
  /// \param isToConnect a boolean value whether connect or disconnect
  virtual void connectToPropertyPanel(ModuleBase_ModelWidget* theWidget, const bool isToConnect);

 private slots:
   void onTreeViewDoubleClick(const QModelIndex&);

   void onActiveDocPopup(const QPoint&);

 private:

  //! Delete features
  virtual bool deleteObjects();

 private:
  SelectMgr_ListOfFilter mySelectionFilters;

  PartSet_SketcherMgr* mySketchMgr;
  PartSet_SketcherReetntrantMgr* mySketchReentrantMgr;
  PartSet_MenuMgr* myMenuMgr;
  /// A default custom presentation, which is used for references objects of started operation
  PartSet_CustomPrs* myCustomPrs;
  int myVisualLayerId;

  /// backup of the visible state to restore them by operation stop
  QMap<PartSet_Tools::ConstraintVisibleState, bool> myHasConstraintShown;

  QModelIndex aActivePartIndex;
};

#endif
