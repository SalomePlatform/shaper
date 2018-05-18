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

#ifndef PartSet_SketcherReentrantMgr_H
#define PartSet_SketcherReentrantMgr_H

#include "PartSet.h"

#include <ModelAPI_Feature.h>

#include <string>
#include <memory>

#include <QObject>

class ModuleBase_IWorkshop;
class ModuleBase_Operation;
class ModuleBase_ModelWidget;
class ModuleBase_IViewWindow;

class ModelAPI_CompositeFeature;

class QMouseEvent;

class XGUI_Workshop;
class PartSet_Module;
class ModuleBase_ViewerPrs;
class Events_Message;
class ModelAPI_Attribute;
class GeomAPI_Pnt2d;

/// \ingroup PartSet_SketcherReentrantMgr
/// It provides reentrant create operations in sketch, that is when all inputs are valid,
/// automatic validation of the creation and switch the created entity to edit mode
/// ('internal' edit operation), with the ability to simultaneously create the next entity
/// of same type (re-entrance of the operation).
/// OK valids the current edition and exits from the operation (no re-entrance).
/// Cancel removes (undo) the entity currently edited and
/// exits from the operation (no re-entrance).
class PARTSET_EXPORT PartSet_SketcherReentrantMgr : public QObject
{
Q_OBJECT

/// Enumeration to specify the restart operation properties.
enum RestartingMode {
  RM_None, /// the operation should not be restarted
  RM_Forbided, /// the operation should not be restarted after there is no active widget
  RM_LastFeatureUsed, /// the operation is restarted and use
                      /// the previous feature for own initialization
  RM_EmptyFeatureUsed /// the operation is restarted and does not use the previous feature
};

public:
  /// Constructor
  /// \param theWorkshop a workshop instance
  PartSet_SketcherReentrantMgr(ModuleBase_IWorkshop* theWorkshop);
  virtual ~PartSet_SketcherReentrantMgr();

public:
  /// Return true if the current edit operation is an internal
  bool isInternalEditActive() const;

  /// Stop internal edit if the operation feature is invalid
  void updateInternalEditActiveState();

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

  /// Fills reentrant message during restarting operation
  /// \param theMessage reentrant message
  void setReentrantMessage(const std::shared_ptr<Events_Message>& theMessage)
  { myReentrantMessage = theMessage; }

  /// Returnss reentrant message
  std::shared_ptr<Events_Message> reentrantMessage() const { return myReentrantMessage; }

  /// Put current selection into reentrant message
  /// \param theMessage a message of reentrant operation
  void setReentrantPreSelection(const std::shared_ptr<Events_Message>& theMessage);

private slots:
  /// SLOT, that is called by a widget activating in the property panel
  /// If the 'internal' edit operation is started, it activates the first widget selection
  //void onWidgetActivated();

  /// SLOT, that is called by no more widget signal emitted by property panel
  /// Start an internal edit operation or, if the internal flag is forbided, commits
  /// the current operation
  /// \param thePreviousAttributeID an index of the previous active attribute
  void onNoMoreWidgets(const std::string& thePreviousAttributeID);

  /// Processing of vertex selected. Set an internal reentrant flag to forbiddent state if
  /// the current feature is a line and there are not obligate widgets anymore
  void onVertexSelected();

  /// Listens to the signal about the modification of the values
  /// have been done in the property panel. If the manager has active edit operation and
  /// the active widget does not process the modification of value, the manager will
  /// restart current operation and fill a new feature attribute by the value of current
  /// widget
  void onAfterValuesChangedInPropertyPanel();

  /// Deactivates selection and filters of the first operation widget if it is an internal
  /// 'edit' operation
  void onBeforeStopped();

private:
  /// Returns true if the current operation is a sketch or a nested sketch operation
  bool isActiveMgr() const;

  /// Sets the focus to the last control of the property panel and activates selection
  /// of the first widget to can select first value of the next create operation
  /// \param thePreviousAttributeID an index of the previous attribute to set focus to this widget
  /// \return true if it is started
  bool startInternalEdit(const std::string& thePreviousAttributeID);

  /// Disconnects this manager from operation signals, deactivate selection of the first control
  /// in the viewer.
  void beforeStopInternalEdit();

  /// Commits the current operation and launches a new with the commited operation feature index
  void restartOperation();

  /// Creates an internal feature and controls to process it
  void createInternalFeature();

  /// A pair method for an internal creation to remove it and clear all created controls
  void deleteInternalFeature();

  /// Breaks sequense of automatically resterted operations
  void resetFlags();

  /// Copy some feature specific attributes from the source to a new feature
  /// This is type for Circle and Arc features
  /// \param theSourceFeature a source feature
  /// \param theNewFeature a new feature
  /// \param theSketch an active sketch
  /// \param isTemporary is used to do not create additional features(e.g. coicidence for line)
  /// \return true is something is copied
  static bool copyReetntrantAttributes(const FeaturePtr& theSourceFeature,
                                      const FeaturePtr& theNewFeature,
                                      const std::shared_ptr<ModelAPI_CompositeFeature>& theSketch,
                                      const bool isTemporary = false);

  /// Checks whethe the feature of the given operation has kind an arc and the arc type is tangent
  bool isTangentArc(ModuleBase_Operation* theOperation,
                    const std::shared_ptr<ModelAPI_CompositeFeature>& /*theSketch*/) const;

  /// Accept All action is enabled if an internal edit is started.
  /// It updates the state of the button
  void updateAcceptAllAction();

  /// Returns the workshop
  XGUI_Workshop* workshop() const;

  /// Returns the workshop module
  PartSet_Module* module() const;

  void setInternalActiveWidget(ModuleBase_ModelWidget* theWidget);

private:
  ModuleBase_IWorkshop* myWorkshop; /// the workshop

  RestartingMode myRestartingMode;  /// automatical restarting mode flag
  bool myIsFlagsBlocked; /// true when reset of flags should not be perfromed
  bool myIsInternalEditOperation; /// true when the 'internal' edit is started

  FeaturePtr myPreviousFeature; /// feature of the previous operation, which is restarted
  FeaturePtr myInternalFeature;
  QWidget* myInternalWidget;
  std::string myNoMoreWidgetsAttribute;

  std::shared_ptr<Events_Message> myReentrantMessage; /// message obtained by operation restart
  ObjectPtr mySelectedObject; /// cashed selected object
  std::shared_ptr<ModelAPI_Attribute> mySelectedAttribute; /// cashed selected attribute
  std::shared_ptr<GeomAPI_Pnt2d> myClickedSketchPoint; /// cashed clicked point
};

#endif
