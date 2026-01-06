// Copyright (C) 2014-2026  CEA/DEN, EDF R&D
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

#ifndef XGUI_SketchConstraintsBrowser_H
#define XGUI_SketchConstraintsBrowser_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Folder.h>
#include <XGUI_DataModel.h>
#include <ModelAPI_Attribute.h>

#include <QWidget>
#include <QTreeView>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>
#include <QCheckBox>
#include <QScrollArea>
#include <QTreeWidget>

class ModuleBase_IDocumentDataModel;
class XGUI_DataModel;
class Config_DataModelReader;
class XGUI_Workshop;
class XGUI_ConstraintsItemDelegate;
class ModuleBase_ITreeNode;

//#define DEBUG_INDXES

struct FeatStruct
{
  FeaturePtr Feature;
  std::vector<AttributePtr> Attributes;
};


class XGUI_EXPORT XGUI_ConstraintsViewTree : public QTreeWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent widget
  XGUI_ConstraintsViewTree(QWidget* theParent = 0) : QTreeWidget(theParent) {}

  /// Returns current data model
  XGUI_DataModel* dataModel() const
  {
    return static_cast<XGUI_DataModel*>(model());
  }

signals:
  //! Emitted on context menu request
  void contextMenuRequested(QContextMenuEvent* theEvent);

protected slots:
  /// Redefinition of virtual method
  virtual void contextMenuEvent(QContextMenuEvent* theEvent)
  {
    emit contextMenuRequested(theEvent);
  }

  void closeEditor(QWidget* theEditor, QAbstractItemDelegate::EndEditHint theHint);

};

/**\class XGUI_SketchConstraintsBrowser
 * \ingroup GUI
 * \brief Object browser window object. Represents data tree of current data structure
 */
class XGUI_EXPORT XGUI_SketchConstraintsBrowser : public QWidget, public Events_Listener
{
Q_OBJECT
 public:

  // Temporary for more simple modification
  XGUI_ConstraintsViewTree* getViewTree() { return myViewTree; }

  bool IsInEditMode();

  // Make more good option
  bool UpdateTree(const std::vector<std::pair<FeaturePtr, std::vector<AttributePtr>>>& theList);

  void CloseEditor();

   /// Constructor
   /// \param theParent a parent widget
  XGUI_SketchConstraintsBrowser(QWidget* theParent, XGUI_Workshop* theWorkshop);
  virtual ~XGUI_SketchConstraintsBrowser();

  /// Event Listener method
  /// \param theMessage an event message
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  //! Returns list of currently selected constraints and geometries
  //! \param theIndexes - output list of corresponded indexes (can be NULL)
  QObjectPtrList selectedObjects(QModelIndexList* theIndexes = 0) const;

  //! Returns list of currently selected constraints in browser
  //! \param theIndexes - output list of corresponded indexes (can be NULL)
  QObjectPtrList selectedConstraints(QModelIndexList* theIndexes = 0) const;

  /// Set selected list of objects
  /// \param theObjects list of objects to select
  void setObjectsSelected(const QObjectPtrList& theObjects);

  //! Returns currently selected indexes
  QModelIndexList selectedIndexes() const
  {
    if (myViewTree->selectionModel())
      return myViewTree->selectionModel()->selectedIndexes();
    else
      return QModelIndexList();
  }

  /// Initialize the Object browser
  void initialize(ModuleBase_ITreeNode* theRoot);

  /// Returns current workshop
  XGUI_Workshop* workshop() const { return myWorkshop; }

  void onSelectionChanged();

public slots:
  //! Called on Edit command request
  void onEditItem();

  //! Change state of constraints
  void onDeactivateItems();

private slots:
  void SelectStateChanged(bool theState);

signals:
  //! Emitted when selection is changed
  void selectionChanged();

  //! Emitted on context menu request
  void contextMenuRequested(QContextMenuEvent* theEvent);

  //! An signal emitted on resize of the Object Browser
  void sizeChanged();

  void editValues();
  void deleteConstraints();
  void deactivate(bool, std::vector<FeaturePtr>);

protected:
  //! redefinition of a virtual method
  void resizeEvent(QResizeEvent* theEvent);

 private slots:
  /// Show context menu
  /// \param theEvent a context menu event
  void onContextMenuRequested(QContextMenuEvent* theEvent);

  //! Called when selection in Data Tree is changed
  void onSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected);

  /// Slot for reaction on double click in the table (start editing)
  /// \param theIndex the clicked index
  void onDoubleClick(const QModelIndex& theIndex);

 private:
  XGUI_Workshop* myWorkshop;

  XGUI_ConstraintsViewTree* myViewTree;
  QVBoxLayout* myLayout;
  QHBoxLayout* myButtons;

  bool myLastState; //Store state of Extended Information CheckBox (need for correct reset after edit constraints)
  QCheckBox* myExtInfo;
  XGUI_ConstraintsItemDelegate* myDelegate;

  std::map<std::string, std::vector<FeatStruct>> myConstrs; // string - name of group, vector - constraints from group
};

#endif
