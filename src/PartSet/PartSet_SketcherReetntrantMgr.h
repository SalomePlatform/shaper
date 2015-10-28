// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PartSet_SketcherReetntrantMgr_H
#define PartSet_SketcherReetntrantMgr_H

#include "PartSet.h"

#include <ModelAPI_Feature.h>

#include <string>

#include <QObject>

class ModuleBase_IWorkshop;
class ModuleBase_Operation;
class ModuleBase_ModelWidget;
class ModuleBase_IViewWindow;

class QMouseEvent;

class XGUI_Workshop;
class PartSet_Module;

/// \ingroup PartSet_SketcherReetntrantMgr
/// It provides reentrant create operations in sketch, that is when all inputs are valid,
/// automatic validation of the creation and switch the created entity to edit mode
/// ('internal' edit operation),
/// with the ability to simultaneously create the next entity of same type (re-entrance of the operation).
/// OK valids the current edition and exits from the operation (no re-entrance).
/// Cancel removes (undo) the entity currently edited and exits from the operation (no re-entrance).
class PARTSET_EXPORT PartSet_SketcherReetntrantMgr : public QObject
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
  /// \param theParent a parent object
  PartSet_SketcherReetntrantMgr(ModuleBase_IWorkshop* theWorkshop);
  virtual ~PartSet_SketcherReetntrantMgr();

public:
  /// If the internal edit operation is started, this is the first widget of the operation
  ModuleBase_ModelWidget* activeWidget() const;

  /// Restarts sketcher operation automatically. If the commited operation is created, it starts
  /// an 'internal' edit operation for the created feature. If an 'internal' operation is committed,
  /// a create operation for a new feature with the current feature kind is started.
  /// Some internal flags are taken into accout, such as whether the previous feature should be
  /// used for initialization of created one or whether the restart of operation is forbidden.
  /// \param theOperation a committed operation
  /// \return true if an operation restarted
  bool operationCommitted(ModuleBase_Operation* theOperation);

  /// \param theOperation a committed operation
  void operationAborted(ModuleBase_Operation* theOperation);

  /// Return true if the manager processes the mouse move event
  /// It happens if the current operation is an internal edit operation and the first
  /// control can be filled by the mouse move event. The operation is committed.
  /// \return true if operation is committed.
  bool processMouseMoved(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// Return true if the manager processes the mouse press event
  /// \return true if the current operation is an internal edit operation.
  bool processMousePressed(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// Return true if the manager processes the mouse enter event
  /// It happens if the current operation is an internal edit operation.
  /// The operation is committed.
  /// \return true if operation is committed.
  bool processMouseReleased(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// Activates the operation widgets relatively the internal reentrant flags
  void propertyPanelDefined(ModuleBase_Operation* theOperation);

  /// It is called by no more widget signal emitted by property panel
  /// Set a specific flag to restart the sketcher operation
  void noMoreWidgets(const std::string& thePreviousAttributeID);

  /// Processing of vertex selected, if the feature is line, save flags to stop reentrant operation
  void vertexSelected();

  /// It is called by enter key released
  /// Set a specific type of restarting the current operation
  void enterReleased();

  /// It is called by the current operation filling with the preselection.
  /// Returns false if the reentrant mode of the operation is not empty.
  bool canBeCommittedByPreselection();

public slots:
  void onInternalActivateFirstWidgetSelection();

private:
  /// Returns true if the current operation is a sketch or a nested sketch operation
  bool isActiveMgr() const;

  /// Breaks sequense of automatically resterted operations
  void breakOperationSequence();

  /// Returns the workshop
  XGUI_Workshop* workshop() const;

  /// Returns the workshop module
  PartSet_Module* module() const;

private:
  ModuleBase_IWorkshop* myWorkshop; /// the workshop

  bool myIsInternalEditOperation; /// true when the 'internal' edit is started

  std::string myLastOperationId;
  FeaturePtr myLastFeature;

  std::string myPreviousAttributeID;
  RestartingMode myRestartingMode;  /// Automatical restarting mode flag
};

#endif
