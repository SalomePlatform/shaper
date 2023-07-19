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

#ifndef PartSet_MenuMgr_H
#define PartSet_MenuMgr_H

#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultPart.h>

#include <QObject>
#include <QMap>
#include <QString>
#include <QList>
#include <QColor>

class ModuleBase_Operation;
class PartSet_Module;
class QAction;
class QMenu;

/**
* \ingroup Modules
* A class for management of module specific menu
*/
class PartSet_MenuMgr: public QObject
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theModule a parent module
  PartSet_MenuMgr(PartSet_Module* theModule);


  /// Returns action according to the given ID
  /// \param theId an action identifier, it should be uniqued in the bounds of the module
  QAction* action(const QString& theId) const;

  /// Add menu items for viewer into the actions map
  /// \param theStdActions a map of standard actions
  /// \param theParent a parent widget for the
  /// \param theMenuActions map of action/menu for the desirable index in the viewer menu
  /// \return true if items are added and there is no necessity to provide standard menu
  bool addViewerMenu(const QMap<QString, QAction*>& theStdActions,
                     QWidget* theParent,
                     QMap<int, QAction*>& theMenuActions) const;

  /// Update state of pop-up menu items in viewer
  /// \param theStdActions - a map of standard actions
  void updateViewerMenu(const QMap<QString, QAction*>& theStdActions);

  /// Activates a PartSet document
  void activatePartSet() const;

  /// Returns list of granted operation indices
  virtual void grantedOperationIds(ModuleBase_Operation* theOperation, QStringList& theIds) const;

  /// Activates a Part document
  void activatePart(ResultPartPtr thePart) const;

public slots:
  /// Processes the context menu action click
  /// \param isChecked a state of toggle if the action is checkable
  void onAction(bool isChecked);

private slots:

  /// A slot which is called on selection of menu item coresponding to line with coincedence
  /// \param theAction an action of the selected item
  void onLineHighlighted(QAction* theAction);

  /// A slot which is called on hiding Detach menu
  void onDetachMenuHide();

  /// A slot which is called on selection an Item in Detach menu
  /// \param theAction an action of the selected item
  void onLineDetach(QAction* theAction);

  /// A slot called on Part activation command
  void onActivatePart(bool);

  /// A slot called on PartSet activation command
  void onActivatePartSet(bool);

  /// A slot called on edit of feature
  void onEdit(bool);

  /// Activates all not loaded parts
  void onActivateAllParts();

protected:
  /// Redefinition of virtual method
  /// \param theObj an object
  /// \param theEvent an event
  virtual bool eventFilter(QObject* theObj, QEvent* theEvent);

private:
  /// Returns true if the current operation is sketch entity create operation
  /// \param theValue the current auxiliary value
  /// \return the boolean result
  bool canSetAuxiliary(bool& theValue) const;

  /// Changes the sketcher entity construction argument value
  /// \param isChecked if true, the feature is a construction
  void setAuxiliary(const bool isChecked);

  /// Create all actions for context menus. It is called on creation of module
  /// Put the created actions into an internal map
  void createActions();

  /// Add action to the internal map
  /// \param theId - string ID of the item
  /// \param theAction - action to add
  void addAction(const QString& theId, QAction* theAction);

  /// Set color on presentation of result listed in myCoinsideLines
  /// \param theId object Id in myCoinsideLines list
  /// \param theColor a color which has to be set
  /// \param theUpdate update viewer flag
  /// \return previously defined color on the object
  QColor setLineColor(int theId, const QColor theColor, bool theUpdate);

  /// Reference to a parent module
  PartSet_Module* myModule;

  /// the popup menu actions
  QMap<QString, QAction*> myActions;

  /// List of lines coincided in a one point
  mutable QList<FeaturePtr> myCoinsideLines;

  /// A Coincedence feature selected by user for detaching
  mutable FeaturePtr mySelectedFeature;

  /// Id of menu item in Detach menu previously selected
  int myPrevId;

  /// Original color of highlighted line
  QColor myColor;
};

#endif