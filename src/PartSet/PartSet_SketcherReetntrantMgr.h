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
/// ('internal' edit operation), with the ability to simultaneously create the next entity
/// of same type (re-entrance of the operation).
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
  /// Returns a first widget of the current opeation if the internal edit operation is active
  /// or return null. If the current widget of the operation is a viewer selector, it returns null.
  ModuleBase_ModelWidget* internalActiveWidget() const;

  /// Return true if the current edit operation is an internal
  bool isInternalEditActive() const;

  /// if the internal flags allow it and the manager is active, it starts an internal edit operation
  /// for the created operation.
  /// \param thePreviousAttributeID an index of the previous active attribute
  //bool restartOperation(const std::string& thePreviousAttributeID);
  bool processEnter(const std::string& thePreviousAttributeID);

  /// Resets the internal flags
  /// \param theOperation a started operation
  void operationStarted(ModuleBase_Operation* theOperation);

  /// Resets the internal flags
  /// \param theOperation a started operation
  /// \return state whether the internal edit operation was active
  bool operationCommitted(ModuleBase_Operation* theOperation);

  /// Resets the internal flags
  /// \param theOperation a started operation
  void operationAborted(ModuleBase_Operation* theOperation);

  /// Return true if the manager processes the mouse move event
  /// It happens if the current operation is an internal edit operation and the first
  /// control can be filled by the mouse move event. The operation is restarted.
  /// \return true if operation is committed.
  bool processMouseMoved(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// Return true if the manager processes the mouse press event
  /// \return true if the current operation is an internal edit operation.
  bool processMousePressed(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// Return true if the manager processes the mouse enter event
  /// It happens if the current operation is an internal edit operation.
  /// The operation is restarted. If the first widget of the started operation is
  /// the point 2d, it processes this mouse event
  /// \return true if operation is committed.
  bool processMouseReleased(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// It is called by the current operation filling with the preselection.
  /// Returns false if the reentrant mode of the operation is not empty.
  bool canBeCommittedByPreselection();

private slots:
  /// SLOT, that is called by a widget activating in the property panel
  /// If the 'internal' edit operation is started, it activates the first widget selection
  void onWidgetActivated();

  /// SLOT, that is called by no more widget signal emitted by property panel
  /// Start an internal edit operation or, if the internal flag is forbided, commits
  /// the current operation
  /// \param thePreviousAttributeID an index of the previous active attribute
  void onNoMoreWidgets(const std::string& thePreviousAttributeID);

  /// Processing of vertex selected. Set an internal reentrant flag to forbiddent state if
  /// the current feature is a line and there are not obligate widgets anymore
  void onVertexSelected();

  /// Deactivates selection and filters of the first operation widget if it is an internal
  /// 'edit' operation
  void onBeforeStopped();

private:
  /// Returns true if the current operation is a sketch or a nested sketch operation
  bool isActiveMgr() const;

  /// Sets the focus to the last control of the property panel and activates selection
  /// of the first widget to can select first value of the next create operation
  /// \param thePreviousAttributeID an index of the previous attribute to set focus to this widget
  void startInternalEdit(const std::string& thePreviousAttributeID);

  /// Disconnects this manager from operation signals, deactivate selection of the first control
  /// in the viewer.
  void beforeStopInternalEdit();

  /// Commits the current operation and launches a new with the commited operation feature index
  void restartOperation();

  /// Breaks sequense of automatically resterted operations
  void resetFlags();

  /// Returns the workshop
  XGUI_Workshop* workshop() const;

  /// Returns the workshop module
  PartSet_Module* module() const;

private:
  ModuleBase_IWorkshop* myWorkshop; /// the workshop

  RestartingMode myRestartingMode;  /// automatical restarting mode flag
  bool myIsFlagsBlocked; /// true when reset of flags should not be perfromed
  bool myIsInternalEditOperation; /// true when the 'internal' edit is started
};

#endif
