// Copyright (C) 2014-2023  CEA, EDF
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef XGUI_ActionsMgr_H
#define XGUI_ActionsMgr_H

#include "XGUI.h"

#include <Events_Listener.h>
#include <ModelAPI_Feature.h>

#include <ModuleBase_ActionInfo.h>

#include <QObject>
#include <QMap>
#include <QList>
#include <QStringList>
#include <QKeySequence>

class XGUI_Workshop;
class XGUI_OperationMgr;
class ModuleBase_Operation;
class QAction;

/// class XGUI_ActionsMgr
/// \ingroup GUI
/// A class for management of actions (features) activation/deactivation
class XGUI_EXPORT XGUI_ActionsMgr : public QObject, public Events_Listener
{
  Q_OBJECT

 public:
  /// Constructor
  /// \param theWorkshop an instance of workshop
  XGUI_ActionsMgr(XGUI_Workshop* theWorkshop);
  virtual ~XGUI_ActionsMgr();

  /// Actions on operations
  enum OperationStateActionId {
    Abort = 0,
    Accept = 1,
    AcceptPlus = 2,
    Help = 3,
    AbortAll = 4,
    AcceptAll = 5,
    Preview = 6
  };

  /// Add a command in the manager.
  /// Please note that nested commands in the Salome mode (No AppElements_Command, pure QActions)
  /// won't be extracted and should be added manually using the addNestedCommands method.
  void addCommand(QAction* theCmd);

  /// Sets relation between the command (with given Id) and it's nested actions.
  void addNestedCommands(const QString& theId, const QStringList& theCommands);

  /// Returns list of nested commands by parent command Id
  /// \param theId a parent command Id
  QStringList nestedCommands(const QString& theId) const;

  /// Returns True if the given Id is an Id of nested command
  /// \param theId an Id to check
  bool isNested(const QString& theId) const;

  /// Registers shortcut (key sequence) for the command triggering
  /// \param theKeySequence a key sequence to register
  QKeySequence registerShortcut(const QKeySequence& theKeySequence);

  /// This is an overloaded function.
  /// Registers shortcut (key sequence) for the command triggering
  /// \param theKeySequence - string that contain a key sequence to register
  QKeySequence registerShortcut(const QString& theKeySequence);

  /// Redefinition of Events_Listener method
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Return property panel's action like ok, cancel, help.
  /// If there is no such action, it will be created.
  QAction* operationStateAction(OperationStateActionId theId);

  /// Return an action by the given id, if it was registered in the manager
  QAction* action(const QString& theId);

  /// Return info (icon, text, etc) about the action by the given id,
  /// if it was registered in the manager
  ActionInfo actionInfoById(const QString& theId);

 private:
  /// Update workbench actions according to OperationMgr state:
  /// No active operations: all actions but nested are available
  /// There is active operation: current operation + it's nested
  /// are enabled, all the rest is disabled. All active commands is checked.
  void updateCommandsStatus();

  /// Sets all commands checked if it's operation is active.
  void updateCheckState();

  /// Updates actions according to current selection in the viewer
  void updateOnViewSelection();

  /// Sets all actions to enabled state.
  void setAllEnabled();

  /// Sets all nested actions to isEnabled state for the command with given ID.
  /// If ID is empty - all nested actions will be affected.
  void setNestedCommandsEnabled(bool isEnabled, const QString& theParent = QString());

  /// Sets to enabled state all siblings of the given operation and it's parents recursively
  void setNestedStackEnabled(ModuleBase_Operation* theOperation);

  /// Sets the action with theId to theChecked state.
  void setActionChecked(const QString& theId, const bool theChecked);

  /// Sets the action with theId to theEnabled state.
  void setActionEnabled(const QString& theId, const bool theEnabled);

  /// Updates actions according to their "document" tag
  void updateByDocumentKind();

  /// Asks plugins about their features state, using the Events system
  void updateByPlugins(FeaturePtr theActiveFeature);

  /// Returns names of nested action for the feature of given operation
  /// \param theOperation an operation
  QStringList allNestedCommands(ModuleBase_Operation* theOperation);

 private:

  QMap<QString, QAction*> myActions;
  QMap<QString, QStringList> myNestedActions;
  QMap<OperationStateActionId, QAction*> myOperationActions;
  QList<QKeySequence> myShortcuts;

  XGUI_Workshop* myWorkshop;
  XGUI_OperationMgr* myOperationMgr;

  friend class XGUI_Workshop;
};

#endif /* XGUI_ActionsMgr_H */

