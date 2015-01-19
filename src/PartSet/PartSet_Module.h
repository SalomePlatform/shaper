// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"
#include "PartSet_Filters.h"
#include "PartSet_SketcherMgr.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_CompositeFeature.h>

//#include <StdSelect_FaceFilter.hxx>
#include <TopoDS_Shape.hxx>

#include <QMap>
#include <QMenu>
#include <QObject>

#include <string>

#include <memory>

class ModuleBase_Operation;
class ModuleBase_IViewWindow;

class QAction;

/**
* \ingroup Modules
* Implementation of Partset module
*/
class PARTSET_EXPORT PartSet_Module : public ModuleBase_IModule
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

  /// Creates custom widgets for property panel
  virtual QWidget* createWidgetByType(const std::string& theType, QWidget* theParent,
                                      Config_WidgetAPI* theWidgetApi, std::string theParentId,
                                      QList<ModuleBase_ModelWidget*>& theModelWidgets);

  /// Call back forlast tuning of property panel before operation performance
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation);


  /// Realizes some functionality by an operation start
  /// Displays all sketcher sub-Objects, hides sketcher result, appends selection filters
  /// \param theOperation a started operation
  virtual void operationStarted(ModuleBase_Operation* theOperation);

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

  /// Returns whether the object can be displayed at the bounds of the active operation.
  /// Display only current operation results for usual operation and ask the sketcher manager
  /// if it is a sketch operation
  /// \param theObject a model object
  virtual bool canDisplayObject(const ObjectPtr& theObject) const;
  /// Add menu atems for viewer into the given menu
  /// \param theMenu a popup menu to be shown in the viewer
  virtual void addViewerItems(QMenu* theMenu) const;

public slots:
  /// SLOT, that is called by no more widget signal emitted by property panel
  /// Set a specific flag to restart the sketcher operation
  void onNoMoreWidgets();

  /// Processes the context menu action click
  /// \param isChecked a state of toggle if the action is checkable
  void onAction(bool isChecked);

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

 private slots:
   /// Processing of vertex selected
   void onVertexSelected();

 private:
  /// Breaks sequense of automatically resterted operations
  void breakOperationSequence();

  /// Check whether there is active opeation and it is the sketch one
  /// \return boolean result
  bool isSketchOperationActive() const;

  /// Create all actions for context menus. It is called on creation of module
  /// Put the created actions into an internal map
  void createActions();

  /// Returns action according to the given ID
  /// \param theId an action identifier, it should be uniqued in the bounds of the module
  QAction* action(const QString& theId) const;

  /// Add action to the internal map
  /// \param theId - string ID of the item
  /// \param theAction - action to add
  void addAction(const QString& theId, QAction* theAction);

  //! Delete features
  void deleteObjects();

 private:
   QString myLastOperationId;
   FeaturePtr myLastFeature;

   // Automatical restarting mode flag
   RestartingMode myRestartingMode;

  /// A filter which provides selection within a current document or whole PartSet
  Handle(PartSet_GlobalFilter) myDocumentShapeFilter;

  PartSet_SketcherMgr* mySketchMgr;

  QMap<QString, QAction*> myActions; // the popup menu actions
};

#endif
