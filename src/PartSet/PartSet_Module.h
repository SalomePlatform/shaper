// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"
#include "PartSet_DocumentDataModel.h"

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

#include <string>

#include <memory>

class ModuleBase_Operation;
class ModuleBase_IViewWindow;
class PartSet_MenuMgr;
class PartSet_SketcherMgr;

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

  /// Call back forlast tuning of property panel before operation performance
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation start
  /// Displays all sketcher sub-Objects, hides sketcher result, appends selection filters
  /// \param theOperation a started operation
  virtual void onOperationStarted(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation commit
  /// Restarts sketcher operation automatically of it is necessary
  /// \param theOperation a committed operation
  virtual void onOperationCommitted(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation abort
  /// Hides all sketcher sub-Objects, displays sketcher result and removes selection filters
  /// \param theOperation an aborted operation
  virtual void onOperationAborted(ModuleBase_Operation* theOperation);

  /// Realizes some functionality by an operation stop
  /// Hides all sketcher sub-Objects, displays sketcher result and removes selection filters
  /// \param theOperation a stopped operation
  virtual void onOperationStopped(ModuleBase_Operation* theOperation);

  /// Returns current operation
  virtual ModuleBase_Operation* currentOperation() const;

  /// Returns True if there are available Undos and the sketch manager allows undo
  /// \return the boolean result
  virtual bool canUndo() const;

  //! Returns True if there are available Redos and the sketch manager allows redo
  /// \return the boolean result
  virtual bool canRedo() const;

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

  PartSet_SketcherMgr* sketchMgr() const { return mySketchMgr; }

  /// Returns data model object for representation of data tree in Object browser
  virtual ModuleBase_IDocumentDataModel* dataModel() const { return myDataModel; }

  /// Event Listener method
  /// \param theMessage an event message
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// This method is called on object browser creation for customisation of module specific features
  /// \param theObjectBrowser a pinter on Object Browser widget
  virtual void customizeObjectBrowser(QWidget* theObjectBrowser);

public slots:
  /// SLOT, that is called by no more widget signal emitted by property panel
  /// Set a specific flag to restart the sketcher operation
  void onNoMoreWidgets();

  /// Redefines the parent method in order to customize the next case:
  /// If the sketch nested operation is active and the presentation is not visualized in the viewer,
  /// the operation should be always aborted.
  virtual void onFeatureTriggered();

  /// Slolt called on object display
  /// \param theObject a data object
  /// \param theAIS a presentation object
  virtual void onObjectDisplayed(ObjectPtr theObject, AISObjectPtr theAIS);

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

  /// SLOT, that is called by enter key released
  /// Set a specific type of restarting the current operation
  void onEnterReleased();

  /// SLOT, that is called by the current operation filling with the preselection.
  /// It commits the operation of it is can be committed
  void onOperationActivatedByPreselection();

 protected:
  /// Register validators for this module
  virtual void registerValidators();

  /// Register selection filters for this module
  virtual void registerFilters();

  /// Register properties of this module
  virtual void registerProperties();

 private slots:
   /// Processing of vertex selected
   void onVertexSelected();

   void onTreeViewDoubleClick(const QModelIndex&);

 private:
  /// Breaks sequense of automatically resterted operations
  void breakOperationSequence();

  //! Delete features
  virtual bool deleteObjects();

 private:
   QString myLastOperationId;
   FeaturePtr myLastFeature;

   // Automatical restarting mode flag
   RestartingMode myRestartingMode;

  SelectMgr_ListOfFilter mySelectionFilters;

  PartSet_SketcherMgr* mySketchMgr;

  PartSet_MenuMgr* myMenuMgr;

  int myVisualLayerId;

  PartSet_DocumentDataModel* myDataModel;
};

#endif
