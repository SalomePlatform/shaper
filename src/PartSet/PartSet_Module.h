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

#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"
#include "PartSet_Tools.h"
#include "PartSet_OverconstraintListener.h"
#include "PartSet_SketcherMgr.h"

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
class ModuleBase_ViewerPrs;

class XGUI_Workshop;
class PartSet_MenuMgr;
class PartSet_CustomPrs;
class PartSet_SketcherMgr;
class PartSet_SketcherReentrantMgr;
class ModelAPI_Result;

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
  RM_LastFeatureUsed, /// the operation is restarted and use the previous feature
                      /// for own initialization
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

  // Stores the current selection
  virtual void storeSelection();

  // Restores the current selection
  virtual void restoreSelection();

  /// Creates custom widgets for property panel
  virtual ModuleBase_ModelWidget* createWidgetByType(const std::string& theType, QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi);

  /// Returns the active widget, by default it is the property panel active widget
  /// If the internal edit operation is started, this is the first widget of the operation
  virtual ModuleBase_ModelWidget* activeWidget() const;

  /// Call back forlast tuning of property panel before operation performance
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation);

  /// If there is found selected attribute, widgets are created and contains
  /// only a widget for the attribute
  /// It is important for Property Panel filling by sketch point attribute
  /// \param theOperation a started operation
  /// \param theWidgets a list of created widgets
  /// \return boolean result, false by default
  virtual bool createWidgets(ModuleBase_Operation* theOperation,
                             QList<ModuleBase_ModelWidget*>& theWidgets) const;

  /// Launching of a edit operation on the feature
  /// \param theFeature feature for editing
  virtual void editFeature(FeaturePtr theFeature);

  /// Returns true if the operation can be committed. Result in default implementation is true.
  /// \return boolean value
  virtual bool canCommitOperation() const;

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  /// \param isStartAfterCommitOnly operation is launched if there is no active operation or
  ///        it is committed
  virtual void launchOperation(const QString& theCmdId,
                               const bool& isStartAfterCommitOnly);

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

  /// Returns parent result if accepted, true if the started operation is a nested operation
  /// of the previous operation
  /// \param thePreviousOperationKind a kind of previous operation
  /// \param theStartedOperationKind a kind of a new operation
  virtual bool canUsePreselection(const QString& thePreviousOperationKind,
                                  const QString& theStartedOperationKind);

  /// Make some functionality after the objects are hidden in viewer
  /// \param theObjects a list of hidden objects
  //virtual void processHiddenObject(const std::list<ObjectPtr>& theObjects);

  /// Returns true if selection for the object can be activate.
  /// For sketch operation allow the selection activation if the operation is edit, for other
  /// operation uses the default result
  /// \param theObject a model object
  virtual bool canActivateSelection(const ObjectPtr& theObject) const;

  /// Add menu atems for object browser into the given menu
  /// \param theMenu a popup menu to be shown in the object browser
  virtual void addObjectBrowserMenu(QMenu* theMenu) const;

  /// Add menu items for viewer into the actions map
  /// \param theStdActions a map of standard actions
  /// \param theParent a parent widget
  /// \param theMenuActions map of action/menu for the desirable index in the viewer menu
  /// \return true if items are added and there is no necessity to provide standard menu
  virtual bool addViewerMenu(const QMap<QString, QAction*>& theStdActions,
                             QWidget* theParent,
                             QMap<int, QAction*>& theMenuActions) const;

  /// Returns a list of modes, where the AIS objects should be activated
  /// \param theModes a list of modes
  virtual void activeSelectionModes(QIntList& theModes);

  /// Appends specific selection modes for the module to the list of types
  /// \param theTypes a selection modes to be extended
  virtual void customSubShapesSelectionModes(QIntList& theTypes);

  /// Returns whether the mouse enter the viewer's window
  /// \return true if items are added and there is no necessity to provide standard menu
  bool isMouseOverWindow();

  /// Returns sketch manager object
  PartSet_SketcherMgr* sketchMgr() const { return mySketchMgr; }

  /// Returns sketch reentrant manager
  PartSet_SketcherReentrantMgr* sketchReentranceMgr() const { return mySketchReentrantMgr; }

  /// Find object and attribute(if selected) for the given viewer selection
  /// \param theSelected a viewer selection
  /// \param theObject a selected model object
  /// \param theAttribute a selected model attribute
  virtual void getGeomSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theSelected,
                                ObjectPtr& theObject, AttributePtr& theAttribute);

  /// Returns listener of overconstraint signal
  /// \return the listener
  PartSet_OverconstraintListener* overconstraintListener() { return myOverconstraintListener; }

  /// Returns true if the current operation is not reentrant and the current state of the
  /// application is not in launch operation mode
  /// \return boolean value
  bool isSketchNeutralPointActivated() const;

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

  /// Return true if the custom presentation is activated
  /// \param theFlag a flag of level of customization, which means that only part of sub-elements
  /// \return boolean value
  virtual bool isCustomPrsActivated(const ModuleBase_CustomizeFlag& theFlag) const;

  /// Activate custom presentation for the object
  /// \param theFeature a feature instance
  /// \param theFlag a flag of level of customization, which means that only part of sub-elements
  /// \param theUpdateViewer the parameter whether the viewer should be update immediately
  virtual void activateCustomPrs(const FeaturePtr& theFeature,
                                 const ModuleBase_CustomizeFlag& theFlag,
                                 const bool theUpdateViewer);

  /// Deactivate custom presentation for the object
  /// \param theFlag a flag of level of customization, which means that only part of sub-elements
  /// \param theUpdateViewer the parameter whether the viewer should be update immediately
  virtual void deactivateCustomPrs(const ModuleBase_CustomizeFlag& theFlag,
                                   const bool theUpdateViewer);

  /// Modifies the given presentation in the custom way.
  virtual bool customisePresentation(std::shared_ptr<ModelAPI_Result> theResult,
                                     AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs);

  /// Modifies the given presentation in the custom way after usual customize is performed.
  virtual bool afterCustomisePresentation(std::shared_ptr<ModelAPI_Result> theResult,
                                          AISObjectPtr thePrs,
                                          GeomCustomPrsPtr theCustomPrs);

  /// Update the object presentable properties such as color, lines width and other
  /// If the object is result with the color attribute value set, it is used,
  /// otherwise the customize is applyed to the object's feature if it is a custom prs
  /// \param theObject an object instance
  /// \param theFlag a flag of level of customization, which means that only part of sub-elements
  /// should be updated(e.g. only highlighted elements)
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \returns true if the object is modified
  virtual bool customizeObject(ObjectPtr theObject, const ModuleBase_CustomizeFlag& theFlag,
                               const bool theUpdateViewer);

  /// This method is called on object browser creation for customisation of module specific features
  /// \param theObjectBrowser a pinter on Object Browser widget
  virtual void customizeObjectBrowser(QWidget* theObjectBrowser);

  /// Returns the viewer Z layer
  int getVisualLayerId() const { return myVisualLayerId; }

  /// Create specific for the module presentation
  /// \param theResult an object for presentation
  /// \return created presentation or NULL(default value)
  virtual Handle(AIS_InteractiveObject) createPresentation(
                              const std::shared_ptr<ModelAPI_Result>& theResult);

  //! Returns data object by AIS
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const;

  //! Returns true if the presentation can be shown in shading mode
  //! \param theAIS presentation to be checked
  //! \return boolean value
  virtual bool canBeShaded(Handle(AIS_InteractiveObject) theAIS) const;

  /// Update state of pop-up menu items in viewer
  /// \param theStdActions - a map of standard actions
  virtual void updateViewerMenu(const QMap<QString, QAction*>& theStdActions);

  /// Returns true if the action should be always enabled
  /// \param theActionId an action index: Accept or Accept All
  /// \return boolean value
  virtual bool isActionEnableStateFixed(const int theActionId) const;

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

  /// Performs some GUI actions before an operation transaction is stopped
  /// Default realization is empty
  virtual void beforeOperationStopped(ModuleBase_Operation* theOperation);

  /// Finds a shape by attribute if it is possible
  /// \param theAttribute an attribute
  /// \return a geom shape
  virtual GeomShapePtr findShape(const AttributePtr& theAttribute);

  /// Finds an attribute by geom shape if it is possible
  /// \param theObject an object of the attribute
  /// \param theGeomShape a geom shape
  /// \return theAttribute
  virtual AttributePtr findAttribute(const ObjectPtr& theObject, const GeomShapePtr& theGeomShape);

  /// Returns reentrant message if it was accepted
  virtual std::shared_ptr<Events_Message> reentrantMessage();

  /// Put current selection into reentrant message
  /// \param theMessage a message of reentrant operation
  virtual void setReentrantPreSelection(const std::shared_ptr<Events_Message>& theMessage);

  /// Returns the workshop
  XGUI_Workshop* getWorkshop() const;

public slots:
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

  /// A slot called on view window creation
  void onViewCreated(ModuleBase_IViewWindow*);

  /// A slot to change property panel title by choice type change if the title information
  /// exists in the XML definition of this control attribute
  /// \param theWidget a sender
  /// \param theIndex the current choice index
  void onChoiceChanged(ModuleBase_ModelWidget* theWidget, int theIndex);

protected:
  /// Sets the constraints states in internal map. If the feature kind is a dimensional constraint
  /// other dimensions are shown.
  /// \param theFeatureKindId a feature kind
  void storeConstraintsState(const std::string& theFeatureKindId);

  /// If the feature kind is a geometrical or dimensional constraint, set visible state for
  /// all types of constraints
  /// \param theFeatureKindId a feature kind
  void updateConstraintsState(const std::string& theFeatureKind);

  /// Register validators for this module
  virtual void registerValidators();

  /// Connects or disconnects to the value changed signal of the property panel widgets
  /// \param theWidget a property contol widget
  /// \param isToConnect a boolean value whether connect or disconnect
  virtual void connectToPropertyPanel(ModuleBase_ModelWidget* theWidget, const bool isToConnect);

  /// Updates reentrant manager state or sketcher operations for the started operation
  /// \param theOperation the started operation
  void updateSketcherOnStart(ModuleBase_Operation* theOperation);

  /// Updates presetnations of results and arguments by operation start
  /// \param theOperation the started operation
  void updatePresentationsOnStart(ModuleBase_Operation* theOperation);

 private slots:
   void onTreeViewDoubleClick(const QModelIndex&);

   void onActiveDocPopup(const QPoint&);

 private:
  //! Delete features
  virtual bool deleteObjects();

  void setDefaultConstraintShown();

private:
  bool myIsOperationIsLaunched; /// state of application between launch and stop operation
  SelectMgr_ListOfFilter mySelectionFilters;

  PartSet_SketcherMgr* mySketchMgr;
  PartSet_SketcherReentrantMgr* mySketchReentrantMgr;
  PartSet_MenuMgr* myMenuMgr;
  /// A default custom presentation, which is used for references objects of started operation
  PartSet_CustomPrs* myCustomPrs;
  PartSet_OverconstraintListener* myOverconstraintListener;
  int myVisualLayerId;

  /// backup of the visible state to restore them by operation stop
  QMap<PartSet_Tools::ConstraintVisibleState, bool> myHasConstraintShown;

  /// backup of selection in the viewer, it is used only to store selection before
  /// redisplay and restore it after
  PartSet_SketcherMgr::FeatureToSelectionMap myCurrentSelection;
  QModelIndex myActivePartIndex;
};

#endif
